using System;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public struct TrackingResult
{
    public string name;
    public Matrix4x4 pose;
    public int status;
}

public static class QuestVuforiaBridge
{
    private static AndroidJavaObject plugin;
    private static bool initialized = false;

    private static AndroidJavaObject Plugin
    {
        get
        {
            if (plugin == null)
            {
                try
                {
                    using var unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
                    var activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
                    plugin = new AndroidJavaObject("com.quforia.QuestVuforiaManager", activity);
                    Debug.Log("QuestVuforiaBridge plugin instantiated");
                }
                catch (Exception e)
                {
                    Debug.LogError("QuestVuforiaBridge plugin init failed: " + e);
                }
            }
            return plugin;
        }
    }

    public static bool Initialize(string licenseKey)
    {
        if (initialized)
        {
            Debug.LogWarning("QuestVuforiaBridge already initialized");
            return true;
        }

        try
        {
            bool result = Plugin.Call<bool>("initialize", licenseKey);
            initialized = result;
            if (result)
            {
                Debug.Log("QuestVuforiaBridge initialized successfully");
            }
            else
            {
                Debug.LogError("QuestVuforiaBridge initialization failed");
            }
            return result;
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge Initialize exception: {e}");
            return false;
        }
    }

    public static void Shutdown()
    {
        if (!initialized) return;

        try
        {
            Plugin.Call("shutdown");
            initialized = false;
            Debug.Log("QuestVuforiaBridge shut down");
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge Shutdown exception: {e}");
        }
    }

    public static bool ProcessFrame(byte[] imageData, int width, int height, float[] intrinsics, long timestamp)
    {
        if (!initialized) return false;

        try
        {
            return Plugin.Call<bool>("processFrame", imageData, width, height, intrinsics, timestamp);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge ProcessFrame exception: {e}");
            return false;
        }
    }

    public static bool LoadImageTargetDatabase(string databasePath)
    {
        if (!initialized) return false;

        try
        {
            return Plugin.Call<bool>("loadImageTargetDatabase", databasePath);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge LoadImageTargetDatabase exception: {e}");
            return false;
        }
    }

    public static bool CreateImageTarget(string targetName)
    {
        if (!initialized) return false;

        try
        {
            return Plugin.Call<bool>("createImageTarget", targetName);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge CreateImageTarget exception: {e}");
            return false;
        }
    }

    public static void DestroyImageTarget(string targetName)
    {
        if (!initialized) return;

        try
        {
            Plugin.Call("destroyImageTarget", targetName);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge DestroyImageTarget exception: {e}");
        }
    }

    public static List<TrackingResult> GetImageTargetResults()
    {
        if (!initialized) return new List<TrackingResult>();

        try
        {
            AndroidJavaObject[] results = Plugin.Call<AndroidJavaObject[]>("getImageTargetResults");
            return ConvertTrackingResults(results);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge GetImageTargetResults exception: {e}");
            return new List<TrackingResult>();
        }
    }

    public static bool LoadModelTargetDatabase(string databasePath)
    {
        if (!initialized) return false;

        try
        {
            return Plugin.Call<bool>("loadModelTargetDatabase", databasePath);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge LoadModelTargetDatabase exception: {e}");
            return false;
        }
    }

    public static bool CreateModelTarget(string targetName, string guideViewName = null)
    {
        if (!initialized) return false;

        try
        {
            return Plugin.Call<bool>("createModelTarget", targetName, guideViewName);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge CreateModelTarget exception: {e}");
            return false;
        }
    }

    public static void DestroyModelTarget(string targetName)
    {
        if (!initialized) return;

        try
        {
            Plugin.Call("destroyModelTarget", targetName);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge DestroyModelTarget exception: {e}");
        }
    }

    public static List<TrackingResult> GetModelTargetResults()
    {
        if (!initialized) return new List<TrackingResult>();

        try
        {
            AndroidJavaObject[] results = Plugin.Call<AndroidJavaObject[]>("getModelTargetResults");
            return ConvertTrackingResults(results);
        }
        catch (Exception e)
        {
            Debug.LogError($"QuestVuforiaBridge GetModelTargetResults exception: {e}");
            return new List<TrackingResult>();
        }
    }

    private static List<TrackingResult> ConvertTrackingResults(AndroidJavaObject[] javaResults)
    {
        var results = new List<TrackingResult>();

        if (javaResults == null) return results;

        foreach (var javaResult in javaResults)
        {
            try
            {
                string name = javaResult.Call<string>("getName");
                float[] matrix = javaResult.Call<float[]>("getPoseMatrix");
                int status = javaResult.Call<int>("getStatus");

                Matrix4x4 pose = new Matrix4x4();
                for (int i = 0; i < 16; i++)
                {
                    pose[i] = matrix[i];
                }

                results.Add(new TrackingResult
                {
                    name = name,
                    pose = pose,
                    status = status
                });
            }
            catch (Exception e)
            {
                Debug.LogError($"Error converting tracking result: {e}");
            }
        }

        return results;
    }
}