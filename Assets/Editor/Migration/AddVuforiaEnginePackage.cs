using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using UnityEditor;
using UnityEngine;

[InitializeOnLoad]
public class AddVuforiaEnginePackage
{
    static readonly string sPackagesPath = Path.Combine(Application.dataPath, "..", "Packages");
    static readonly string sManifestJsonPath = Path.Combine(sPackagesPath, "manifest.json");
    const string VUFORIA_VERSION = "11.4.4";
    const string VUFORIA_TAR_FILE_DIR = "Assets/Editor/Migration/";
    const string DEPENDENCIES_DIR = "Assets/Resources/VuforiaDependencies";
    const string PACKAGES_RELATIVE_PATH = "Packages";
    const string MRTK_PACKAGE = "org.mixedrealitytoolkit.core";
    const string OPEN_XR_PACKAGE = "com.microsoft.mixedreality.openxr";
    const string PACKAGE_NAME_REGEX = @"(([a-z]+)(\.[a-z0-9]+)*)(\-)?((\d+)\.(\d+)\.(\d+)(\-([a-z0-9\.])+)*)?(\.tgz)";

    static readonly ScopedRegistry sVuforiaRegistry = new ScopedRegistry
    {
        name = "Vuforia",
        url = "https://registry.packages.developer.vuforia.com/",
        scopes = new[] { "com.ptc.vuforia" }
    };

    static AddVuforiaEnginePackage()
    {
        if (Application.isBatchMode)
            return;
        
        var manifest = Manifest.JsonDeserialize(sManifestJsonPath);

        var packages = GetPackageDescriptions();
            
        if (!packages.All(p => IsVuforiaUpToDate(manifest, p.BundleId)))
            DisplayAddPackageDialog(manifest, packages);
        
        ResolveDependencies(manifest);
    }

    public static void ResolveDependenciesSilent()
    {
        var manifest = Manifest.JsonDeserialize(sManifestJsonPath);
        
        var packages = GetDependencyDescriptions();
        if (packages != null && packages.Count > 0)
            MoveDependencies(manifest, packages);
        
        CleanupDependenciesFolder();
    }
    
    static void ResolveDependencies(Manifest manifest)
    {
        var packages = GetDependencyDescriptions();
        if (packages != null && packages.Count > 0)
            DisplayDependenciesDialog(manifest, packages);
    }
    
    static bool IsVuforiaUpToDate(Manifest manifest, string bundleId)
    {
        var dependencies = manifest.Dependencies.Split(',').ToList();
        var upToDate = false;

        if(dependencies.Any(d => d.Contains(bundleId) && d.Contains("file:")))
            upToDate = IsUsingRightFileVersion(manifest, bundleId);

        return upToDate;
    }
    
    static bool IsUsingRightFileVersion(Manifest manifest, string bundleId)
    {
        var dependencies = manifest.Dependencies.Split(',').ToList();
        return dependencies.Any(d => d.Contains(bundleId) && d.Contains("file:") && VersionNumberIsTheLatestTarball(d));
    }

    static bool VersionNumberIsTheLatestTarball(string package)
    {
        var version = package.Split('-');
        if (version.Length >= 2)
        {
            version[1] = version[1].TrimEnd(".tgz\"".ToCharArray());
            return IsCurrentVersionHigher(version[1]);
        }

        return false;
    }

    static bool IsCurrentVersionHigher(string currentVersionString)
    {
        if (string.IsNullOrEmpty(currentVersionString) || string.IsNullOrEmpty(VUFORIA_VERSION))
            return false;

        var currentVersion = TryConvertStringToVersion(currentVersionString);
        var updatingVersion = TryConvertStringToVersion(VUFORIA_VERSION);
        
        if (currentVersion >= updatingVersion)
            return true;

        return false;
    }

