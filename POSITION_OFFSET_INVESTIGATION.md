# Position Offset Issue - Investigation Report

## Problem Summary

When using Vuforia AR tracking on Meta Quest with a custom External Camera Driver (feeding PassthroughCameraAccess frames to Vuforia), Image Target tracking exhibits:

- **✅ CORRECT**: Rotation/orientation is accurate
- **❌ INCORRECT**: Position has ~4-5cm offset
- **🔄 FLIP BEHAVIOR**: Offset direction flips when switching between Left and Right cameras
  - Left camera: Target appears ~4-5cm too far RIGHT
  - Right camera: Target appears ~4-5cm too far LEFT

## System Architecture

### Hardware
- Meta Quest 3 (tested with both Left and Right passthrough cameras)
- Camera resolution: 1280x960 @ 30fps
- Physical camera offset (LensOffset) from head center:
  - **Left camera**: (-0.032, -0.018, 0.063) meters = (-3.2cm left, -1.8cm down, +6.3cm forward)
  - **Right camera**: (+0.032, -0.018, 0.062) meters = (+3.2cm right, -1.8cm down, +6.2cm forward)

### Software Stack

**Unity Layer** (`Assets/QuestVuforia/`):
- `MetaCameraProvider.cs`: Captures frames from PassthroughCameraAccess, extracts RGB data, feeds to native plugin
- `QuestVuforiaBridge.cs`: P/Invoke bridge to native plugin (FeedDevicePose, FeedCameraFrame)
- `QuestVuforiaDriverInit.cs`: Initializes Vuforia Driver Framework

**Native C++ Layer** (`QuforiaPlugin/src/`):
- `external_tracker.cpp`: Transforms camera poses from Unity convention to Vuforia CV convention
- `vuforia_driver.cpp`: Implements Vuforia Driver Framework External Camera API
- `external_camera.cpp`: Camera lifecycle and frame delivery
- `quforia_jni.cpp`: JNI bridge for Unity P/Invoke

**Vuforia Integration**:
- Uses Vuforia Engine 11.4.4 with Driver Framework (External Camera API)
- Standard Vuforia Unity SDK components: `VuforiaBehaviour`, `ImageTargetBehaviour`
- Vuforia components placed on `CenterEyeAnchor` GameObject

### Coordinate Systems

**Unity/OpenXR Convention** (Left-handed):
- X: Right
- Y: Up
- Z: Back (toward user)

**Vuforia CV Convention** (Right-handed):
- X: Right
- Y: Down
- Z: Forward (into scene)

Transformation: Flip Y and Z axes (equivalent to 180° rotation around X-axis)

## Investigation Timeline

### Initial Observations

1. **Camera Pose Source**: `PassthroughCameraAccess.GetCameraPose()` correctly returns camera world pose with LensOffset applied:
   ```csharp
   var headPose = OVRPlugin.GetNodePoseStateAtTime(..., OVRPlugin.Node.Head);
   var lensOffset = Intrinsics.LensOffset;
   return new Pose(
       headPose.position + headPose.orientation * lensOffset.position,
       headPose.orientation * lensOffset.rotation
   );
   ```

2. **LensOffset Logging**: Added debug logging confirmed physical camera offsets match expected IPD (~6-7cm separation between left/right cameras)

3. **Camera Switch Test**: Switching from Left to Right camera **flipped the offset direction**, confirming LensOffset is involved in the issue

### Hypothesis 1: Missing Camera Rotation in Pose Transform

**Theory**: Position offset caused by using `Quaternion.identity` instead of actual camera rotation.

**Implementation**: Set `useCameraRotation = true` in MetaCameraProvider

**Result**: ❌ User reported this made tracking worse, not better

### Hypothesis 2: Coordinate Conversion Order - Convert First, Then Invert

**Theory**: The coordinate system transformation (Unity → CV) must happen BEFORE pose inversion (camera-to-world → world-to-camera).

**Implementation** (`external_tracker.cpp`):
```cpp
// Step 1: Convert camera-to-world from Unity to CV convention
float cv_px = positionIn[0];
float cv_py = -positionIn[1];  // Flip Y
float cv_pz = -positionIn[2];  // Flip Z

// Rotation: apply 180° rotation around X-axis
float cv_qx = qw;
float cv_qy = -qz;
float cv_qz = -qy;
float cv_qw = qx;

// Step 2: Invert to get world-to-camera
// Quaternion inverse
float inv_qx = -cv_qx;
float inv_qy = -cv_qy;
float inv_qz = -cv_qz;
float inv_qw = cv_qw;

// Translation inverse: -R^T * P
positionOut[0] = -(r00 * cv_px + r01 * cv_py + r02 * cv_pz);
positionOut[1] = -(r10 * cv_px + r11 * cv_py + r12 * cv_pz);
positionOut[2] = -(r20 * cv_px + r21 * cv_py + r22 * cv_pz);
```

