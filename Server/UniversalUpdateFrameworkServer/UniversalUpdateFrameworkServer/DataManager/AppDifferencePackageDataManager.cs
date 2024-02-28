using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public struct DifferencePackageFileInfo
{
    public string filePath;
    public string md5;
    public string version;
}

public class AppDifferencePackageDataManager : IDataManager
{
    private Mutex m_Mutex = new Mutex();

    private string m_AppDataFolderPath;

    public AppDifferencePackageDataManager(string appDataFolderPath)
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
                "appdifferencepackagemanifest.json");
        }
    }

    public JObject GetData(string appname, string appversion, string channel, string platform)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getData(getAppFolderPath(appname), appname, appversion, channel, platform,
                "appdifferencepackagemanifest.json");
        }
    }

    public DifferencePackageFileInfo GetCurrentDifferencePackageFilePath(string appname, string channel,
        string platform)
    {
        var jObject = GetCurrentData(appname, channel, platform);

        var filename = jObject["filename"];
        var md5 = jObject["md5"];
        var version = jObject["appversion"];

        DifferencePackageFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), channel, platform, version.ToString(),
            filename.ToString());
        info.md5 = md5.ToString();
        info.version = version.ToString();

        return info;
    }

    public DifferencePackageFileInfo getDifferencePackageFilePath(string appname, string appversion, string channel,
        string platform)
    {
        var jObject = GetData(appname, appversion, channel, platform);

        var filename = jObject["filename"];
        var md5 = jObject["md5"];
        var version = jObject["appversion"];

        DifferencePackageFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), channel, platform, appversion, filename.ToString());
        info.md5 = md5.ToString();
        info.version = version.ToString();

        return info;
    }
}