using System.Net;
using System.Runtime.InteropServices.JavaScript;
using System.Text.Json;
using System.Text.RegularExpressions;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Net.Http.Headers;
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
            return new FileStreamResult(fileStream, contentType);
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpHead("DownloadCurrentFullPackage")]
    public async Task<IActionResult> DownloadCurrentFullPackageHead(string appname, string channel, string platform)
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
            Response.Headers.Add("Content-Length", contentLength.ToString());
            return Ok();
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpGet("DownloadFullPackage")]
    public async Task<IActionResult> DownloadFullPackage(string appname, string appversion, string channel,
        string platform)
    {
        /*try
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
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }*/
        try
        {
            var fullPackageFileInfo =
                m_AppFullPackageDataManager.GetFullPackageFilePath(appname, appversion, channel, platform);
            var fileFullPath = fullPackageFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }
            
            // 获取文件的大小
            var fileInfo = new FileInfo(fullPackageFileInfo.filePath);
            long fileLength = fileInfo.Length;

            // 读取请求中的Range头
            RangeHeaderValue rangeHeader = Request.GetTypedHeaders().Range;
            if (rangeHeader == null || rangeHeader.Ranges.Count != 1)
            {
                // 如果没有Range头或者Range头不合法，返回整个文件
                return PhysicalFile(fullPackageFileInfo.filePath, "application/octet-stream", true);
            }

            // 解析Range头，只处理单个范围的情况
            // 确保Ranges不为空并且至少有一个元素
            long start = 0;
            long end = 0;
            if (rangeHeader?.Ranges?.Count > 0)
            {
                var firstRange = rangeHeader.Ranges.ElementAt(0);
                start = firstRange.From ?? 0;
                end = firstRange.To ?? fileLength - 1;
            }
            else
            {
                // 没有有效的范围请求，处理默认行为
                // 如果没有Range头或者Range头不合法，返回整个文件
                return PhysicalFile(fullPackageFileInfo.filePath, "application/octet-stream", true);
            }
            
            // 长度需要进行计算
            long contentLength = end - start + 1;

            // 设置状态码为206 Partial Content
            Response.StatusCode = (int)System.Net.HttpStatusCode.PartialContent;

            // 设置Content Range
            Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileLength}");

            // 设置Content Length
            Response.Headers.Add("Content-Length", contentLength.ToString());

            // 创建文件流
            var stream = new FileStream(fullPackageFileInfo.filePath, FileMode.Open, FileAccess.Read);

            // 读取指定范围的数据
            stream.Seek(start, SeekOrigin.Begin);
            var buffer = new byte[contentLength];
            stream.Read(buffer, 0, buffer.Length);
            
            // 返回部分文件内容
            return File(buffer, "application/octet-stream", false);
        }
        catch (Exception ex)
        {
            // 如果发生错误，记录错误并返回服务器错误状态码
            Console.WriteLine(ex.Message);
            return StatusCode(500, "Internal server error");
        }
    }

    [HttpHead("DownloadFullPackage")]
    public async Task<IActionResult> DownloadFullPackageHead(string appname, string appversion, string channel,
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
            Response.Headers.Add("Content-Length", contentLength.ToString());
            return Ok();
        }
        catch (Exception ex)
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
            return new FileStreamResult(fileStream, contentType);
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpHead("DownloadFileFromFullPackage")]
    public async Task<IActionResult> DownloadFileFromFullPackageHead(string appname, string appversion, string channel,
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
            Response.Headers.Add("Content-Length", contentLength.ToString());
            return Ok();
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }
}