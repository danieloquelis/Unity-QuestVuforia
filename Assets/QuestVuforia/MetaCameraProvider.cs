using System;
using System.Collections;
using Meta.XR;
using Unity.Collections;
using UnityEngine;
using UnityEngine.Android;

/// <summary>
/// Provides camera frames and device poses to Vuforia Driver Framework.
/// Uses Meta Quest's PassthroughCameraAccess for frame capture.
/// </summary>
[DefaultExecutionOrder(-50)]
public class MetaCameraProvider : MonoBehaviour
{
    [Header("Camera Access")]
    [SerializeField] private PassthroughCameraAccess cameraAccess;

    [Header("Settings")]
    [SerializeField] private bool autoStart = true;
    [SerializeField] private bool flipImageVertically = true;

    [Header("Debug")]
    [SerializeField] private bool enableDebugLogs = false;
    [SerializeField] private bool showFrameStats = false;
    [SerializeField] private float statsInterval = 1.0f;

    private byte[] imageDataRGB;
    private bool isRunning = false;
    private int frameCount = 0;
    private int width, height;
    private float[] cachedIntrinsics;

    // Frame stats
    private float lastStatsTime;
    private int framesProcessed;

    private void Start()
    {
        if (cameraAccess == null)
        {
            Debug.LogError("[Quforia] PassthroughCameraAccess not assigned!");
            return;
        }

        // Request camera permission
        if (!Permission.HasUserAuthorizedPermission("horizonos.permission.HEADSET_CAMERA"))
        {
            Permission.RequestUserPermission("horizonos.permission.HEADSET_CAMERA");
        }

        if (autoStart)
        {
            StartCoroutine(InitializeCamera());
        }
    }

    public IEnumerator InitializeCamera()
    {
        if (isRunning) yield break;

        Log("Initializing camera...");

        if (!cameraAccess.enabled)
        {
            cameraAccess.enabled = true;
            yield return null;
        }

        // Wait for camera to start (10s timeout)
        float elapsed = 0f;
        while (!cameraAccess.IsPlaying && elapsed < 10f)
        {
            yield return null;
            elapsed += Time.deltaTime;
        }

        if (!cameraAccess.IsPlaying)
        {
            Debug.LogError("[Quforia] Camera failed to start!");
            yield break;
        }

        // Get resolution and allocate buffer
        Vector2Int resolution = cameraAccess.CurrentResolution;
        width = resolution.x;
        height = resolution.y;
        imageDataRGB = new byte[width * height * 3];

        Log($"Camera initialized: {width}x{height}");

        // Setup intrinsics
        SetupCameraIntrinsics();

        isRunning = true;
        lastStatsTime = Time.time;
        StartCoroutine(ProcessFrames());
    }

    private void SetupCameraIntrinsics()
    {
        try
        {
            var intrinsics = cameraAccess.Intrinsics;

            cachedIntrinsics = new float[14];
            cachedIntrinsics[0] = width;
            cachedIntrinsics[1] = height;
            cachedIntrinsics[2] = intrinsics.FocalLength.x;
            cachedIntrinsics[3] = intrinsics.FocalLength.y;
            cachedIntrinsics[4] = intrinsics.PrincipalPoint.x;
            cachedIntrinsics[5] = intrinsics.PrincipalPoint.y;

            QuestVuforiaBridge.SetCameraIntrinsics(cachedIntrinsics);
            Log($"Intrinsics: fx={intrinsics.FocalLength.x:F1}, fy={intrinsics.FocalLength.y:F1}, " +
                $"cx={intrinsics.PrincipalPoint.x:F1}, cy={intrinsics.PrincipalPoint.y:F1}");
        }
        catch (Exception e)
        {
            Debug.LogError($"[Quforia] Failed to get intrinsics: {e.Message}");
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
                    Debug.LogError($"[Quforia] Frame processing error: {e.Message}");
                }
            }

            // Stats logging
            if (showFrameStats && Time.time - lastStatsTime >= statsInterval)
            {
                float fps = framesProcessed / (Time.time - lastStatsTime);
                Log($"Processing: {fps:F1} FPS | Total: {frameCount}");
                lastStatsTime = Time.time;
                framesProcessed = 0;
            }

            yield return null;
        }
    }

    private void ProcessCurrentFrame()
    {
        // Get camera frame pixels
        NativeArray<Color32> pixels = cameraAccess.GetColors();

        if (!pixels.IsCreated || pixels.Length != width * height)
        {
            return;
        }

        // Convert Color32 to RGB888
        for (int i = 0; i < pixels.Length; i++)
        {
            int rgbIndex = i * 3;
            Color32 pixel = pixels[i];
            imageDataRGB[rgbIndex] = pixel.r;
            imageDataRGB[rgbIndex + 1] = pixel.g;
            imageDataRGB[rgbIndex + 2] = pixel.b;
        }

        // Flip Y-axis if needed
        if (flipImageVertically)
        {
            FlipImageVertically(imageDataRGB, width, height);
        }

        // Get synchronized timestamp and pose
        DateTime currentTime = DateTime.Now;
        long timestampNs = currentTime.Ticks * 100;
        Pose cameraPose = cameraAccess.GetCameraPose();

        // Feed to Vuforia (position only, identity rotation)
        QuestVuforiaBridge.FeedDevicePose(cameraPose.position, Quaternion.identity, timestampNs);
        QuestVuforiaBridge.FeedCameraFrame(imageDataRGB, width, height, null, timestampNs);

        frameCount++;
    }

    private void FlipImageVertically(byte[] imageData, int width, int height)
    {
        int stride = width * 3;
        byte[] rowBuffer = new byte[stride];

        for (int row = 0; row < height / 2; row++)
        {
            int topRow = row * stride;
            int bottomRow = (height - 1 - row) * stride;

            Array.Copy(imageData, topRow, rowBuffer, 0, stride);
            Array.Copy(imageData, bottomRow, imageData, topRow, stride);
            Array.Copy(rowBuffer, 0, imageData, bottomRow, stride);
        }
    }

    public void StopCamera()
    {
        if (!isRunning) return;

        isRunning = false;
        if (cameraAccess != null && cameraAccess.enabled)
        {
            cameraAccess.enabled = false;
        }
        Log("Camera stopped");
    }

    private void OnDestroy() => StopCamera();

    private void OnApplicationPause(bool isPaused)
    {
        if (cameraAccess == null) return;

        if (isPaused && cameraAccess.enabled)
        {
            cameraAccess.enabled = false;
        }
        else if (!isPaused && isRunning && !cameraAccess.enabled)
        {
            cameraAccess.enabled = true;
        }
    }

    private void Log(string message)
    {
        if (enableDebugLogs)
        {
            Debug.Log($"[Quforia] {message}");
        }
    }
}
