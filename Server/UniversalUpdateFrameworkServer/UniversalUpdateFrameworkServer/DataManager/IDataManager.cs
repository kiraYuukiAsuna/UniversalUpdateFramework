using Newtonsoft.Json.Linq;

namespace UniversalUpdateFrameworkServer.DataManager;

public interface IDataManager
{
    public JObject GetCurrentData(string appname, string channel, string platform);

    public JObject GetData(string appname, string appversion, string channel, string platform);
}

public static class DataManagerUtil
{
    public static List<string> getAllVersion(string appFolderPath, string appname, string channel, string platform)
    {
        var versionconfigJsonFilePath = Path.Combine(appFolderPath, channel, platform,
            "versionconfig.json");
        List<string>? versions = new List<string>();
        if (File.Exists(versionconfigJsonFilePath))
        {
            string currentFileContent = File.ReadAllText(versionconfigJsonFilePath);
            var obj = JObject.Parse(currentFileContent);
            if (obj.ContainsKey("versions"))
            {
                versions = ((JArray) obj["versions"]!).ToObject<List<string>>();
            }
        }

        return versions;
    }

    public static JObject getCurrentData(string appFolderPath, string appname, string channel, string platform,
        string filename)
    {
        string currentVersionFolderName = "";
        string appVersionFilePath = "";

        var appVersionsPath = Path.Combine(appFolderPath, channel, platform);

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

    public static JObject getData(string appFolderPath, string appname, string appversion, string channel,
        string platform, string filename)
    {
        string currentVersionFolderPath = "";
        string appVersionFilePath = "";
        var versionFolders = Directory
            .GetDirectories(Path.Combine(appFolderPath, channel, platform))
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