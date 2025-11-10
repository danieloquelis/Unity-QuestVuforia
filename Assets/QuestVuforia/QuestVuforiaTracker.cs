using System;
using System.Collections.Generic;
using UnityEngine;

public enum TargetType
{
    ImageTarget,
    ModelTarget
}

[Serializable]
public class TargetConfig
{
    public string name;
    public TargetType type;
    public string databasePath;
    public string guideViewName;
    public GameObject anchorPrefab;
}

public class QuestVuforiaTracker : MonoBehaviour
{
    [SerializeField] private List<TargetConfig> targets = new List<TargetConfig>();
    [SerializeField] private bool updateEveryFrame = true;
    [SerializeField] private float updateRate = 30f;

    private Dictionary<string, GameObject> targetAnchors = new Dictionary<string, GameObject>();
    private HashSet<string> loadedDatabases = new HashSet<string>();
    private float lastUpdateTime;

    private void Start()
    {
        InitializeTargets();
    }

    private void InitializeTargets()
    {
        foreach (var config in targets)
        {
            if (!string.IsNullOrEmpty(config.databasePath) && !loadedDatabases.Contains(config.databasePath))
            {
                bool loaded = config.type == TargetType.ImageTarget
                    ? QuestVuforiaBridge.LoadImageTargetDatabase(config.databasePath)
                    : QuestVuforiaBridge.LoadModelTargetDatabase(config.databasePath);

                if (loaded)
                {
                    loadedDatabases.Add(config.databasePath);
                    Debug.Log($"Loaded database: {config.databasePath}");
                }
                else
                {
                    Debug.LogError($"Failed to load database: {config.databasePath}");
                }
            }

            bool created = config.type == TargetType.ImageTarget
                ? QuestVuforiaBridge.CreateImageTarget(config.name)
                : QuestVuforiaBridge.CreateModelTarget(config.name, config.guideViewName);

            if (created)
            {
                Debug.Log($"Created {config.type}: {config.name}");

                if (config.anchorPrefab != null)
                {
                    GameObject anchor = Instantiate(config.anchorPrefab, Vector3.zero, Quaternion.identity);
                    anchor.name = $"Anchor_{config.name}";
                    anchor.SetActive(false);
                    targetAnchors[config.name] = anchor;
                }
            }
            else
            {
                Debug.LogError($"Failed to create {config.type}: {config.name}");
            }
        }
    }

    private void Update()
    {
        if (!updateEveryFrame)
        {
            if (Time.time - lastUpdateTime < 1f / updateRate)
            {
                return;
            }
            lastUpdateTime = Time.time;
        }

        UpdateTracking();
    }

    private void UpdateTracking()
    {
        var trackedTargets = new HashSet<string>();

        var imageResults = QuestVuforiaBridge.GetImageTargetResults();
        foreach (var result in imageResults)
        {
            UpdateTargetPose(result.name, result.pose);
            trackedTargets.Add(result.name);
        }

        var modelResults = QuestVuforiaBridge.GetModelTargetResults();
        foreach (var result in modelResults)
        {
            UpdateTargetPose(result.name, result.pose);
            trackedTargets.Add(result.name);
        }

        foreach (var kvp in targetAnchors)
        {
            if (!trackedTargets.Contains(kvp.Key))
            {
                kvp.Value.SetActive(false);
            }
        }
    }

    private void UpdateTargetPose(string targetName, Matrix4x4 pose)
    {
        if (targetAnchors.TryGetValue(targetName, out GameObject anchor))
        {
            anchor.SetActive(true);

            Matrix4x4 unityPose = ConvertVuforiaToUnity(pose);

            anchor.transform.position = unityPose.GetColumn(3);
            anchor.transform.rotation = Quaternion.LookRotation(
                unityPose.GetColumn(2),
                unityPose.GetColumn(1)
            );
        }
    }

    private Matrix4x4 ConvertVuforiaToUnity(Matrix4x4 vuforiaPose)
    {
        Matrix4x4 converted = Matrix4x4.identity;

        converted.m00 = vuforiaPose.m00;
        converted.m01 = -vuforiaPose.m02;
        converted.m02 = vuforiaPose.m01;
        converted.m03 = vuforiaPose.m03;

        converted.m10 = vuforiaPose.m20;
        converted.m11 = -vuforiaPose.m22;
        converted.m12 = vuforiaPose.m21;
        converted.m13 = vuforiaPose.m23;

        converted.m20 = -vuforiaPose.m10;
        converted.m21 = vuforiaPose.m12;
        converted.m22 = -vuforiaPose.m11;
        converted.m23 = -vuforiaPose.m13;

        converted.m30 = vuforiaPose.m30;
        converted.m31 = vuforiaPose.m31;
        converted.m32 = vuforiaPose.m32;
        converted.m33 = vuforiaPose.m33;

        return converted;
    }

    private void OnDestroy()
    {
        foreach (var config in targets)
        {
            if (config.type == TargetType.ImageTarget)
            {
                QuestVuforiaBridge.DestroyImageTarget(config.name);
            }
            else
            {
                QuestVuforiaBridge.DestroyModelTarget(config.name);
            }
        }

        foreach (var anchor in targetAnchors.Values)
        {
            if (anchor != null)
            {
                Destroy(anchor);
            }
        }

        targetAnchors.Clear();
    }

    public void AddTarget(TargetConfig config)
    {
        targets.Add(config);
        InitializeTargets();
    }

    public void RemoveTarget(string targetName)
    {
        var config = targets.Find(t => t.name == targetName);
        if (config != null)
        {
            targets.Remove(config);

            if (config.type == TargetType.ImageTarget)
            {
                QuestVuforiaBridge.DestroyImageTarget(targetName);
            }
            else
            {
                QuestVuforiaBridge.DestroyModelTarget(targetName);
            }

            if (targetAnchors.TryGetValue(targetName, out GameObject anchor))
            {
                Destroy(anchor);
                targetAnchors.Remove(targetName);
            }
        }
    }

    public GameObject GetTargetAnchor(string targetName)
    {
        targetAnchors.TryGetValue(targetName, out GameObject anchor);
        return anchor;
    }
}