    static Version TryConvertStringToVersion(string versionString)
    {
        Version res;
        try
        {
            res = new Version(versionString);
        }
        catch (Exception)
        {
            return new Version();
        }

        return new Version(res.Major, res.Minor, res.Build);
    }

    static void DisplayAddPackageDialog(Manifest manifest, IEnumerable<PackageDescription> packages)
    {
        if (EditorUtility.DisplayDialog("Add Vuforia Engine Package",
            $"Would you like to update your project to include the Vuforia Engine {VUFORIA_VERSION} package from the unitypackage?\n" +
            $"If an older Vuforia Engine package is already present in your project it will be upgraded to version {VUFORIA_VERSION}\n\n",
            "Update", "Cancel"))
        {
            foreach (var package in packages)
            {
                MovePackageFile(VUFORIA_TAR_FILE_DIR, package.FileName);
                UpdateManifest(manifest, package.BundleId, package.FileName);
            }
        }
    }
    
    static void DisplayDependenciesDialog(Manifest manifest, IEnumerable<PackageDescription> packages)
    {
        if (EditorUtility.DisplayDialog("Add Sample Dependencies",
                                        "Would you like to update your project to include all of its dependencies?\n" +
                                        "If a different version of the package is already present, it will be deleted.\n\n",
                                        "Update", "Cancel"))
        {
            MoveDependencies(manifest, packages);
            CleanupDependenciesFolder();
            if (ShouldProjectRestart(packages))
                DisplayRestartDialog();
        }
    }

    static void DisplayRestartDialog()
    {
        if (EditorUtility.DisplayDialog("Restart Unity Editor",
                                        "Due to a Unity lifecycle issue, this project needs to be closed and re-opened " +
                                        "after importing this Vuforia Engine sample.\n\n",
                                        "Restart", "Cancel"))
        {
            RestartEditor();
        }
    }

    static List<PackageDescription> GetPackageDescriptions()
    {
        var tarFilePaths = Directory.GetFiles(Path.Combine(Directory.GetCurrentDirectory(), VUFORIA_TAR_FILE_DIR)).Where(f => f.EndsWith(".tgz"));

        // Define a regular expression for repeated words.
        var rx = new Regex(PACKAGE_NAME_REGEX, RegexOptions.Compiled | RegexOptions.IgnoreCase);

        var packageDescriptions = new List<PackageDescription>();

        foreach (var filePath in tarFilePaths)
        {
            var fileName = Path.GetFileName(filePath);
            // Find matches.
            var matches = rx.Matches(fileName);

            // Report on each match.
            foreach (Match match in matches)
            {
                var groups = match.Groups;
                var bundleId = groups[1].Value;
                var versionString = groups[5].Value;

                if (string.Equals(versionString, VUFORIA_VERSION))
                {
                    packageDescriptions.Add(new PackageDescription()
                    {
                        BundleId = bundleId,
                        FileName = fileName
                    });
                }
            }
        }

        return packageDescriptions;
    }
    
    static List<PackageDescription> GetDependencyDescriptions()
    {
        var dependencyDirectory = Path.Combine(Directory.GetCurrentDirectory(), DEPENDENCIES_DIR);
        if (!Directory.Exists(dependencyDirectory))
            return null;
        var tarFilePaths = Directory.GetFiles(dependencyDirectory).Where(f => f.EndsWith(".tgz"));

        // Define a regular expression for repeated words.
        var rx = new Regex(PACKAGE_NAME_REGEX, RegexOptions.Compiled | RegexOptions.IgnoreCase);

        var packageDescriptions = new List<PackageDescription>();

        foreach (var filePath in tarFilePaths)
        {
            var fileName = Path.GetFileName(filePath);
            // Find matches.
            var matches = rx.Matches(fileName);

            // Report on each match.
            foreach (Match match in matches)
            {
                var groups = match.Groups;
                var bundleId = groups[1].Value;
                bundleId = bundleId.Replace(".tgz", "");

                packageDescriptions.Add(new PackageDescription
                                        {
                                            BundleId = bundleId,
                                            FileName = fileName
                                        });
            }
        }

        return packageDescriptions;
    }

