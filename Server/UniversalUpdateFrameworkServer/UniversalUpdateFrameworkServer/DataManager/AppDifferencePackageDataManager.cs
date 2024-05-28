using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public struct DifferencePackageFileInfo
{
    public string filePath;
    public string md5;
    public string version;
}

public class AppDifferencePackageDataManager(string appDataFolderPath) : IDataManager
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

        var filename = jObject["FileName"];
        var md5 = jObject["Md5"];
        var version = jObject["AppCurrentVersion"];

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

        var filename = jObject["FileName"];
        var md5 = jObject["Md5"];
        var version = jObject["AppCurrentVersion"];

        DifferencePackageFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), channel, platform, appversion, filename.ToString());
        info.md5 = md5.ToString();
        info.version = version.ToString();

        return info;
    }
}