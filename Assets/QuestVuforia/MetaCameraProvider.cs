using System;
using System.Collections;
using Meta.XR;
using UnityEngine;

/// <summary>
/// Provides camera frames and device poses to the Vuforia Driver Framework.
/// Uses Meta Quest's PassthroughCameraAccess component from MRUK (Meta SDK 81+).
/// </summary>
[DefaultExecutionOrder(-50)]  // Execute after QuestVuforiaDriverInit
public class MetaCameraProvider : MonoBehaviour
{
    [Header("Camera Access")]
    [Tooltip("PassthroughCameraAccess component (add to scene and assign here)")]
    [SerializeField] private PassthroughCameraAccess cameraAccess;

    [Header("Settings")]
    [SerializeField] private bool autoStart = true;

    [Header("Debug")]
    [SerializeField] private bool enableDebugLogs = true;
    [SerializeField] private bool showFrameStats = true;

    private Texture2D frameTexture;
    private byte[] imageDataRGB;
    private bool isRunning = false;
    private int frameCount = 0;
    private int width, height;

    // Cached intrinsics
    private float[] cachedIntrinsics;
    private bool intrinsicsSet = false;

    // Frame stats
    private float lastStatsTime;
    private int framesProcessed;
    private float currentFPS;

    private void Start()
    {
        if (cameraAccess == null)
        {
            LogError("PassthroughCameraAccess component not assigned! Please add it to scene and assign in Inspector.");
            return;
        }

        if (autoStart)
        {
            StartCoroutine(InitializeCamera());
        }
    }

    public IEnumerator InitializeCamera()
    {
        if (isRunning)
        {
            Log("Camera already running");
            yield break;
        }

        Log("Initializing PassthroughCameraAccess...");

        // Enable the PassthroughCameraAccess component
        if (!cameraAccess.enabled)
        {
            cameraAccess.enabled = true;
            Log("PassthroughCameraAccess enabled");
        }

        // Wait until camera is playing
        // Texture may be black for first few frames but is already safe to use
        while (!cameraAccess.IsPlaying)
        {
            yield return null;
        }

        Log("PassthroughCameraAccess is now playing");

        // Get current resolution
        Vector2Int resolution = cameraAccess.CurrentResolution;
        width = resolution.x;
        height = resolution.y;

        Log($"Camera resolution: {width}x{height}");

        // Allocate buffers
        frameTexture = new Texture2D(width, height, TextureFormat.RGB24, false);
        imageDataRGB = new byte[width * height * 3];  // RGB888

        // Get and set camera intrinsics
        if (!SetupCameraIntrinsics())
        {
            LogWarning("Failed to get camera intrinsics");
        }

        isRunning = true;
        lastStatsTime = Time.time;
        framesProcessed = 0;

        Log("Camera provider ready, starting frame processing");

        // Start frame processing
        StartCoroutine(ProcessFrames());
    }

    private bool SetupCameraIntrinsics()
    {
        try
        {
            // Get camera intrinsics from PassthroughCameraAccess component
            PassthroughCameraAccess.CameraIntrinsics intrinsics = cameraAccess.Intrinsics;

            // Pack intrinsics into array format expected by driver
            // Format: [width, height, fx, fy, cx, cy, d0-d7]
            cachedIntrinsics = new float[14];
            cachedIntrinsics[0] = width;
            cachedIntrinsics[1] = height;
            cachedIntrinsics[2] = intrinsics.FocalLength.x;
            cachedIntrinsics[3] = intrinsics.FocalLength.y;
            cachedIntrinsics[4] = intrinsics.PrincipalPoint.x;
            cachedIntrinsics[5] = intrinsics.PrincipalPoint.y;
            // Distortion coefficients [6-13] remain 0 (not exposed by PassthroughCameraAccess)

            intrinsicsSet = true;
            Log($"Camera intrinsics: fx={intrinsics.FocalLength.x:F2}, fy={intrinsics.FocalLength.y:F2}, " +
                $"cx={intrinsics.PrincipalPoint.x:F2}, cy={intrinsics.PrincipalPoint.y:F2}");

            // Set intrinsics in the driver once
            QuestVuforiaBridge.SetCameraIntrinsics(cachedIntrinsics);
            return true;
        }
        catch (Exception e)
        {
            LogError($"Failed to get camera intrinsics: {e.Message}");
            return false;
        }
    }

