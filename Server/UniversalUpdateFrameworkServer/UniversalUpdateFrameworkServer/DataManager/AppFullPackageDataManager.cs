using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public struct DownloadFileInfo
{
    public string filePath;
    public string md5;
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
    
    public JObject GetCurrentData(string appname)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getCurrentData(getAppFolderPath(appname), appname, "appfullpackagemanifest.json");
        }
    }

    public JObject GetData(string appname, string appversion)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getData(getAppFolderPath(appname), appname, appversion,"appfullpackagemanifest.json");
        }
    }

    public DownloadFileInfo GetCurrentFullPackageFilePath(string appname)
    {
        var jObject = GetCurrentData(appname);

        var filename = jObject["filename"];
        var md5 = jObject["md5"];

        DownloadFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), "current", filename.ToString());
        info.md5 = md5.ToString();
        
        return info;
    }
    
    public DownloadFileInfo GetFullPackageFilePath(string appname, string appversion)
    {
        var jObject = GetData(appname, appversion);

        var filename = jObject["filename"];
        var md5 = jObject["md5"];

        DownloadFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), appversion, filename.ToString());
        info.md5 = md5.ToString();
        
        return info;
    }
    
    public DownloadFileInfo GetFileFromFullPackageFilePath(string appname, string appversion, string md5)
    {
        AppManifestDataManager appManifestDataManager = new AppManifestDataManager("appdata");
        
        var manifest = appManifestDataManager.GetData(appname, appversion);

        string relativePath = "";
        
        foreach (var fileInfo in manifest["manifest"])
        {
            if (fileInfo["md5"].ToString() == md5)
            {
                relativePath = fileInfo["filepath"].ToString();
            }
        }
        
        DownloadFileInfo info;
        info.filePath = Path.Combine(getAppFolderPath(appname), appversion, "fullpackage", relativePath);
        info.md5 = md5.ToString();
        
        return info;
    }
    
}
