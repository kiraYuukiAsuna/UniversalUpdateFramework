using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public struct DownloadFileInfo
{
    public string filePath;
    public string md5;
    public string version;
}

public class AppFullPackageDataManager(string appDataFolderPath) : IDataManager
{
    private Mutex m_Mutex = new();

    public string getAppFolderPath(string appname)
    {
        return Path.Combine(appDataFolderPath, appname);
    }

    public JObject GetCurrentData(string appname, string channel, string platform)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getCurrentData(getAppFolderPath(appname), appname, channel, platform,
                "appfullpackagemanifest.json");
        }
    }

    public JObject GetData(string appname, string appversion, string channel, string platform)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getData(getAppFolderPath(appname), appname, appversion, channel, platform,
                "appfullpackagemanifest.json");
        }
    }

    public DownloadFileInfo GetCurrentFullPackageFilePath(string appname, string channel, string platform)
    {
        var jObject = GetCurrentData(appname, channel, platform);

        var filename = jObject["FileName"];
        var md5 = jObject["Md5"];
        var version = jObject["AppVersion"];

        DownloadFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), channel, platform, version.ToString(),
            filename.ToString());
        info.md5 = md5.ToString();
        info.version = version.ToString();

        return info;
    }

    public DownloadFileInfo GetFullPackageFilePath(string appname, string appversion, string channel, string platform)
    {
        var jObject = GetData(appname, appversion, channel, platform);

        var filename = jObject["FileName"];
        var md5 = jObject["Md5"];
        var version = jObject["AppVersion"];

        DownloadFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), channel, platform, appversion, filename.ToString());
        info.md5 = md5.ToString();
        info.version = version.ToString();

        return info;
    }

    public DownloadFileInfo GetFileFromFullPackageFilePath(string appname, string appversion, string channel,
        string platform, string md5)
    {
        AppManifestDataManager appManifestDataManager = new AppManifestDataManager("appdata");

        var manifest = appManifestDataManager.GetData(appname, appversion, channel, platform);

        string relativePath = "";

        foreach (var fileInfo in manifest["Manifests"])
        {
            if (fileInfo["Md5"].ToString() == md5)
            {
                relativePath = fileInfo["FilePath"].ToString();
            }
        }

        DownloadFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), channel, platform, appversion, "fullpackage",
            relativePath);
        info.md5 = md5.ToString();
        info.version = appversion;

        return info;
    }
}