**Result**: ❌ No improvement, offset still present

### Hypothesis 3: Coordinate Conversion Order - Invert First, Then Convert

**Theory**: The pose should be inverted in Unity convention first, THEN converted to CV convention.

**Implementation** (`external_tracker.cpp`):
```cpp
// Step 1: Invert Unity camera-to-world to get world-to-camera
float inv_qx = -qx;
float inv_qy = -qy;
float inv_qz = -qz;
float inv_qw = qw;

// Compute inverted translation: -R^T * t
float inv_px = -(r00 * px + r01 * py + r02 * pz);
float inv_py = -(r10 * px + r11 * py + r12 * pz);
float inv_pz = -(r20 * px + r21 * py + r22 * pz);

// Step 2: Convert world-to-camera from Unity to CV convention
positionOut[0] = inv_px;
positionOut[1] = -inv_py;   // Flip Y
positionOut[2] = -inv_pz;   // Flip Z

// Apply rotation conversion
float cv_quat[4];
cv_quat[0] = inv_qw;
cv_quat[1] = -inv_qz;
cv_quat[2] = -inv_qy;
cv_quat[3] = inv_qx;
```

**Result**: ❌ No improvement, offset still present

### Hypothesis 4: Simple Coordinate Flip Without Inversion

**Theory**: Maybe we're over-complicating it. Vuforia might expect camera-to-world pose (not world-to-camera), just in CV convention.

**Implementation** (`external_tracker.cpp`):
```cpp
// Simple coordinate transformation without inversion
positionOut[0] = positionIn[0];   // X unchanged
positionOut[1] = -positionIn[1];  // Y flipped
positionOut[2] = -positionIn[2];  // Z flipped

// Transform rotation
float transformedQuat[4];
transformedQuat[0] = qw;
transformedQuat[1] = -qz;
transformedQuat[2] = -qy;
transformedQuat[3] = qx;
```

**Result**: ❌ No improvement, offset still present (this was reverted by user via linter/manual edit)

### Hypothesis 5: Feed Head Pose Instead of Camera Pose

**Theory**: Vuforia External Camera API might expect the **device/head pose** (center reference point), not the actual camera pose with LensOffset applied. The camera offset might be handled internally by Vuforia or specified differently.

**Implementation** (`MetaCameraProvider.cs`):
```csharp
// Get head pose WITHOUT LensOffset
var headPose = OVRPlugin.GetNodePoseStateAtTime(
    OVRPlugin.GetTimeInSeconds(),
    OVRPlugin.Node.Head
).Pose.ToOVRPose();

QuestVuforiaBridge.FeedDevicePose(headPose.position, headPose.orientation, timestamp);
```

**Result**: ❌ Compilation errors, then after fix: No improvement, offset still present

## Key Findings

### What Works
1. ✅ **Rotation is always correct** - coordinate transformations for rotation are working properly
2. ✅ **Camera frame delivery** - Vuforia receives and processes images successfully
3. ✅ **Target detection** - Image targets are detected reliably
4. ✅ **Basic tracking** - Targets are tracked, just with positional offset

### What Doesn't Work
1. ❌ **Position accuracy** - Consistent ~4-5cm offset
2. ❌ **Offset direction flips with camera** - Strong evidence LensOffset is involved but being mishandled
3. ❌ **All transformation approaches tried** - Neither conversion order resolved the issue

### Critical Observations

1. **Rotation works, position doesn't**: This suggests the issue is specifically with translational component handling, not the overall transformation approach

2. **Offset magnitude**: ~4-5cm offset vs 3.2cm LensOffset X-component suggests possible:
   - LensOffset being applied incorrectly
   - Additional offset from Y (1.8cm) and Z (6.3cm) components
   - Combined 3D offset: sqrt(3.2² + 1.8² + 6.3²) = 7.3cm

3. **Flip behavior**: Offset direction changes when switching cameras is definitive proof that LensOffset is the culprit

4. **Vuforia components placement**: VuforiaBehaviour on CenterEyeAnchor while camera is Left/Right might be causing reference frame mismatch

## Unanswered Questions

### About Vuforia Driver Framework

1. **What does "camera coordinate system" mean?**
   - `PoseCoordSystem::CAMERA` is set in the pose, but documentation is unclear
   - Does Vuforia expect camera-to-world or world-to-camera transform?
   - Is the pose relative to device center or camera sensor?

