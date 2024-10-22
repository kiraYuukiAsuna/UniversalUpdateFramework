using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public class FileCdnUrlDataManager(string appDataFolderPath) : IDataManager
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
                "FileCdnUrlList.json");
        }
    }

    public JObject GetData(string appname, string appversion, string channel, string platform)
    {
        lock (m_Mutex)
        {
            return DataManagerUtil.getData(getAppFolderPath(appname), appname, appversion, channel, platform,
                "FileCdnUrlList.json");
        }
    }
}