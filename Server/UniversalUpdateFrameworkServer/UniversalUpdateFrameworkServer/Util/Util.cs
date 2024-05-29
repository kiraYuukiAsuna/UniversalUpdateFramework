using System.Security.Cryptography;
using System.IO;
using System.Text;


public static class Md5Util{
    public static string CalculateFileMD5(string filename)
    {
        using var md5 = MD5.Create();
        using var stream = File.OpenRead(filename);
        var hash = md5.ComputeHash(stream);
        return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
    }
}
