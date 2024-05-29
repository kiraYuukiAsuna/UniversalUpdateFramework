namespace UniversalUpdateFrameworkServer.Definition;

public class FileManifestInfo
{
    public string FilePath { get; set; }
    public string FileName { get; set; }
    public string Md5 { get; set; }
    public ushort Permission { get; set; }
}
public class AppVersionBriefInfo
{
    public string Version { get; set; }
    public List<FileManifestInfo> FileManifests { get; set; }
}

public class AppVersionConfigInfo
{
    public string CurrentVersion { get; set; }
    public List<AppVersionBriefInfo> Versions { get; set; }
}
