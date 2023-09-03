using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public class AppManifestDataManager : IDataManager
{
    private Mutex m_Mutex = new Mutex();

    private string m_AppDataFolderPath;

    public AppManifestDataManager(string appDataFolderPath)
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
            return DataManagerUtil.getCurrentData(getAppFolderPath(appname), appname, "appmanifest.json");
        }
    }

    public JObject GetData(string appname, string appversion)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getData(getAppFolderPath(appname), appname, appversion, "appmanifest.json");
        }
    }
}
