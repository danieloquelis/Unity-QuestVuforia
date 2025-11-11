# Quforia Plugin - Simplified Architecture

## Overview

This is a **simplified Vuforia Driver Framework** implementation that removes the unnecessary Kotlin layer and calls C++ directly from Unity.

### Architecture

```
Unity C# (QuestVuforiaBridge.cs)
    ↓ [DllImport("quforia")]
C++ Driver (libquforia.so)
    ├── quforia_jni.cpp (P/Invoke bridge)
    ├── vuforia_driver.cpp (Driver Framework entry points)
    ├── external_camera.cpp (Camera implementation)
    └── external_tracker.cpp (Tracker implementation)
    ↓
Vuforia Engine (loaded by Vuforia Unity SDK)
```

**NO Kotlin, NO .androidlib, NO Gradle complexity!**

---

## Building

### Quick Build

```bash
cd /Users/danieloquelis/Developer/Unity-QuestVuforia/QuforiaPlugin
./build.sh
```

This will:
1. Build `libquforia.so` for arm64-v8a
2. Copy it to `Assets/Plugins/Android/libs/arm64-v8a/`

### After Building

1. **Refresh Unity**: Assets → Refresh
2. **Build APK**: File → Build and Run
3. **Deploy to Quest**

---

## Project Structure

```
QuforiaPlugin/
├── build.sh              # Build script
├── CMakeLists.txt        # CMake configuration
├── README.md             # This file
├── src/                  # C++ source files
│   ├── quforia_jni.cpp          # P/Invoke bridge (Unity → C++)
│   ├── vuforia_driver.cpp       # Driver Framework implementation
│   ├── vuforia_driver.h
│   ├── external_camera.cpp      # Camera implementation
│   ├── external_camera.h
│   ├── external_tracker.cpp     # Tracker implementation
│   └── external_tracker.h
└── include/              # Vuforia SDK headers
    └── VuforiaEngine/
        ├── Driver/
        └── ...
```

---

## What Was Removed

### ❌ Deleted (No Longer Needed)

- **Kotlin layer** (`QuestVuforiaManager.kt`)
- **.androidlib structure** (entire `QuforiaPlugin.androidlib` directory)
- **Gradle configuration** (`build.gradle`, etc.)
- **AndroidJavaObject calls** (replaced with direct P/Invoke)

### ✅ Kept (Essential)

- **C++ Driver Framework implementation** (vuforia_driver.cpp, external_camera.cpp, external_tracker.cpp)
- **P/Invoke bridge** (quforia_jni.cpp - simplified)
- **Unity C# bridge** (QuestVuforiaBridge.cs - using [DllImport])
- **Vuforia SDK headers** (include/VuforiaEngine/)

---

## How It Works

### 1. Vuforia Engine Loads the Driver

When you call `VuforiaApplication.Instance.Initialize("libquforia.so", IntPtr.Zero)`, Vuforia Engine loads your driver and calls:

```cpp
vuforiaDriver_init()          // Create driver instance
vuforiaDriver_getAPIVersion() // Check API compatibility
```

### 2. Unity Feeds Camera Data

From Unity C#:

```csharp
// Set intrinsics once
QuestVuforiaBridge.SetCameraIntrinsics(intrinsics);

// Every frame:
QuestVuforiaBridge.FeedDevicePose(position, rotation, timestamp);
QuestVuforiaBridge.FeedCameraFrame(imageData, width, height, null, timestamp);
```

These call C++ functions directly via P/Invoke (no Kotlin!):

```cpp
nativeSetCameraIntrinsics(float* intrinsics, int length)
nativeFeedDevicePose(float* position, float* rotation, long long timestamp)
nativeFeedCameraFrame(unsigned char* imageData, int width, int height, ...)
```

### 3. Driver Feeds Vuforia Engine

The driver implementation stores the data in queues and delivers it to Vuforia Engine when requested:

```cpp
// Vuforia Engine calls these:
ExternalCamera::onNewCameraFrame()  // Get next camera frame
ExternalTracker::onNewPose()        // Get next device pose
```

---

## Benefits of Simplified Architecture

1. **No Kotlin overhead** - Direct C# to C++ calls
2. **No Gradle complexity** - Build with simple CMake script
3. **Faster development** - Change C++ code, run `./build.sh`, rebuild Unity
4. **Easier debugging** - One less layer to troubleshoot
5. **Smaller APK** - No Kotlin runtime dependencies

---

## Troubleshooting

### Library Not Found

If Unity can't find `libquforia.so`:

1. Verify it's in `Assets/Plugins/Android/libs/arm64-v8a/`
2. **Assets → Refresh** in Unity
3. Check Unity Console for load errors

### Build Errors

If `./build.sh` fails:

1. Check NDK path in `build.sh` (line 13)
2. Verify CMake is installed: `which cmake`
3. Check compiler errors in build output

### Runtime Errors

If the app crashes on device:

1. Check logcat: `adb logcat -s QUFORIA Unity`
2. Verify Vuforia initialized: Look for "vuforiaDriver_init called"
3. Check frames are being fed: Look for "nativeFeedCameraFrame"

---

## Next Steps

1. **Add Image Target** to scene
2. **Build and deploy** to Quest
3. **Test tracking** with printed image

The hard part is done! 🚀
