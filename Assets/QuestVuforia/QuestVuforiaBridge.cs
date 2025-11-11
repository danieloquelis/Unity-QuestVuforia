using System;
using System.Runtime.InteropServices;
using UnityEngine;

/// <summary>
/// Bridge between Unity C# and native Vuforia Driver Framework.
/// Directly calls C++ functions via P/Invoke (no Kotlin layer needed).
/// All Vuforia initialization and target tracking is handled by Vuforia Unity SDK.
/// </summary>
public static class QuestVuforiaBridge
{
    private const string LibraryName = "quforia";

    // =============================================================================
    // Native Function Declarations (P/Invoke)
    // =============================================================================

    [DllImport(LibraryName)]
    private static extern bool nativeSetCameraIntrinsics(float[] intrinsics, int length);

    [DllImport(LibraryName)]
    private static extern bool nativeFeedDevicePose(float[] position, float[] rotation, long timestamp);

    [DllImport(LibraryName)]
    private static extern bool nativeFeedCameraFrame(byte[] imageData, int width, int height, float[] intrinsics, int intrinsicsLength, long timestamp);

    [DllImport(LibraryName)]
    private static extern bool nativeIsDriverInitialized();

    // =============================================================================
    // Public API Methods
    // =============================================================================

    /// <summary>
    /// Set camera intrinsics (call once at initialization).
    /// Intrinsics array format: [width, height, fx, fy, cx, cy, d0-d7]
    /// </summary>
    /// <param name="intrinsics">Camera intrinsics array (at least 6 floats)</param>
    /// <returns>True if successful</returns>
    public static bool SetCameraIntrinsics(float[] intrinsics)
    {
        if (intrinsics == null || intrinsics.Length < 6)
        {
            Debug.LogError("[QUFORIA] Intrinsics array must have at least 6 elements");
            return false;
        }

        try
        {
            bool result = nativeSetCameraIntrinsics(intrinsics, intrinsics.Length);
            if (result)
            {
                Debug.Log($"[QUFORIA] Camera intrinsics set: {intrinsics[0]}x{intrinsics[1]}, " +
                         $"fx={intrinsics[2]}, fy={intrinsics[3]}, cx={intrinsics[4]}, cy={intrinsics[5]}");
            }
            return result;
        }
        catch (Exception e)
        {
            Debug.LogError($"[QUFORIA] SetCameraIntrinsics failed: {e.Message}");
            return false;
        }
    }

    /// <summary>
    /// Feed device pose to the Vuforia Driver.
    /// CRITICAL: Must be called BEFORE FeedCameraFrame with the same timestamp.
    /// </summary>
    /// <param name="position">Camera position in world space</param>
    /// <param name="rotation">Camera rotation (quaternion)</param>
    /// <param name="timestamp">Frame timestamp in nanoseconds</param>
    /// <returns>True if successful</returns>
    public static bool FeedDevicePose(Vector3 position, Quaternion rotation, long timestamp)
    {
        try
        {
            float[] positionArray = new float[] { position.x, position.y, position.z };
            float[] rotationArray = new float[] { rotation.x, rotation.y, rotation.z, rotation.w };

            return nativeFeedDevicePose(positionArray, rotationArray, timestamp);
        }
        catch (Exception e)
        {
            Debug.LogError($"[QUFORIA] FeedDevicePose failed: {e.Message}");
            return false;
        }
    }

    /// <summary>
    /// Feed camera frame to the Vuforia Driver.
    /// Must be called AFTER FeedDevicePose with the same timestamp.
    /// </summary>
    /// <param name="imageData">RGB image data (width * height * 3 bytes)</param>
    /// <param name="width">Image width in pixels</param>
    /// <param name="height">Image height in pixels</param>
    /// <param name="intrinsics">Optional intrinsics (uses cached if null)</param>
    /// <param name="timestamp">Frame timestamp in nanoseconds</param>
    /// <returns>True if successful</returns>
    public static bool FeedCameraFrame(byte[] imageData, int width, int height,
                                       float[] intrinsics, long timestamp)
    {
        if (imageData == null)
        {
            Debug.LogError("[QUFORIA] Image data is null");
            return false;
        }

        int expectedSize = width * height * 3;  // RGB888
        if (imageData.Length != expectedSize)
        {
            Debug.LogError($"[QUFORIA] Image data size mismatch: " +
                          $"{imageData.Length}, expected {expectedSize}");
            return false;
        }

        try
        {
            int intrinsicsLength = (intrinsics != null) ? intrinsics.Length : 0;
            return nativeFeedCameraFrame(imageData, width, height, intrinsics, intrinsicsLength, timestamp);
        }
        catch (Exception e)
        {
            Debug.LogError($"[QUFORIA] FeedCameraFrame failed: {e.Message}");
            return false;
        }
    }

    /// <summary>
    /// Check if the native driver is initialized and ready.
    /// </summary>
    /// <returns>True if driver is initialized</returns>
    public static bool IsDriverInitialized()
    {
        try
        {
            return nativeIsDriverInitialized();
        }
        catch (Exception e)
        {
            Debug.LogError($"[QUFORIA] IsDriverInitialized failed: {e.Message}");
            return false;
        }
    }
}
