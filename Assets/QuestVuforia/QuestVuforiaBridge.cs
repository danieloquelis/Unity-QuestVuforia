using System;
using UnityEngine;

public static class QuestVuforiaBridge 
{
    private static readonly AndroidJavaObject Plugin;

    static QuestVuforiaBridge()
    {
        try 
        {
            using var unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
            var activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
            Plugin = new AndroidJavaObject("com.quforia.QuestVuforiaManager", activity);
            Debug.Log("QuestVuforiaBridge initialized");
        }
        catch (Exception e)
        {
            Debug.LogError("QuestVuforiaBridge init failed: " + e);
        }    
    }

}