using System;
using UnityEngine;

/// <summary>
/// Bridge between Unity C# and native Vuforia Driver Framework.
/// Simplified API for Driver Framework - only handles frame/pose feeding.
/// All Vuforia initialization and target tracking is handled by Vuforia Unity SDK.
/// </summary>
public static class QuestVuforiaBridge
{
    private static AndroidJavaObject plugin;
    private static readonly object lockObject = new object();

    private static AndroidJavaObject Plugin
    {
        get
        {
            if (plugin == null)
            {
                lock (lockObject)
                {
                    if (plugin == null)
                    {
                        InitializePlugin();
                    }
                }
            }
            return plugin;
        }
    }

    private static void InitializePlugin()
    {
#if UNITY_ANDROID && !UNITY_EDITOR
        try
        {
            using var unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
            var activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
            plugin = new AndroidJavaObject("com.quforia.QuestVuforiaManager", activity);
            Debug.Log("[QuestVuforiaBridge] Plugin initialized successfully");
        }
        catch (Exception e)
        {
            Debug.LogError($"[QuestVuforiaBridge] Failed to initialize plugin: {e.Message}");
            throw;
        }
#else
        Debug.LogWarning("[QuestVuforiaBridge] Running in editor - plugin not available");
#endif
    }

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
            Debug.LogError("[QuestVuforiaBridge] Intrinsics array must have at least 6 elements");
            return false;
        }

#if UNITY_ANDROID && !UNITY_EDITOR
        try
        {
            return Plugin.Call<bool>("setCameraIntrinsics", intrinsics);
        }
        catch (Exception e)
        {
            Debug.LogError($"[QuestVuforiaBridge] SetCameraIntrinsics failed: {e.Message}");
            return false;
        }
#else
        Debug.Log($"[QuestVuforiaBridge] [Editor] SetCameraIntrinsics: " +
                 $"{intrinsics[0]}x{intrinsics[1]}, fx={intrinsics[2]}, fy={intrinsics[3]}");
        return true;
#endif
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
#if UNITY_ANDROID && !UNITY_EDITOR
        try
        {
            float[] positionArray = new float[] { position.x, position.y, position.z };
            float[] rotationArray = new float[] { rotation.x, rotation.y, rotation.z, rotation.w };

            return Plugin.Call<bool>("feedDevicePose", positionArray, rotationArray, timestamp);
        }
        catch (Exception e)
        {
            Debug.LogError($"[QuestVuforiaBridge] FeedDevicePose failed: {e.Message}");
            return false;
        }
#else
        // Debug.Log($"[QuestVuforiaBridge] [Editor] FeedDevicePose: pos={position}, rot={rotation.eulerAngles}");
        return true;
#endif
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
            Debug.LogError("[QuestVuforiaBridge] Image data is null");
            return false;
        }

        int expectedSize = width * height * 3;  // RGB888
        if (imageData.Length != expectedSize)
        {
            Debug.LogError($"[QuestVuforiaBridge] Image data size mismatch: " +
                          $"{imageData.Length}, expected {expectedSize}");
            return false;
        }

#if UNITY_ANDROID && !UNITY_EDITOR
        try
        {
            return Plugin.Call<bool>("feedCameraFrame", imageData, width, height, intrinsics, timestamp);
        }
        catch (Exception e)
        {
            Debug.LogError($"[QuestVuforiaBridge] FeedCameraFrame failed: {e.Message}");
            return false;
        }
#else
        // Debug.Log($"[QuestVuforiaBridge] [Editor] FeedCameraFrame: {width}x{height}, {imageData.Length} bytes");
        return true;
#endif
    }

    /// <summary>
    /// Check if the native driver is initialized and ready.
    /// </summary>
    /// <returns>True if driver is initialized</returns>
    public static bool IsDriverInitialized()
    {
#if UNITY_ANDROID && !UNITY_EDITOR
        try
        {
            return Plugin.Call<bool>("isDriverInitialized");
        }
        catch (Exception e)
        {
            Debug.LogError($"[QuestVuforiaBridge] IsDriverInitialized failed: {e.Message}");
            return false;
        }
#else
        return true;  // In editor, assume initialized
#endif
    }

    /// <summary>
    /// Dispose of the plugin (called automatically on application quit).
    /// </summary>
    public static void Dispose()
    {
        lock (lockObject)
        {
            if (plugin != null)
            {
                try
                {
                    plugin.Dispose();
                    plugin = null;
                    Debug.Log("[QuestVuforiaBridge] Plugin disposed");
                }
                catch (Exception e)
                {
                    Debug.LogError($"[QuestVuforiaBridge] Dispose failed: {e.Message}");
                }
            }
        }
    }

    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
    private static void OnRuntimeMethodLoad()
    {
        // Reset static fields on domain reload (for play mode)
        plugin = null;
    }
}
