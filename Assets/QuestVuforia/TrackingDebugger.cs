using UnityEngine;
using Vuforia;

/// <summary>
/// Debug script to monitor Vuforia tracking status in real-time.
/// Attach to ImageTarget GameObject.
/// </summary>
public class TrackingDebugger : MonoBehaviour
{
    private ObserverBehaviour observerBehaviour;
    private TargetStatus lastStatus;
    private int framesSinceLastLog = 0;
    private const int LOG_INTERVAL = 120; // Log every 120 frames (~2 seconds @ 60fps)

    void Start()
    {
        observerBehaviour = GetComponent<ObserverBehaviour>();

        if (observerBehaviour == null)
        {
            Debug.LogError("[TrackingDebugger] No ObserverBehaviour found on this GameObject!");
            enabled = false;
            return;
        }

        observerBehaviour.OnTargetStatusChanged += OnTargetStatusChanged;

        Debug.Log($"[TrackingDebugger] ===== MONITORING TARGET: {observerBehaviour.TargetName} =====");
        Debug.Log($"[TrackingDebugger] Initial Status: {observerBehaviour.TargetStatus.Status}");
        Debug.Log($"[TrackingDebugger] Initial StatusInfo: {observerBehaviour.TargetStatus.StatusInfo}");
    }

    void OnTargetStatusChanged(ObserverBehaviour behaviour, TargetStatus targetStatus)
    {
        Debug.Log($"[TrackingDebugger] ***** STATUS CHANGE *****");
        Debug.Log($"[TrackingDebugger] Target: {behaviour.TargetName}");
        Debug.Log($"[TrackingDebugger] Status: {targetStatus.Status}");
        Debug.Log($"[TrackingDebugger] StatusInfo: {targetStatus.StatusInfo}");

        if (targetStatus.Status == Status.TRACKED)
        {
            Debug.Log($"[TrackingDebugger] *** TARGET TRACKED! Position: {transform.position} ***");
        }
        else if (targetStatus.Status == Status.EXTENDED_TRACKED)
        {
            Debug.Log($"[TrackingDebugger] *** EXTENDED TRACKING ACTIVE ***");
        }
        else if (targetStatus.Status == Status.NO_POSE)
        {
            Debug.Log($"[TrackingDebugger] *** NOT TRACKING (NO_POSE) ***");
        }

        lastStatus = targetStatus;
    }

    void Update()
    {
        framesSinceLastLog++;

        if (framesSinceLastLog >= LOG_INTERVAL)
        {
            framesSinceLastLog = 0;

            var currentStatus = observerBehaviour.TargetStatus;
            Debug.Log($"[TrackingDebugger] Periodic Status: {currentStatus.Status}, StatusInfo: {currentStatus.StatusInfo}");

            if (currentStatus.Status == Status.NO_POSE)
            {
                Debug.Log($"[TrackingDebugger] Still searching for target '{observerBehaviour.TargetName}'...");
            }
        }
    }

    void OnDestroy()
    {
        if (observerBehaviour != null)
        {
            observerBehaviour.OnTargetStatusChanged -= OnTargetStatusChanged;
        }
    }
}
