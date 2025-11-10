using System;
using System.Collections;
using UnityEngine;

public class QuestVuforiaCamera : MonoBehaviour
{
    [SerializeField] private string vuforiaLicenseKey;
    [SerializeField] private bool autoStart = true;
    [SerializeField] private int targetFPS = 30;

    private WebCamTexture webCamTexture;
    private Color32[] pixelBuffer;
    private byte[] imageDataRGB;
    private float[] intrinsics;
    private bool isRunning = false;
    private int frameCount = 0;

    private void Start()
    {
        if (autoStart)
        {
            StartCamera();
        }
    }

    public void StartCamera()
    {
        if (isRunning)
        {
            Debug.LogWarning("Camera already running");
            return;
        }

        if (!QuestVuforiaBridge.Initialize(vuforiaLicenseKey))
        {
            Debug.LogError("Failed to initialize Vuforia");
            return;
        }

        StartCoroutine(InitializeWebCam());
    }

    public void StopCamera()
    {
        if (!isRunning) return;

        isRunning = false;

        if (webCamTexture != null)
        {
            webCamTexture.Stop();
            Destroy(webCamTexture);
            webCamTexture = null;
        }

        QuestVuforiaBridge.Shutdown();
        Debug.Log("Camera stopped");
    }

    private IEnumerator InitializeWebCam()
    {
        if (!Application.HasUserAuthorization(UserAuthorization.WebCam))
        {
            yield return Application.RequestUserAuthorization(UserAuthorization.WebCam);
        }

        if (!Application.HasUserAuthorization(UserAuthorization.WebCam))
        {
            Debug.LogError("Camera permission denied");
            yield break;
        }

        WebCamDevice[] devices = WebCamTexture.devices;
        if (devices.Length == 0)
        {
            Debug.LogError("No camera devices found");
            yield break;
        }

        string deviceName = devices[0].name;
        Debug.Log($"Using camera: {deviceName}");

        webCamTexture = new WebCamTexture(deviceName, 1280, 960, targetFPS);
        webCamTexture.Play();

        while (!webCamTexture.didUpdateThisFrame)
        {
            yield return null;
        }

        int width = webCamTexture.width;
        int height = webCamTexture.height;

        pixelBuffer = new Color32[width * height];
        imageDataRGB = new byte[width * height * 3];

        intrinsics = GetCameraIntrinsics(width, height);

        isRunning = true;
        Debug.Log($"Camera initialized: {width}x{height}");

        StartCoroutine(ProcessFrames());
    }

    private IEnumerator ProcessFrames()
    {
        while (isRunning)
        {
            if (webCamTexture != null && webCamTexture.didUpdateThisFrame)
            {
                ProcessCurrentFrame();
            }

            yield return new WaitForSeconds(1.0f / targetFPS);
        }
    }

    private void ProcessCurrentFrame()
    {
        try
        {
            webCamTexture.GetPixels32(pixelBuffer);

            int width = webCamTexture.width;
            int height = webCamTexture.height;

            for (int i = 0; i < pixelBuffer.Length; i++)
            {
                Color32 pixel = pixelBuffer[i];
                int idx = i * 3;
                imageDataRGB[idx] = pixel.r;
                imageDataRGB[idx + 1] = pixel.g;
                imageDataRGB[idx + 2] = pixel.b;
            }

            long timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() * 1000000;

            QuestVuforiaBridge.ProcessFrame(imageDataRGB, width, height, intrinsics, timestamp);
            frameCount++;
        }
        catch (Exception e)
        {
            Debug.LogError($"Error processing frame: {e}");
        }
    }

    private float[] GetCameraIntrinsics(int width, int height)
    {
        float focalLengthX = width * 0.8f;
        float focalLengthY = height * 0.8f;
        float principalPointX = width * 0.5f;
        float principalPointY = height * 0.5f;

        return new float[]
        {
            width, height,
            focalLengthX, focalLengthY,
            principalPointX, principalPointY,
            0, 0, 0, 0, 0, 0
        };
    }

    private void OnDestroy()
    {
        StopCamera();
    }

    private void OnApplicationPause(bool pauseStatus)
    {
        if (pauseStatus)
        {
            if (webCamTexture != null && webCamTexture.isPlaying)
            {
                webCamTexture.Pause();
            }
        }
        else
        {
            if (webCamTexture != null && !webCamTexture.isPlaying)
            {
                webCamTexture.Play();
            }
        }
    }

    public int GetFrameCount() => frameCount;
    public bool IsRunning() => isRunning;
    public WebCamTexture GetWebCamTexture() => webCamTexture;
}
