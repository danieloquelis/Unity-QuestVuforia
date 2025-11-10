# Quest Vuforia Driver Framework - Quick Start Guide

**Goal:** Get Vuforia tracking working on Meta Quest 3/3S using your camera rig.

---

## ✅ Prerequisites (Already Done)

- Unity 6 with Meta XR SDK 81
- OpenXR configured (not Oculus plugin)
- Vuforia Unity SDK imported
- Native C++ driver built (libquforia.so)
- Android build configuration set

---

## 🎯 Step 1: Scene Setup with Meta Camera Rig

### Option A: If you already have Meta Camera Rig in your scene
1. **Open** `Assets/Scenes/SampleScene.unity`
2. **Locate** your existing Camera Rig (should be named `OVRCameraRig`, `MRUKCameraRig`, or similar)
3. **Select** the **Center Eye Anchor** (the camera that renders the view)
   - Usually located at: `YourCameraRig > TrackingSpace > CenterEyeAnchor`

### Option B: If you need to add Meta Camera Rig
1. **Window → Building Blocks**
2. **Add** the "Camera Rig" building block to your scene
3. This creates an `OVRCameraRig` with proper Quest setup
4. **Delete** the default Main Camera if it exists

---

## 🎯 Step 2: Add Vuforia + Driver Components

### On the Center Eye Anchor Camera:

**Important:** Add components in this exact order!

