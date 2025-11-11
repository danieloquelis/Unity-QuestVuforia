using System;
using UnityEngine;
using Vuforia;

/// <summary>
/// Initializes Vuforia with the Quest Vuforia Driver Framework.
/// This script must run before Vuforia is initialized (use Script Execution Order).
/// </summary>
[DefaultExecutionOrder(-100)]  // Execute before default scripts
public class QuestVuforiaDriverInit : MonoBehaviour
{
    [Header("Driver Configuration")]
    [Tooltip("Name of the native driver library")]
    [SerializeField] private string driverLibraryName = "libquforia.so";

    [Header("Debug")]
    [SerializeField] private bool enableDebugLogs = true;

    private bool isInitialized = false;

    private void Awake()
    {
        Log("QuestVuforiaDriverInit Awake");

        // Ensure Vuforia is configured for delayed initialization
        var config = VuforiaConfiguration.Instance;
        if (config == null)
        {
            LogError("VuforiaConfiguration not found! Please ensure Vuforia is properly imported.");
            return;
        }

        if (!config.Vuforia.DelayedInitialization)
        {
            LogWarning("DelayedInitialization is not enabled in VuforiaConfiguration. " +
                      "Driver Framework requires delayed initialization. Please enable it in Vuforia Configuration.");
        }
    }

    private void Start()
    {
        Log("QuestVuforiaDriverInit Start");
        InitializeVuforiaWithDriver();
    }

    private void InitializeVuforiaWithDriver()
    {
        if (isInitialized)
        {
            Log("Already initialized");
            return;
        }

        try
        {
            Log($"Initializing Vuforia with driver: {driverLibraryName}");

            // Register for initialization callback
            VuforiaApplication.Instance.OnVuforiaInitialized += OnVuforiaInitialized;
            VuforiaApplication.Instance.OnVuforiaDeinitialized += OnVuforiaDeinitialized;

            // Initialize Vuforia with the custom driver
            // The driver library (libquforia.so) will be loaded by Vuforia
            VuforiaApplication.Instance.Initialize(driverLibraryName, IntPtr.Zero);

            isInitialized = true;
            Log("Vuforia initialization requested with driver");
        }
        catch (Exception e)
        {
            LogError($"Failed to initialize Vuforia with driver: {e.Message}\n{e.StackTrace}");
        }
    }

    private void OnVuforiaInitialized(VuforiaInitError initError)
    {
        if (initError == VuforiaInitError.NONE)
        {
            Log("Vuforia initialized successfully with driver!");
        }
        else
        {
            LogError($"Vuforia initialization failed with error: {initError}");
        }
    }

    private void OnVuforiaDeinitialized()
    {
        Log("Vuforia deinitialized");
    }

    private void OnDestroy()
    {
        // Unregister events
        if (VuforiaApplication.Instance != null)
        {
            VuforiaApplication.Instance.OnVuforiaInitialized -= OnVuforiaInitialized;
            VuforiaApplication.Instance.OnVuforiaDeinitialized -= OnVuforiaDeinitialized;
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