2. **How should camera offset be specified?**
   - Is LensOffset supposed to be in the pose itself?
   - Should it be part of camera intrinsics?
   - Is there a separate camera extrinsics parameter we're missing?

3. **World space reference**
   - What is Vuforia's world origin?
   - Should world poses be relative to device center or camera?
   - How does Vuforia transform target poses back to Unity world space?

### About Unity Integration

1. **VuforiaBehaviour placement**
   - Should VuforiaBehaviour be on CenterEyeAnchor or at actual camera position?
   - Does `WorldCenterMode.SPECIFIC_TARGET` affect pose calculations?
   - How does standard Vuforia Unity SDK interact with custom External Camera driver?

2. **Pose synchronization**
   - Are we using the correct timestamp for pose-frame synchronization?
   - Should poses be from camera frame capture time or current time?

## Recommended Next Steps

### 1. Review Vuforia Driver Framework Documentation
- Re-read External Camera API documentation carefully
- Check if there are example implementations for devices with offset cameras
- Contact Vuforia support with specific questions about LensOffset handling

### 2. Test with Different Vuforia Components Setup
- Try moving VuforiaBehaviour to a GameObject at Left camera's position
- Test with `WorldCenterMode.DEVICE` or `CAMERA` instead of `SPECIFIC_TARGET`
- Create a custom GameObject hierarchy that matches camera offset

### 3. Investigate Camera Intrinsics/Extrinsics
- Check if camera offset should be in intrinsics structure
- Look for separate extrinsics parameters in Vuforia Driver API
- Verify if there's a camera transformation matrix we should be providing

### 4. Debug Vuforia's Internal State
- Add logging to see what poses Vuforia is actually receiving
- Check if Vuforia has debug output showing its world-to-camera transform
- Verify target poses Vuforia computes before Unity applies them

### 5. Test Simplified Scenarios
- Try with a stationary head (no head movement) to isolate translation vs rotation issues
- Test with target at different distances to see if offset scales
- Check if offset exists in all 3 axes or primarily in one

### 6. Compare with Working Implementation
- Find other projects using Vuforia External Camera API with offset cameras
- Check if Meta has example code for Quest + Vuforia integration
- Look for open-source AR projects with similar architecture

## Code References

### Key Files Modified During Investigation

1. **QuforiaPlugin/src/external_tracker.cpp**
   - `transformOpenXRToCV()` function (lines 206-287)
   - Multiple different transformation approaches tested

2. **Assets/QuestVuforia/MetaCameraProvider.cs**
   - Camera pose acquisition (lines 202-215)
   - Tested both camera pose and head pose
   - Added LensOffset logging (lines 114-117)

3. **Assets/Plugins/Android/libs/arm64-v8a/libquforia.so**
   - Native plugin rebuilt 3+ times with different transformations

### Relevant External Code

1. **PassthroughCameraAccess.cs** (Meta XR SDK)
   - Line 478-482: GetCameraPose() implementation
   - Line 515: LensOffset definition

2. **VuforiaEngine/Driver/Driver.h**
   - Line 275-294: Pose structure definition
   - Line 146-149: PoseCoordSystem enum

## Technical Notes

### Coordinate Transformation Math

For a rigid body transform [R|t] (rotation + translation):
- **Inverse**: [R^T | -R^T * t]
- **Coordinate conversion**: Apply to both position and rotation
- **Order matters**: Converting then inverting ≠ Inverting then converting

### Unity-to-CV Coordinate Conversion

Position: (x, y, z) → (x, -y, -z)

Rotation (quaternion): Apply q_x(180°) * q_unity
- q_x(180°) = (1, 0, 0, 0)
- Result: (x, y, z, w) → (w, -z, -y, x)

### Camera-to-World vs World-to-Camera

**Camera-to-World** (camera's pose in world space):
- Position: Where camera is in world
- Rotation: Which way camera faces

**World-to-Camera** (transform world points to camera space):
- Position: -R^T * P_cam
- Rotation: R^T

## Current Status

**Status**: ❌ **UNRESOLVED**

All attempted fixes have been reverted. The system is back to the original implementation with:
- Position offset still present (~4-5cm)
- Rotation working correctly
- Left/Right camera flip behavior confirmed

The root cause remains unknown. The issue appears to be fundamental to how LensOffset (camera position relative to head center) is being handled in the pose transformations or how Vuforia interprets the provided poses.

## Contact & Support

For future investigation:
- Vuforia Developer Forums: https://developer.vuforia.com/forum
- Vuforia Support: https://developer.vuforia.com/support
- Meta Quest Developer Support: https://developers.meta.com/support

---

*Document created: 2025-11-13*
*Last updated: 2025-11-13*
*Status: Investigation ongoing*
