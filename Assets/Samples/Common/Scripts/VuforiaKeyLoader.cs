using UnityEngine;
using System.IO;
using Vuforia;

public class VuforiaKeyLoader : MonoBehaviour
{
    private void Awake()
    {
        var path = Path.Combine(Application.streamingAssetsPath, "VuforiaKeyLicense.txt");
        if (File.Exists(path))
        {
            var key = File.ReadAllText(path).Trim();
            VuforiaConfiguration.Instance.Vuforia.LicenseKey = key;
        }
        else
        {
            Debug.LogError("Vuforia license key file not found!");
        }
    }
}