1. **Add VuforiaBehaviour:**
   - Select `CenterEyeAnchor` (the actual Camera component)
   - `Add Component` → Search "Vuforia Behaviour"
   - Click `Add VuforiaBehaviour to Scene`
   - **Leave all default settings** (don't change World Center Mode)

2. **Add QuestVuforiaDriverInit:**
   - `Add Component` → Search "Quest Vuforia Driver Init"
   - Configure:
     - Driver Library Name: `libquforia.so` ✅
     - Enable Debug Logs: ✅ (checked)

3. **Add MetaCameraProvider:**
   - `Add Component` → Search "Meta Camera Provider"
   - Configure:
     - Target Width: `1280`
     - Target Height: `960`
     - Target FPS: `30`
     - Camera Eye: `Left` (recommended)
     - Auto Start: ✅ (checked)
     - Enable Debug Logs: ✅ (checked)

**Your hierarchy should look like:**
```
Scene
├── OVRCameraRig (or MRUKCameraRig)
│   └── TrackingSpace
│       └── CenterEyeAnchor
│           ├── Camera
│           ├── VuforiaBehaviour          ← Added
│           ├── QuestVuforiaDriverInit    ← Added
│           └── MetaCameraProvider        ← Added
└── Directional Light
```

---

## 🎯 Step 3: Configure Vuforia License

1. **Assets → Resources → VuforiaConfiguration**
2. In Inspector, find **"Vuforia"** section
3. **Paste your Vuforia license key** in `Vuforia License Key` field
   - Get one at https://developer.vuforia.com/
4. **Verify settings:**
   - `Delayed Initialization`: ✅ (checked) **CRITICAL**
   - `Device Tracker` → `Auto Start Tracker`: ✅ (checked)

---

## 🎯 Step 4: Add a Test Image Target (Optional but Recommended)

To verify tracking is working:

### Create Image Target Database:
1. Go to https://developer.vuforia.com/target-manager
2. Create new database (Device type)
3. Upload an image (high contrast, 4-5 star rating recommended)
4. Download for Unity Editor
5. Import `.unitypackage` into your project

### Add Image Target to Scene:
1. Right-click in Hierarchy → `Vuforia Engine → Image Target`
2. Configure `ImageTargetBehaviour`:
   - Database: Select your imported database
   - Image Target: Select your uploaded image
3. **Add visual feedback** (child GameObject):
   - Right-click on ImageTarget → `3D Object → Cube`
   - Scale it: `(0.1, 0.1, 0.1)`
   - Change material color (so you can see it)

**Hierarchy:**
```
Scene
├── OVRCameraRig
│   └── ...
├── ImageTarget
│   ├── ImageTargetBehaviour
│   └── Cube (visual feedback)
└── Directional Light
```

---

## 🎯 Step 5: Build Settings

1. **File → Build Settings**
2. **Platform: Android** (switch if needed)
3. **Add Open Scenes** (if not already added)
4. **Player Settings:**
   - Minimum API Level: **Android 10.0 (API 29)** or higher
   - Target API Level: **Android 14.0 (API 34)** or higher
   - Scripting Backend: **IL2CPP**
   - Target Architectures: **ARM64** ✅

5. **XR Plug-in Management (Android tab):**
   - OpenXR: ✅ (checked)
   - **NOT** Oculus

6. **OpenXR Feature Groups (Android → OpenXR):**
   - Meta Quest Support: ✅
   - Meta Quest Feature Group: ✅

7. **Android Manifest Permissions:**
   - Should already be configured in `Assets/Plugins/Android/AndroidManifest.xml`
   - Verify it has: `horizonos.permission.HEADSET_CAMERA`

---

## 🎯 Step 6: Build and Deploy

### First Build (Test Compilation):

1. **Connect Quest 3/3S via USB**
2. Enable Developer Mode on Quest
3. **Build and Run** (or just **Build** to verify)
4. Unity will compile native code (`libquforia.so`) automatically
5. **Watch for errors** in Console

### If build succeeds:
✅ APK generated successfully
✅ `libquforia.so` included in APK

---

## 🎯 Step 7: Test on Device

### Deploy to Quest:
```bash
# If you built without "Build and Run"
adb install /Users/danieloquelis/Developer/Unity-QuestVuforia/Quforia.apk

# Or just use Unity's "Build and Run"
```

### Monitor Logs:
Open a terminal and run:
```bash
adb logcat -s Unity QuestVuforiaDriver QuestExternalCamera QuestExternalTracker QuestVuforiaDriverInit MetaCameraProvider QuestVuforiaBridge
```

### Expected Log Sequence:

**1. Initialization (should see within 3 seconds of app start):**
```
QuestVuforiaDriverInit: Awake
QuestVuforiaDriverInit: Initializing Vuforia with driver: libquforia.so
QuestVuforiaDriver: vuforiaDriver_init called
QuestVuforiaDriver: QuestVuforiaDriver created successfully
QuestVuforiaDriver: createExternalCamera()
QuestExternalCamera: QuestExternalCamera constructor
QuestExternalCamera: open()
QuestVuforiaDriver: createExternalPositionalDeviceTracker()
QuestExternalTracker: QuestExternalTracker constructor
QuestVuforiaDriverInit: Vuforia initialized successfully with driver!
```

**2. Camera Start (should see after ~1 second):**
```
MetaCameraProvider: Initializing camera...
MetaCameraProvider: Found 2 cameras
MetaCameraProvider: Selected camera: Left Eye (1280x960)
MetaCameraProvider: Camera initialized: 1280x960 @ 30fps
QuestExternalCamera: start() with mode: 1280x960@30fps
QuestExternalCamera: Frame delivery thread started
```

**3. Frame/Pose Delivery (should repeat at ~30fps):**
```
QuestVuforiaDriver: Pose fed: pos(0.123,-0.456,0.789), timestamp=...
QuestVuforiaDriver: Frame fed: 1280x960, timestamp=...
QuestExternalCamera: Delivered 30 frames
QuestExternalTracker: Delivered 30 poses
```

---

## ✅ Verification Checklist

### Basic Functionality (Must Pass):
- [ ] App launches without crashing
- [ ] Logs show "Vuforia initialized successfully with driver!"
- [ ] Logs show "Camera initialized: 1280x960 @ 30fps"
- [ ] Logs show "Frame fed" messages at ~30fps
- [ ] Logs show "Pose fed" messages at ~30fps
- [ ] No errors in logcat

### Image Target Tracking (If you added one):
- [ ] Print your Image Target on paper
- [ ] Put on Quest and start app
- [ ] Point camera at printed target
- [ ] **Cube appears on target** ✅
- [ ] Cube stays aligned when you move your head
- [ ] Logs show target tracking events

---

## ❌ Common Issues

### Issue: "Driver not initialized" in logs
**Solution:**
- Check Vuforia license key is set
- Verify `DelayedInitialization = true` in VuforiaConfiguration
- Check `libquforia.so` is in APK: `unzip -l Quforia.apk | grep libquforia`

### Issue: "Camera not initializing" / Black screen
**Solution:**
- Check permissions in AndroidManifest.xml
- Verify Building Blocks camera rig is properly configured
- Check MetaCameraProvider logs for errors
- Try: `adb logcat | grep WebCamTexture`

### Issue: "No tracking" / Target not recognized
**Solution:**
- Verify Image Target database is imported
- Check target has high rating (4-5 stars)
- Ensure good lighting conditions
- Print target at actual size (check scale)
- Look at logs: `adb logcat | grep Vuforia`

### Issue: "Build fails" / Native compilation errors
**Solution:**
- Check CMakeLists.txt is correct
- Verify Vuforia headers are in `src/main/cpp/include/`
- Clean build: `Assets → Refresh` in Unity
- Check NDK version is compatible (27.x should work)

---

## 🔬 Advanced: Getting Real Camera Intrinsics

Currently, MetaCameraProvider **estimates** intrinsics. To use **real Quest camera intrinsics**:

1. **Wait for MRUK integration** (code is ready but commented out)
2. In `Assets/QuestVuforia/MetaCameraProvider.cs`, line ~135:
   - **Uncomment** the PassthroughCameraUtils calls
3. This will fetch real focal length, principal point from Quest's Camera API

**Code to uncomment (when ready):**
```csharp
// Line ~135 - SetupCameraIntrinsics()
var passthroughCameraEye = cameraEye == CameraEye.Left
    ? Meta.XR.MRUtilityKit.PassthroughCameraEye.Left
    : Meta.XR.MRUtilityKit.PassthroughCameraEye.Right;

var intrinsics = PassthroughCameraUtils.GetCameraIntrinsics(passthroughCameraEye);
```

---

## 📊 Performance Targets

**Expected Performance:**
- VR Rendering: **72 FPS** (Quest 3 native refresh rate)
- Camera Processing: **30 FPS** (640x480 or 1280x960)
- Frame Latency: **< 50ms** (pose to frame delivery)
- Memory Usage: **< 500MB** stable

**Monitor in Unity Profiler or via:**
```bash
adb shell dumpsys gfxinfo com.YourCompany.Quforia
```

---

## 🎉 Success Criteria

You'll know it's working when:
1. ✅ App launches smoothly
2. ✅ Logs show continuous frame/pose delivery
3. ✅ Image Target appears on printed image
4. ✅ Virtual content stays locked to target
5. ✅ 72fps VR rendering maintained

---

## 🆘 Need Help?

**Check logs first:**
```bash
# Full log
adb logcat > full_log.txt

# Filtered log
adb logcat -s Unity QuestVuforiaDriver QuestExternalCamera QuestExternalTracker > driver_log.txt
```

**Common log file locations:**
- Android: `adb logcat`
- Unity Editor: `~/Library/Logs/Unity/Editor.log` (macOS)

**Documentation:**
- Vuforia Driver Framework: https://developer.vuforia.com/library/platform-support/driver-framework
- Meta SDK 81: https://developers.meta.com/horizon/documentation/
- OpenXR: Unity XR Plug-in Management docs

---

**Ready to test? Follow Step 1 and let me know if you hit any issues!** 🚀