    private IEnumerator ProcessFrames()
    {
        while (isRunning)
        {
            if (cameraAccess.IsPlaying)
            {
                try
                {
                    ProcessCurrentFrame();
                    framesProcessed++;
                }
                catch (Exception e)
                {
                    LogError($"Error processing frame: {e.Message}");
                }
            }

            // Update FPS stats
            if (showFrameStats && Time.time - lastStatsTime >= 1.0f)
            {
                currentFPS = framesProcessed / (Time.time - lastStatsTime);
                Log($"Camera FPS: {currentFPS:F1}, Total frames: {frameCount}");
                lastStatsTime = Time.time;
                framesProcessed = 0;
            }

            yield return null;  // Process every frame
        }
    }

    private void ProcessCurrentFrame()
    {
        // Get GPU texture from PassthroughCameraAccess
        Texture gpuTexture = cameraAccess.GetTexture();
        if (gpuTexture == null)
        {
            LogWarning("GPU texture is null");
            return;
        }

        // Copy GPU texture to CPU (blocking operation)
        // Note: This is a performance bottleneck - consider AsyncGPUReadback for production
        RenderTexture rt = gpuTexture as RenderTexture;
        if (rt != null)
        {
            RenderTexture.active = rt;
            frameTexture.ReadPixels(new Rect(0, 0, width, height), 0, 0);
            frameTexture.Apply();
            RenderTexture.active = null;
        }
        else
        {
            // Handle Texture2D case
            Graphics.CopyTexture(gpuTexture, frameTexture);
        }

        // Get raw RGB bytes
        byte[] rawData = frameTexture.GetRawTextureData();

        // Ensure correct size
        if (rawData.Length != imageDataRGB.Length)
        {
            LogWarning($"Texture data size mismatch: {rawData.Length} vs expected {imageDataRGB.Length}");
            return;
        }

        // Copy to our buffer
        Array.Copy(rawData, imageDataRGB, rawData.Length);

        // Get timestamp from PassthroughCameraAccess (convert DateTime to nanoseconds)
        DateTime timestamp = cameraAccess.Timestamp;
        long timestampNs = timestamp.Ticks * 100; // Convert .NET ticks (100ns) to nanoseconds

        // Get camera pose from PassthroughCameraAccess
        Pose cameraPose = cameraAccess.GetCameraPose();
        Vector3 cameraPosition = cameraPose.position;
        Quaternion cameraRotation = cameraPose.rotation;

        // Feed pose FIRST, then frame (CRITICAL for Driver Framework)
        QuestVuforiaBridge.FeedDevicePose(cameraPosition, cameraRotation, timestampNs);
        QuestVuforiaBridge.FeedCameraFrame(imageDataRGB, width, height, null, timestampNs);

        frameCount++;
    }

    public void StopCamera()
    {
        if (!isRunning) return;

        Log("Stopping camera...");
        isRunning = false;

        if (cameraAccess != null && cameraAccess.enabled)
        {
            cameraAccess.enabled = false;
        }

        if (frameTexture != null)
        {
            Destroy(frameTexture);
            frameTexture = null;
        }

        Log("Camera stopped");
    }

    private void OnDestroy()
    {
        StopCamera();
    }

    private void OnApplicationPause(bool pauseStatus)
    {
        if (cameraAccess == null) return;

        // Pause/resume camera
        if (pauseStatus)
        {
            if (cameraAccess.enabled)
            {
                cameraAccess.enabled = false;
                Log("Camera paused");
            }
        }
        else
        {
            if (isRunning && !cameraAccess.enabled)
            {
                cameraAccess.enabled = true;
                Log("Camera resumed");
            }
        }
    }

    private void Log(string message)
    {
        if (enableDebugLogs)
        {
            Debug.Log($"[MetaCameraProvider] {message}");
        }
    }

    private void LogWarning(string message)
    {
        Debug.LogWarning($"[MetaCameraProvider] {message}");
    }

    private void LogError(string message)
    {
        Debug.LogError($"[MetaCameraProvider] {message}");
    }

    // Public getters
    public bool IsRunning => isRunning;
    public int FrameCount => frameCount;
    public float CurrentFPS => currentFPS;
    public int Width => width;
    public int Height => height;
}
