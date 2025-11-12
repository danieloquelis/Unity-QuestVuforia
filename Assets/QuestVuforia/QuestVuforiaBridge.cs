using System;
using System.Runtime.InteropServices;
using UnityEngine;

/// <summary>
/// Bridge between Unity and native Vuforia Driver Framework via P/Invoke.
/// </summary>
public static class QuestVuforiaBridge
{
    private const string LibraryName = "quforia";

    [DllImport(LibraryName)]
    private static extern bool nativeSetCameraIntrinsics(float[] intrinsics, int length);

    [DllImport(LibraryName)]
    private static extern bool nativeFeedDevicePose(float[] position, float[] rotation, long timestamp);

    [DllImport(LibraryName)]
    private static extern bool nativeFeedCameraFrame(byte[] imageData, int width, int height, float[] intrinsics, int intrinsicsLength, long timestamp);

    [DllImport(LibraryName)]
    private static extern bool nativeIsDriverInitialized();

    /// <summary>
    /// Set camera intrinsics: [width, height, fx, fy, cx, cy, d0-d7]
    /// </summary>
    public static bool SetCameraIntrinsics(float[] intrinsics)
    {
        if (intrinsics == null || intrinsics.Length < 6)
        {
            Debug.LogError("[Quforia] Invalid intrinsics array");
            return false;
        }

        return nativeSetCameraIntrinsics(intrinsics, intrinsics.Length);
    }

    /// <summary>
    /// Feed device pose to driver. Call BEFORE FeedCameraFrame.
    /// </summary>
    public static bool FeedDevicePose(Vector3 position, Quaternion rotation, long timestamp)
    {
        float[] pos = new float[] { position.x, position.y, position.z };
        float[] rot = new float[] { rotation.x, rotation.y, rotation.z, rotation.w };
        return nativeFeedDevicePose(pos, rot, timestamp);
    }

    /// <summary>
    /// Feed camera frame to driver. Call AFTER FeedDevicePose.
    /// </summary>
    public static bool FeedCameraFrame(byte[] imageData, int width, int height, float[] intrinsics, long timestamp)
    {
        if (imageData == null || imageData.Length != width * height * 3)
        {
            Debug.LogError("[Quforia] Invalid image data");
            return false;
        }

        int intrinsicsLength = intrinsics?.Length ?? 0;
        return nativeFeedCameraFrame(imageData, width, height, intrinsics, intrinsicsLength, timestamp);
    }

    /// <summary>
    /// Check if native driver is initialized.
    /// </summary>
    public static bool IsDriverInitialized()
    {
        return nativeIsDriverInitialized();
    }
}
