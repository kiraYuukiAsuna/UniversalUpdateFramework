using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public struct DownloadFileInfo
{
    public string filePath;
    public string md5;
    public string version;
}

public class AppFullPackageDataManager : IDataManager
{
    private Mutex m_Mutex = new Mutex();

    private string m_AppDataFolderPath;

    public AppFullPackageDataManager(string appDataFolderPath)
    {
        m_AppDataFolderPath = appDataFolderPath;
    }

    public string getAppFolderPath(string appname)
    {
        return Path.Combine(m_AppDataFolderPath, appname);
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

        var filename = jObject["filename"];
        var md5 = jObject["md5"];
        var version = jObject["appversion"];

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

        var filename = jObject["filename"];
        var md5 = jObject["md5"];
        var version = jObject["appversion"];

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

        foreach (var fileInfo in manifest["manifest"])
        {
            if (fileInfo["md5"].ToString() == md5)
            {
                relativePath = fileInfo["filepath"].ToString();
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