    static void MoveDependencies(Manifest manifest, IEnumerable<PackageDescription> packages)
    {
        foreach (var package in packages)
        {
            RemoveDependency(manifest, package.BundleId, package.FileName);
            MovePackageFile(DEPENDENCIES_DIR, package.FileName);
            UpdateManifest(manifest, package.BundleId, package.FileName);
        }
    }
    
    static void MovePackageFile(string folder, string fileName)
    {
        var sourceFile = Path.Combine(Directory.GetCurrentDirectory(), folder, fileName);
        var destFile = Path.Combine(Directory.GetCurrentDirectory(), PACKAGES_RELATIVE_PATH, fileName);
        File.Copy(sourceFile, destFile, true);
        File.Delete(sourceFile);
        File.Delete(sourceFile + ".meta");
    }

    static void UpdateManifest(Manifest manifest, string bundleId, string fileName)
    {
        //remove existing, outdated NPM scoped registry if present
        var registries = manifest.ScopedRegistries.ToList();
        if (registries.Contains(sVuforiaRegistry))
        {
            registries.Remove(sVuforiaRegistry);
            manifest.ScopedRegistries = registries.ToArray();
        }

        //add specified vuforia version via Git URL
        SetVuforiaVersion(manifest, bundleId, fileName);

        manifest.JsonSerialize(sManifestJsonPath);

        AssetDatabase.Refresh();
    }

    static void RemoveDependency(Manifest manifest, string bundleId, string fileName)
    {
        var destFile = Path.Combine(Directory.GetCurrentDirectory(), PACKAGES_RELATIVE_PATH, fileName);
        if (File.Exists(destFile))
            File.Delete(destFile);
        
        // remove existing
        var dependencies = manifest.Dependencies.Split(',').ToList();
        for (var i = 0; i < dependencies.Count; i++)
        {
            if (dependencies[i].Contains(bundleId))
            {
                dependencies.RemoveAt(i);
                break;
            }
        }

        manifest.Dependencies = string.Join(",", dependencies);

        manifest.JsonSerialize(sManifestJsonPath);

        AssetDatabase.Refresh();
    }

    static void CleanupDependenciesFolder()
    {
        if (!Directory.Exists(DEPENDENCIES_DIR)) 
            return;
        
        Directory.Delete(DEPENDENCIES_DIR);
        File.Delete(DEPENDENCIES_DIR + ".meta");
        AssetDatabase.Refresh();
    }

    static bool ShouldProjectRestart(IEnumerable<PackageDescription> packages)
    {
        return packages.Any(p => p.BundleId == MRTK_PACKAGE || p.BundleId == OPEN_XR_PACKAGE);
    }

    static void RestartEditor()
    {
        EditorApplication.OpenProject(Directory.GetCurrentDirectory());
    }

    static void SetVuforiaVersion(Manifest manifest, string bundleId, string fileName)
    {
        var dependencies = manifest.Dependencies.Split(',').ToList();

        var versionEntry = $"\"file:{fileName}\"";
        var versionSet = false;
        for (var i = 0; i < dependencies.Count; i++)
        {
            if (!dependencies[i].Contains(bundleId))
                continue;

            var kvp = dependencies[i].Split(':');
            dependencies[i] = kvp[0] + ": " + versionEntry;
            versionSet = true;
        }

        if (!versionSet)
            dependencies.Insert(0, $"\n    \"{bundleId}\": {versionEntry}");

        manifest.Dependencies = string.Join(",", dependencies);
    }

    class Manifest
    {
        const int INDEX_NOT_FOUND = -1;
        const string DEPENDENCIES_KEY = "\"dependencies\"";

        public ScopedRegistry[] ScopedRegistries;
        public string Dependencies;

