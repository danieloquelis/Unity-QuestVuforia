using System;
using UnityEngine;
using Vuforia;

/// <summary>
/// Initializes Vuforia with the Quest custom driver.
/// Requires delayed initialization enabled in VuforiaConfiguration.
/// </summary>
[DefaultExecutionOrder(-100)]
public class QuestVuforiaDriverInit : MonoBehaviour
{
    [SerializeField] private string driverLibraryName = "quforia";
    [SerializeField] private bool enableDebugLogs = false;

    private void Start()
    {
        InitializeVuforiaWithDriver();
    }

    private void InitializeVuforiaWithDriver()
    {
        try
        {
            Log($"Initializing Vuforia with driver: {driverLibraryName}");

            VuforiaApplication.Instance.OnVuforiaInitialized += OnVuforiaInitialized;
            VuforiaApplication.Instance.OnVuforiaDeinitialized += OnVuforiaDeinitialized;
            VuforiaApplication.Instance.Initialize(driverLibraryName, IntPtr.Zero);
        }
        catch (Exception e)
        {
            Debug.LogError($"[Quforia] Driver initialization failed: {e.Message}");
        }
    }

    private void OnVuforiaInitialized(VuforiaInitError error)
    {
        if (error == VuforiaInitError.NONE)
        {
            Log("Vuforia initialized successfully");
        }
        else
        {
            Debug.LogError($"[Quforia] Initialization error: {error}");
        }
    }

    private void OnVuforiaDeinitialized()
    {
        Log("Vuforia deinitialized");
    }

    private void OnDestroy()
    {
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
            Debug.Log($"[Quforia] {message}");
        }
    }
}
