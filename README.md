# Quforia - Quest + Vuforia Integration

![Status](https://img.shields.io/badge/Status-Work%20In%20Progress%20|%20Experimental-orange)

An experimental Unity project integrating Vuforia Engine AR tracking with Meta Quest 3's passthrough cameras through a custom native driver implementation.

![Demo](/Media/image-target-demo.gif)

## Overview

Quforia bridges Vuforia Engine 11.4.4 with Meta Quest passthrough camera system by implementing a custom C++ plugin using the Vuforia Driver Framework. This enables AR image tracking directly on Quest's passthrough view without requiring external devices.

**Built With:**

- Unity 6000.0.61f1
- Vuforia Engine 11.4.4 (Driver Framework/External Camera API)
- Meta XR SDK 81.0.0

## Current State

### Working

- **Image Target Tracking**: Basic image recognition and tracking functional
- **Camera Integration**: Quest passthrough camera frames successfully fed to Vuforia
- **Pose Synchronization**: Device pose data correctly passed through driver framework
- **Real-time Processing**: Stable frame delivery and tracking updates

### Known Issues

- **Position Offset (~4-5cm)**: Tracked objects appear offset from their actual position, despite correct rotation alignment
  - Offset direction flips when switching between left/right cameras
  - Root cause under investigation - likely related to camera lens offset or coordinate system handling

### In Development

- **Model Target Support**: Integration planned but not yet implemented
- **Position Offset Fix**: Active investigation into coordinate system transformations

## Setup

- Clone this project.
- Make sure you have a [Vuforia License Key](https://developer.vuforia.com/home) (works with the free tier).
- Go to Assets/StreamingAssets and create a copy of `VuforiaLicenseKey.text.template` and more the suffix `VuforiaLicenseKey.txt`.
- Paste your license key in this new file. Now you should have a file named `VuforiaLicenseKey.txt` with the key pasted.

## Running Sample Scenes

**Image Target Sample**

- Create an Image Target Database inside Vuforia Dashboard.
- Export it into unity.
- Go to `Assets/Samples/ImageTarget` and open `ImageTargetScene.unity`
- Find the `GameObject` called `ImageTarget`.
- Modify the `Database` param within `ImageTargetBehaviour` component and look for your database.
- Locate your Image Target in the dropdown below.
- Run sample in your headset.

**Model Target Sample**

_This is currently work in progress_

## Technical Approach

The project uses a two-layer architecture:

1. **Unity C# Layer**: Captures Quest camera frames via `PassthroughCameraAccess`, extracts RGB data and pose information, and feeds it to the native plugin through P/Invoke bridges.

2. **Native C++ Plugin** (`libquforia.so`): Implements the Vuforia Driver Framework interface, managing camera lifecycle, frame queuing, and coordinate system transformations between Unity/OpenXR and Vuforia CV conventions.

### Key Challenges

- **Coordinate System Complexity**: Converting between Unity's left-handed Y-up and Vuforia's right-handed Y-down coordinate systems while handling camera-to-world pose inversions
- **Camera Extrinsics**: Managing lens offset (camera position relative to head center) in the Vuforia Driver Framework
- **Sparse Documentation**: Limited official guidance on implementing Vuforia Driver Framework with offset cameras
- **Cross-Platform Build Chain**: Integrating Unity, Android NDK, Meta SDK, and Vuforia Engine native libraries

## Building

### Native Plugin

```bash
cd QuforiaPlugin
./build.sh
```

### Unity Project

1. Open in Unity 6000.0.61f1
2. File → Build Settings → Android
3. Build and Run to Quest 3

## Requirements

- Meta SDK 81
- Unity 6000.0.61f1
- Vuforia Engine license (free development license available)

## Project Structure

```
Assets/
├── QuestVuforia/          # C# integration scripts
│   ├── QuestVuforiaDriverInit.cs
│   ├── MetaCameraProvider.cs
│   └── QuestVuforiaBridge.cs
├── Plugins/Android/       # Native plugin (.so)
└── Samples/               # Example scenes

QuforiaPlugin/             # C++ native plugin source
├── src/
│   ├── vuforia_driver.cpp
│   ├── external_camera.cpp
│   └── external_tracker.cpp
└── build.sh
```

## Future Work

- Resolve position offset issue
- Implement Model Target tracking
- Add dual-camera (stereo) support for improved robustness
- Optimize frame conversion pipeline
- Performance profiling and optimization

## Contributing

This is an experimental research project. Contributions, suggestions, and issue reports are welcome as we work through the integration challenges.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Vuforia Engine by PTC
- Meta Quest SDK
- Unity Technologies

---

**Note**: This project is experimental and under active development. Use at your own risk.
