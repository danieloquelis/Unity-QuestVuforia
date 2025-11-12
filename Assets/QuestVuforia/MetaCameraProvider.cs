using System;
using System.Collections;
using Meta.XR;
using Unity.Collections;
using UnityEngine;
using UnityEngine.Android;

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
    [SerializeField] private float statsInterval = 1.0f;  // Log stats every N seconds

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

        // Check Android camera permissions
        if (!Permission.HasUserAuthorizedPermission("horizonos.permission.HEADSET_CAMERA"))
        {
            LogWarning("HEADSET_CAMERA permission not granted. Requesting...");
            Permission.RequestUserPermission("horizonos.permission.HEADSET_CAMERA");
        }
        else
        {
            Log("HEADSET_CAMERA permission already granted");
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

        // Check if cameraAccess component exists
        if (cameraAccess == null)
        {
            LogError("PassthroughCameraAccess component is null! Check Inspector assignment.");
            yield break;
        }

        // Log detailed state
        Log($"PassthroughCameraAccess initial state:");
        Log($"  - enabled: {cameraAccess.enabled}");
        Log($"  - isPlaying: {cameraAccess.IsPlaying}");
        Log($"  - gameObject.activeInHierarchy: {cameraAccess.gameObject.activeInHierarchy}");
        Log($"  - gameObject.activeSelf: {cameraAccess.gameObject.activeSelf}");
        Log($"  - component name: {cameraAccess.GetType().Name}");

        try
        {
            var currentResolution = cameraAccess.CurrentResolution;
            Log($"  - CurrentResolution: {currentResolution.x}x{currentResolution.y}");
        }
        catch (Exception e)
        {
            Log($"  - CurrentResolution: ERROR - {e.Message}");
        }

        // Enable the PassthroughCameraAccess component
        if (!cameraAccess.enabled)
        {
            Log("Enabling PassthroughCameraAccess component...");
            cameraAccess.enabled = true;

            // Wait a frame for enable to take effect
            yield return null;

            Log($"After enable: enabled={cameraAccess.enabled}, isPlaying={cameraAccess.IsPlaying}");
        }
        else
        {
            Log("PassthroughCameraAccess was already enabled");
        }

        // Also ensure GameObject is active
        if (!cameraAccess.gameObject.activeInHierarchy)
        {
            LogWarning("PassthroughCameraAccess GameObject is not active! Activating...");
            cameraAccess.gameObject.SetActive(true);
            yield return null;
        }

        // Wait until camera is playing (with timeout)
        float timeout = 10.0f;  // 10 second timeout
        float elapsed = 0f;

        Log("Waiting for PassthroughCameraAccess.IsPlaying...");
        while (!cameraAccess.IsPlaying)
        {
            yield return null;
            elapsed += Time.deltaTime;

            if (elapsed >= timeout)
            {
                LogError($"Timeout waiting for PassthroughCameraAccess to start! IsPlaying={cameraAccess.IsPlaying}, enabled={cameraAccess.enabled}");
                LogError("Make sure PassthroughCameraAccess is properly added to your scene as a GameObject with the component attached.");
                yield break;
            }

            // Log every 2 seconds while waiting
            if ((int)elapsed % 2 == 0 && elapsed - Time.deltaTime < (int)elapsed)
            {
                Log($"Still waiting... (elapsed: {elapsed:F1}s, IsPlaying={cameraAccess.IsPlaying})");
            }
        }

        Log("PassthroughCameraAccess is now playing");

        // Get current resolution
        Vector2Int resolution = cameraAccess.CurrentResolution;
        width = resolution.x;
        height = resolution.y;

        Log($"Camera resolution: {width}x{height}");

        // Allocate RGB buffer for frame data
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
            if (showFrameStats && Time.time - lastStatsTime >= statsInterval)
            {
                currentFPS = framesProcessed / (Time.time - lastStatsTime);
                Log($"MetaCameraProvider: Processed {framesProcessed} frames at {currentFPS:F1} FPS (Total: {frameCount})");
                lastStatsTime = Time.time;
                framesProcessed = 0;
            }

            yield return null;  // Process every frame
        }
    }

    private void ProcessCurrentFrame()
    {
        // CORRECTED: Use GetColors() method as shown in Meta's official PassthroughCameraApiSamples
        // This is the proper API to get CPU-readable pixel data from PassthroughCameraAccess
        // Returns NativeArray<Color32> (byte-based RGBA format)
        NativeArray<Color32> pixels = cameraAccess.GetColors();

        if (!pixels.IsCreated || pixels.Length == 0)
        {
            LogWarning("GetColors() returned invalid or empty NativeArray");
            return;
        }

        // Verify we got the expected number of pixels
        int expectedPixels = width * height;
        if (pixels.Length != expectedPixels)
        {
            LogWarning($"GetColors() returned unexpected pixel count: {pixels.Length} (expected {expectedPixels})");
            return;
        }

        // Log every 120 frames for debugging
        if (frameCount % 120 == 0 && frameCount > 0)
        {
            // Sample center pixel to verify we're getting real data
            int centerIdx = (height / 2) * width + (width / 2);
            Color32 centerPixel = pixels[centerIdx];
            Log($"GetColors() returned {pixels.Length} pixels, center pixel: R={centerPixel.r}, G={centerPixel.g}, B={centerPixel.b}");
        }

        // Convert NativeArray<Color32> to byte[] RGB888 format
        // Color32 already uses bytes (0-255), just strip the alpha channel
        for (int i = 0; i < pixels.Length; i++)
        {
            int rgbIndex = i * 3;
            Color32 pixel = pixels[i];
            imageDataRGB[rgbIndex] = pixel.r;
            imageDataRGB[rgbIndex + 1] = pixel.g;
            imageDataRGB[rgbIndex + 2] = pixel.b;
            // Skip alpha channel (pixel.a)
        }

        // Flip Y-axis (Unity textures are bottom-up, Vuforia expects top-down)
        FlipImageVertically(imageDataRGB, width, height);

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

    /// <summary>
    /// Flips image vertically in place (Unity textures are bottom-up, Vuforia expects top-down).
    /// </summary>
    private void FlipImageVertically(byte[] imageData, int width, int height)
    {
        int bytesPerPixel = 3; // RGB888
        int stride = width * bytesPerPixel;
        byte[] rowBuffer = new byte[stride];

        for (int row = 0; row < height / 2; row++)
        {
            int topRowOffset = row * stride;
            int bottomRowOffset = (height - 1 - row) * stride;

            // Swap rows
            System.Array.Copy(imageData, topRowOffset, rowBuffer, 0, stride);
            System.Array.Copy(imageData, bottomRowOffset, imageData, topRowOffset, stride);
            System.Array.Copy(rowBuffer, 0, imageData, bottomRowOffset, stride);
        }
    }

    private void Log(string message)
    {
        if (enableDebugLogs)
        {
            Debug.Log($"[QUFORIA] {message}");
        }
    }

    private void LogWarning(string message)
    {
        Debug.LogWarning($"[QUFORIA] {message}");
    }

    private void LogError(string message)
    {
        Debug.LogError($"[QUFORIA] {message}");
    }
}
