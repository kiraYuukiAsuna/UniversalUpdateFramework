using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public struct DifferencePackageFileInfo
{
    public string filePath;
    public string md5;
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
    
    public JObject GetCurrentData(string appname)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getCurrentData(getAppFolderPath(appname), appname,"appdifferencepackagemanifest.json");
        }
    }

    public JObject GetData(string appname, string appversion)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getData(getAppFolderPath(appname), appname, appversion,
                "appdifferencepackagemanifest.json");
        }
    }

    public DifferencePackageFileInfo GetCurrentDifferencePackageFilePath(string appname)
    {
        var jObject = GetCurrentData(appname);

        var filename = jObject["filename"];
        var md5 = jObject["md5"];

        DifferencePackageFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), "current",filename.ToString());
        info.md5 = md5.ToString();
        
        return info;
    }
    
    public DifferencePackageFileInfo getDifferencePackageFilePath(string appname, string appversion)
    {
        var jObject = GetData(appname, appversion);

        var filename = jObject["filename"];
        var md5 = jObject["md5"];

        DifferencePackageFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), appversion, filename.ToString());
        info.md5 = md5.ToString();
        
        return info;
    }
    
}
