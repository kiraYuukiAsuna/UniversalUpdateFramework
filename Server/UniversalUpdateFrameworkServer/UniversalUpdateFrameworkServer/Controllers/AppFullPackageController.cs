using System.Net;
using System.Runtime.InteropServices.JavaScript;
using System.Text.Json;
using System.Text.RegularExpressions;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using UniversalUpdateFrameworkServer.DataManager;

namespace UniversalUpdateFrameworkServer.Controllers;

[ApiController]
[Route("api/v1")]
public class AppFullPackageController : ControllerBase
{
    private readonly ILogger<AppFullPackageController> _logger;

    private AppFullPackageDataManager m_AppFullPackageDataManager;

    public AppFullPackageController(ILogger<AppFullPackageController> logger)
    {
        _logger = logger;

        m_AppFullPackageDataManager = new AppFullPackageDataManager("appdata");
    }

    [HttpGet("GetCurrentAppFullPackage")]
    public JsonResult GetCurrentAppFullPackage(string appname, string channel, string platform)
    {
        try
        {
            var jObject = m_AppFullPackageDataManager.GetCurrentData(appname, channel, platform);
            string jsonString = jObject.ToString();
            JsonDocument jsonDocument = JsonDocument.Parse(jsonString);
            JsonElement rootElement = jsonDocument.RootElement;

            return new JsonResult(rootElement);
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpGet("GetAppFullPackage")]
    public JsonResult GetAppFullPackage(string appname, string appversion, string channel, string platform)
    {
        try
        {
            var jObject = m_AppFullPackageDataManager.GetData(appname, appversion, channel, platform);
            string jsonString = jObject.ToString();
            JsonDocument jsonDocument = JsonDocument.Parse(jsonString);
            JsonElement rootElement = jsonDocument.RootElement;

            return new JsonResult(rootElement);
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpGet("DownloadCurrentFullPackage")]
    public async Task<IActionResult> DownloadCurrentFullPackage(string appname, string channel, string platform)
    {
        try
        {
            var fullPackageFileInfo =
                m_AppFullPackageDataManager.GetCurrentFullPackageFilePath(appname, channel, platform);
            var fileFullPath = fullPackageFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            var fileInfo = new FileInfo(fileFullPath);
            var contentType = "application/octet-stream";
            var fileName = fileInfo.Name;
            var fileStream = new FileStream(fileFullPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            var contentLength = fileStream.Length;
            var range = Request.Headers["Range"].ToString();
            if (!string.IsNullOrEmpty(range))
            {
                var match = Regex.Match(range, @"bytes=(\d*)-(\d*)");
                var start = long.Parse(match.Groups[1].Value);
                var end = string.IsNullOrEmpty(match.Groups[2].Value)
                    ? contentLength - 1
                    : long.Parse(match.Groups[2].Value);
                contentLength = end - start + 1;
                fileStream.Seek(start, SeekOrigin.Begin);
                Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileInfo.Length}");
                Response.StatusCode = (int)HttpStatusCode.PartialContent;
            }

            Response.Headers.Add("Accept-Ranges", "bytes");
            Response.ContentType = contentType;
            Response.Headers.Add("Content-Disposition", $"attachment; filename=\"{fileName}\"");
            Response.Headers.Add("Content-Length", contentLength.ToString());
            await fileStream.CopyToAsync(Response.Body);
            return new EmptyResult();
        }catch(Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpGet("DownloadFullPackage")]
    public async Task<IActionResult> DownloadFullPackage(string appname, string appversion, string channel,
        string platform)
    {
        try
        {
            var fullPackageFileInfo =
                m_AppFullPackageDataManager.GetFullPackageFilePath(appname, appversion, channel, platform);
            var fileFullPath = fullPackageFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            var fileInfo = new FileInfo(fileFullPath);
            var contentType = "application/octet-stream";
            var fileName = fileInfo.Name;
            var fileStream = new FileStream(fileFullPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            var contentLength = fileStream.Length;
            var range = Request.Headers["Range"].ToString();
            if (!string.IsNullOrEmpty(range))
            {
                var match = Regex.Match(range, @"bytes=(\d*)-(\d*)");
                var start = long.Parse(match.Groups[1].Value);
                var end = string.IsNullOrEmpty(match.Groups[2].Value)
                    ? contentLength - 1
                    : long.Parse(match.Groups[2].Value);
                contentLength = end - start + 1;
                fileStream.Seek(start, SeekOrigin.Begin);
                Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileInfo.Length}");
                Response.StatusCode = (int)HttpStatusCode.PartialContent;
            }

            Response.Headers.Add("Accept-Ranges", "bytes");
            Response.ContentType = contentType;
            Response.Headers.Add("Content-Disposition", $"attachment; filename=\"{fileName}\"");
            Response.Headers.Add("Content-Length", contentLength.ToString());
            await fileStream.CopyToAsync(Response.Body);
            return new EmptyResult();
        }catch(Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpGet("DownloadFileFromFullPackage")]
    public async Task<IActionResult> DownloadFileFromFullPackage(string appname, string appversion, string channel,
        string platform, string md5)
    {
        try
        {
            var downloadFileInfo =
                m_AppFullPackageDataManager.GetFileFromFullPackageFilePath(appname, appversion, channel, platform, md5);

            if (downloadFileInfo.filePath == "")
            {
                return new EmptyResult();
            }

            var fileFullPath = downloadFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            var fileInfo = new FileInfo(fileFullPath);
            var contentType = "application/octet-stream";
            var fileName = fileInfo.Name;
            var fileStream = new FileStream(fileFullPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            var contentLength = fileStream.Length;
            var range = Request.Headers["Range"].ToString();
            if (!string.IsNullOrEmpty(range))
            {
                var match = Regex.Match(range, @"bytes=(\d*)-(\d*)");
                var start = long.Parse(match.Groups[1].Value);
                var end = string.IsNullOrEmpty(match.Groups[2].Value)
                    ? contentLength - 1
                    : long.Parse(match.Groups[2].Value);
                contentLength = end - start + 1;
                fileStream.Seek(start, SeekOrigin.Begin);
                Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileInfo.Length}");
                Response.StatusCode = (int)HttpStatusCode.PartialContent;
            }

            Response.Headers.Add("Accept-Ranges", "bytes");
            Response.ContentType = contentType;
            Response.Headers.Add("Content-Disposition", $"attachment; filename=\"{fileName}\"");
            Response.Headers.Add("Content-Length", contentLength.ToString());
            await fileStream.CopyToAsync(Response.Body);
            return new EmptyResult();
        }catch(Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }
}