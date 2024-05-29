using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using UniversalUpdateFrameworkServer.Definition;

namespace UniversalUpdateFrameworkServer.DataManager;

public class UpdaterUpdateSelfDataManager(string updaterDataFolderPath)
{
    private Mutex m_Mutex = new();

    private string m_UpdaterDataFolderPath = updaterDataFolderPath;

    public string getUpdaterFolderPath(string updaterName)
    {
        return Path.Combine(m_UpdaterDataFolderPath, updaterName);
    }

    public string GetCurrentData(string updaterName, string channel, string platform)
    {
        lock (m_Mutex)
        {
            string currentVersionFolderName = "";
            string appVersionFilePath = "";

            var appVersionsPath = Path.Combine(getUpdaterFolderPath(updaterName), channel, platform);

            var versionconfigJsonFilePath = Path.Combine(appVersionsPath, "versionconfig.json");
            if (File.Exists(versionconfigJsonFilePath))
            {
                string currentFileContent = File.ReadAllText(versionconfigJsonFilePath);
                var obj = JObject.Parse(currentFileContent);
                if (obj.ContainsKey("current_version"))
                {
                    currentVersionFolderName = obj["current_version"].ToString();
                }
            }

            var currentVersionFolderPath = Path.Combine(appVersionsPath, currentVersionFolderName);

            if (!Directory.Exists(currentVersionFolderPath))
            {
                var versionFolders = Directory.GetDirectories(appVersionsPath)
                    .OrderByDescending(f => new FileInfo(f).Name).ToArray();

                bool bGetOneVersion = false;

                foreach (var versionFolder in versionFolders)
                {
                    if (bGetOneVersion)
                    {
                        break;
                    }

                    currentVersionFolderPath = versionFolder;
                    bGetOneVersion = true;
                }

                if (!bGetOneVersion)
                {
                    return "";
                }
            }

            return currentVersionFolderName;
        }
    }

    public string GetUpdaterVersionConfig(string updaterName, string channel, string platform)
    {
        lock (m_Mutex)
        {
            var versionconfigJsonFilePath = Path.Combine(getUpdaterFolderPath(updaterName), channel, platform,
                "versionconfig.json");
            return File.ReadAllText(versionconfigJsonFilePath);
        }
    }
    
    public DownloadFileInfo GetFile(string updaterName, string updaterVersion, string channel,
        string platform, string md5)
    {
        var versionconfigJsonFilePath = Path.Combine(getUpdaterFolderPath(updaterName), channel, platform,
            "versionconfig.json");
        string jsonContent = File.ReadAllText(versionconfigJsonFilePath);
        var versionConfig = JsonConvert.DeserializeObject<AppVersionConfigInfo>(jsonContent);

        string relativePath = "";
        foreach (var version in versionConfig.Versions)
        {
            foreach (var manifest in version.FileManifests)
            {
                if (manifest.Md5 == md5)
                {
                    relativePath = manifest.FilePath;
                }
            }
        }

        DownloadFileInfo info;
        info.filePath = Path.Combine(getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "fullpackage",
            relativePath);
        info.md5 = md5;
        info.version = updaterVersion;

        return info;
    }
    
}