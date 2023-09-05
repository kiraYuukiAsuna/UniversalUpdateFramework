using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public interface IDataManager
{
    public JObject GetCurrentData(string appname);

    public JObject GetData(string appname, string appversion);

}

public static class DataManagerUtil
{
    public static JObject getCurrentData(string appFolderPath, string appname, string filename)
    {
        string currentVersionFolderName = "";
        string appVersionFilePath = "";
        
        var currentJsonConfigFilePath = Path.Combine(appFolderPath, "current.json");
        if (File.Exists(currentJsonConfigFilePath))
        {
            string currentFileContent = File.ReadAllText(currentJsonConfigFilePath);
            var obj = JObject.Parse(currentFileContent);
            if (obj.ContainsKey("current_version"))
            {
                currentVersionFolderName = obj["current_version"].ToString();
            }
        }

        var currentVersionFolderPath = Path.Combine(appFolderPath, currentVersionFolderName);
        
        if (!Directory.Exists(currentVersionFolderPath))
        {
            var versionFolders = Directory.GetDirectories(appFolderPath)
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
                return new JObject();
            }
        }

        appVersionFilePath = Path.Combine(currentVersionFolderPath, filename);
        if (!File.Exists(appVersionFilePath))
        {
            return new JObject();
        }

        string fileContent = File.ReadAllText(appVersionFilePath);
        return JObject.Parse(fileContent);
    }
    
    public static JObject getData(string appFolderPath, string appname, string appversion, string filename)
    {
        string currentVersionFolderPath = "";
        string appVersionFilePath = "";
        var versionFolders = Directory.GetDirectories(appFolderPath)
            .OrderByDescending(f => new FileInfo(f).Name).ToArray();

        bool bGetOneVersion = false;

        foreach (var versionFolder in versionFolders)
        {
            var a = Path.GetFileName(versionFolder);
            if (appversion == a)
            {
                appVersionFilePath = Path.Combine(versionFolder, filename);
                bGetOneVersion = true;
                break;
            }
        }

        if (!bGetOneVersion)
        {
            return new JObject();
        }

        if (!File.Exists(appVersionFilePath))
        {
            return new JObject();
        }

        string fileContent = File.ReadAllText(appVersionFilePath);
        return JObject.Parse(fileContent);
    }
}