        public void JsonSerialize(string path)
        {
            var jsonString = GetJsonString();

            var startIndex = GetDependenciesStart(jsonString);
            var endIndex = GetDependenciesEnd(jsonString, startIndex);

            var stringBuilder = new StringBuilder();

            stringBuilder.Append(jsonString.Substring(0, startIndex));
            stringBuilder.Append(Dependencies);
            stringBuilder.Append(jsonString.Substring(endIndex, jsonString.Length - endIndex));

            File.WriteAllText(path, stringBuilder.ToString());
        }

        string GetJsonString()
        {
            if (ScopedRegistries.Length > 0)
                return JsonUtility.ToJson(
                    new UnitySerializableManifest { scopedRegistries = ScopedRegistries, dependencies = new DependencyPlaceholder() },
                    true);

            return JsonUtility.ToJson(
                new UnitySerializableManifestDependenciesOnly() { dependencies = new DependencyPlaceholder() },
                true);
        }


        public static Manifest JsonDeserialize(string path)
        {
            var jsonString = File.ReadAllText(path);

            var registries = JsonUtility.FromJson<UnitySerializableManifest>(jsonString).scopedRegistries ?? new ScopedRegistry[0];
            var dependencies = DeserializeDependencies(jsonString);

            return new Manifest { ScopedRegistries = registries, Dependencies = dependencies };
        }

        static string DeserializeDependencies(string json)
        {
            var startIndex = GetDependenciesStart(json);
            var endIndex = GetDependenciesEnd(json, startIndex);

            if (startIndex == INDEX_NOT_FOUND || endIndex == INDEX_NOT_FOUND)
                return null;

            var dependencies = json.Substring(startIndex, endIndex - startIndex);
            return dependencies;
        }

        static int GetDependenciesStart(string json)
        {
            var dependenciesIndex = json.IndexOf(DEPENDENCIES_KEY, StringComparison.InvariantCulture);
            if (dependenciesIndex == INDEX_NOT_FOUND)
                return INDEX_NOT_FOUND;

            var dependenciesStartIndex = json.IndexOf('{', dependenciesIndex + DEPENDENCIES_KEY.Length);

            if (dependenciesStartIndex == INDEX_NOT_FOUND)
                return INDEX_NOT_FOUND;

            dependenciesStartIndex++; //add length of '{' to starting point

            return dependenciesStartIndex;
        }

        static int GetDependenciesEnd(string jsonString, int dependenciesStartIndex)
        {
            return jsonString.IndexOf('}', dependenciesStartIndex);
        }
    }

    class UnitySerializableManifestDependenciesOnly
    {
        public DependencyPlaceholder dependencies;
    }

    class UnitySerializableManifest
    {
        public ScopedRegistry[] scopedRegistries;
        public DependencyPlaceholder dependencies;
    }

    [Serializable]
    struct ScopedRegistry
    {
        public string name;
        public string url;
        public string[] scopes;

        public override bool Equals(object obj)
        {
            if (!(obj is ScopedRegistry))
                return false;

            var other = (ScopedRegistry)obj;

            return name == other.name &&
                   url == other.url &&
                   scopes.SequenceEqual(other.scopes);
        }

        public static bool operator ==(ScopedRegistry a, ScopedRegistry b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(ScopedRegistry a, ScopedRegistry b)
        {
            return !a.Equals(b);
        }

        public override int GetHashCode()
        {
            var hash = 17;

            foreach (var scope in scopes)
                hash = hash * 23 + (scope == null ? 0 : scope.GetHashCode());

            hash = hash * 23 + (name == null ? 0 : name.GetHashCode());
            hash = hash * 23 + (url == null ? 0 : url.GetHashCode());

            return hash;
        }
    }

    [Serializable]
    struct DependencyPlaceholder { }
    
    struct PackageDescription
    {
        public string BundleId;
        public string FileName;
    }
}