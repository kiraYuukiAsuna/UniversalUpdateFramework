using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Web;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;
using UniversalUpdateFrameworkServer.DataManager;

namespace UniversalUpdateFrameworkServer.Controllers;

[ApiController]
[Route("api/v1")]
public class AuthenticationController : ControllerBase
{
    private readonly ILogger<AppVersionController> _logger;

    public AuthenticationController(ILogger<AppVersionController> logger)
    {
        _logger = logger;
    }

    [HttpGet("GetSignedDirectLink")]
    public JsonResult GetSignedDirectLink(string originUrl /*待签名URL，即用户在123云盘直链空间目录下复制的直链url*/)
    {
        try
        {
            string privateKey = "BVxB5aBpPa5ISeXf"; // 鉴权密钥，即用户在123云盘直链管理中设置的鉴权密钥
            ulong uid = 1838918272; // 账号id，即用户在123云盘个人中心页面所看到的账号id
            TimeSpan validDuration = TimeSpan.FromDays(1); // 链接签名有效期

            try
            {
                string newUrl = SignURL(originUrl, privateKey, uid, validDuration);
                Console.WriteLine(newUrl);
                var response = new
                {
                    code = 0,
                    message = "Success",
                    signedUrl = newUrl
                };

                return new JsonResult(response);
            }
            catch (Exception ex)
            {
                return new JsonResult(ex.ToString());
            }
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    static string SignURL(string originUrl, string privateKey, ulong uid, TimeSpan validDuration)
    {
        long ts = DateTimeOffset.UtcNow.Add(validDuration).ToUnixTimeSeconds(); // 有效时间戳
        int rInt = new Random().Next(); // 随机正整数

        Uri objURL = new Uri(originUrl);
        string path = objURL.AbsolutePath;
        string toBeHashed = $"{path}-{ts}-{rInt}-{uid}-{privateKey}";

        using (MD5 md5 = MD5.Create())
        {
            byte[] hashBytes = md5.ComputeHash(Encoding.UTF8.GetBytes(toBeHashed));
            string hash = BitConverter.ToString(hashBytes).Replace("-", "").ToLower();

            string authKey = $"{ts}-{rInt}-{uid}-{hash}";

            var query = HttpUtility.ParseQueryString(objURL.Query);
            query["auth_key"] = authKey;

            UriBuilder uriBuilder = new UriBuilder(objURL)
            {
                Query = query.ToString()
            };

            return uriBuilder.ToString();
        }
    }
}