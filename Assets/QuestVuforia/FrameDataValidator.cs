using UnityEngine;
using Meta.XR;
using Unity.Collections;

/// <summary>
/// Validates camera frame data to ensure Vuforia is receiving valid images.
/// Attach to PassthroughCameraAccess GameObject.
/// </summary>
public class FrameDataValidator : MonoBehaviour
{
    private PassthroughCameraAccess cameraAccess;
    private int validationFrame = 0;
    private const int VALIDATION_INTERVAL = 120; // Every 2 seconds @ 60fps

    void Start()
    {
        cameraAccess = GetComponent<PassthroughCameraAccess>();
        if (cameraAccess == null)
        {
            Debug.LogError("[FrameValidator] No PassthroughCameraAccess found!");
            enabled = false;
            return;
        }

        Debug.Log("[FrameValidator] Started frame validation");
    }

    void Update()
    {
        if (!cameraAccess.IsPlaying)
            return;

        validationFrame++;

        if (validationFrame >= VALIDATION_INTERVAL)
        {
            validationFrame = 0;
            ValidateCurrentFrame();
        }
    }

    void ValidateCurrentFrame()
    {
        // UPDATED: Validate GetColors() which is what we actually send to Vuforia
        NativeArray<Color32> pixels = cameraAccess.GetColors();

        if (!pixels.IsCreated || pixels.Length == 0)
        {
            Debug.LogWarning("[FrameValidator] GetColors() returned invalid or empty NativeArray!");
            return;
        }

        Vector2Int resolution = cameraAccess.CurrentResolution;
        int width = resolution.x;
        int height = resolution.y;

        Debug.Log($"[FrameValidator] ===== FRAME VALIDATION (GetColors) =====");
        Debug.Log($"[FrameValidator] Resolution: {width}x{height}");
        Debug.Log($"[FrameValidator] Pixel count: {pixels.Length}");
        Debug.Log($"[FrameValidator] Expected pixels: {width * height}");

        // Sample center pixel
        int centerIdx = (height / 2) * width + (width / 2);
        Color32 centerPixel = pixels[centerIdx];
        Debug.Log($"[FrameValidator] Center Pixel: R={centerPixel.r}, G={centerPixel.g}, B={centerPixel.b}");

        // Sample corners
        Color32 topLeft = pixels[10 * width + 10];
        Color32 topRight = pixels[10 * width + (width - 10)];
        Color32 bottomLeft = pixels[(height - 10) * width + 10];
        Color32 bottomRight = pixels[(height - 10) * width + (width - 10)];

        Debug.Log($"[FrameValidator] TopLeft: R={topLeft.r}, G={topLeft.g}, B={topLeft.b}");
        Debug.Log($"[FrameValidator] TopRight: R={topRight.r}, G={topRight.g}, B={topRight.b}");
        Debug.Log($"[FrameValidator] BottomLeft: R={bottomLeft.r}, G={bottomLeft.g}, B={bottomLeft.b}");
        Debug.Log($"[FrameValidator] BottomRight: R={bottomRight.r}, G={bottomRight.g}, B={bottomRight.b}");

        // Check if image is all black (common error)
        if (centerPixel.r < 2 && centerPixel.g < 2 && centerPixel.b < 2 &&
            topLeft.r < 2 && topRight.r < 2)
        {
            Debug.LogError("[FrameValidator] ***** FRAME IS ALL BLACK! *****");
        }
        // Check if image is all white
        else if (centerPixel.r > 253 && centerPixel.g > 253 && centerPixel.b > 253 &&
                 topLeft.r > 253 && topRight.r > 253)
        {
            Debug.LogError("[FrameValidator] ***** FRAME IS ALL WHITE! *****");
        }
        else
        {
            Debug.Log("[FrameValidator] ✓ Frame appears to have valid image data");
        }

        // Sample first few pixels
        Debug.Log($"[FrameValidator] First 10 pixels:");
        for (int i = 0; i < 10 && i < pixels.Length; i++)
        {
            Color32 p = pixels[i];
            Debug.Log($"  Pixel[{i}]: R={p.r}, G={p.g}, B={p.b}, A={p.a}");
        }
    }
}
