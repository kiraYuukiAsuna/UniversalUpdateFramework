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
public class UpdaterUpdateSelfController : ControllerBase
{
    private readonly ILogger<UpdaterUpdateSelfController> _logger;

    private UpdaterUpdateSelfDataManager m_UpdaterUpdateSelfDataManager;

    public UpdaterUpdateSelfController(ILogger<UpdaterUpdateSelfController> logger)
    {
        _logger = logger;

        m_UpdaterUpdateSelfDataManager = new UpdaterUpdateSelfDataManager("updaterdata");
    }

    [HttpGet("GetCurrentUpdaterVersion")]
    public JsonResult GetCurrentUpdaterVersion(string updaterName, string channel, string platform)
    {
        try
        {
            var currentVersionString = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);
            JsonDocument jsonDocument = JsonDocument.Parse("{\"version\":\"" + currentVersionString + "\"}");
            JsonElement rootElement = jsonDocument.RootElement;

            return new JsonResult(rootElement);
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpGet("GetUpdaterVersionConfig")]
    public JsonResult GetUpdaterVersionConfig(string updaterName, string channel, string platform)
    {
        try
        {
            var versionconfigFileContent =
                m_UpdaterUpdateSelfDataManager.GetUpdaterVersionConfig(updaterName, channel, platform);

            JsonDocument jsonDocument = JsonDocument.Parse(versionconfigFileContent);
            JsonElement rootElement = jsonDocument.RootElement;

            return new JsonResult(rootElement);
        }
        catch (Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }

    [HttpGet("DownloadCurrentUpdaterInstaller")]
    public async Task<IActionResult> DownloadCurrentUpdaterInstaller(string updaterName, string channel,
        string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);

            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

            var fileFullPath = downloadFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            // 获取文件的大小
            var fileInfo = new FileInfo(downloadFileInfo.filePath);
            var fileLength = fileInfo.Length;

            // 读取请求中的Range头
            var rangeHeader = Request.GetTypedHeaders().Range;
            if (rangeHeader == null || rangeHeader.Ranges.Count != 1)
                // 如果没有Range头或者Range头不合法，返回整个文件
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);

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
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);
            }

            // 长度需要进行计算
            var contentLength = end - start + 1;

            // 设置状态码为206 Partial Content
            Response.StatusCode = (int)HttpStatusCode.PartialContent;

            // 设置Content Range
            Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileLength}");

            // 设置Content Length
            Response.Headers.Add("Content-Length", contentLength.ToString());

            // 创建文件流
            var stream = new FileStream(downloadFileInfo.filePath, FileMode.Open, FileAccess.Read);

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

    [HttpHead("DownloadCurrentUpdaterInstaller")]
    public async Task<IActionResult> DownloadCurrentUpdaterInstallerHead(string updaterName, string channel,
        string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);

            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

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

    [HttpGet("DownloadUpdaterInstaller")]
    public async Task<IActionResult> DownloadUpdaterInstaller(string updaterName, string updaterVersion, string channel,
        string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

            var fileFullPath = downloadFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            // 获取文件的大小
            var fileInfo = new FileInfo(downloadFileInfo.filePath);
            var fileLength = fileInfo.Length;

            // 读取请求中的Range头
            var rangeHeader = Request.GetTypedHeaders().Range;
            if (rangeHeader == null || rangeHeader.Ranges.Count != 1)
                // 如果没有Range头或者Range头不合法，返回整个文件
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);

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
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);
            }

            // 长度需要进行计算
            var contentLength = end - start + 1;

            // 设置状态码为206 Partial Content
            Response.StatusCode = (int)HttpStatusCode.PartialContent;

            // 设置Content Range
            Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileLength}");

            // 设置Content Length
            Response.Headers.Add("Content-Length", contentLength.ToString());

            // 创建文件流
            var stream = new FileStream(downloadFileInfo.filePath, FileMode.Open, FileAccess.Read);

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

    [HttpHead("DownloadUpdaterInstaller")]
    public async Task<IActionResult> DownloadUpdaterInstallerHead(string updaterName, string updaterVersion,
        string channel, string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

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

    [HttpGet("DownloadCurrentUpdaterZipFile")]
    public async Task<IActionResult> DownloadCurrentUpdaterZipFile(string updaterName, string channel, string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);

            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

            var fileFullPath = downloadFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            // 获取文件的大小
            var fileInfo = new FileInfo(downloadFileInfo.filePath);
            var fileLength = fileInfo.Length;

            // 读取请求中的Range头
            var rangeHeader = Request.GetTypedHeaders().Range;
            if (rangeHeader == null || rangeHeader.Ranges.Count != 1)
                // 如果没有Range头或者Range头不合法，返回整个文件
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);

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
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);
            }

            // 长度需要进行计算
            var contentLength = end - start + 1;

            // 设置状态码为206 Partial Content
            Response.StatusCode = (int)HttpStatusCode.PartialContent;

            // 设置Content Range
            Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileLength}");

            // 设置Content Length
            Response.Headers.Add("Content-Length", contentLength.ToString());

            // 创建文件流
            var stream = new FileStream(downloadFileInfo.filePath, FileMode.Open, FileAccess.Read);

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

    [HttpHead("DownloadCurrentUpdaterZipFile")]
    public async Task<IActionResult> DownloadCurrentUpdaterZipFileHead(string updaterName, string channel,
        string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);

            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

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

    [HttpGet("DownloadUpdaterZipFile")]
    public async Task<IActionResult> DownloadUpdaterZipFile(string updaterName, string updaterVersion, string channel,
        string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

            var fileFullPath = downloadFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            // 获取文件的大小
            var fileInfo = new FileInfo(downloadFileInfo.filePath);
            var fileLength = fileInfo.Length;

            // 读取请求中的Range头
            var rangeHeader = Request.GetTypedHeaders().Range;
            if (rangeHeader == null || rangeHeader.Ranges.Count != 1)
                // 如果没有Range头或者Range头不合法，返回整个文件
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);

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
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);
            }

            // 长度需要进行计算
            var contentLength = end - start + 1;

            // 设置状态码为206 Partial Content
            Response.StatusCode = (int)HttpStatusCode.PartialContent;

            // 设置Content Range
            Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileLength}");

            // 设置Content Length
            Response.Headers.Add("Content-Length", contentLength.ToString());

            // 创建文件流
            var stream = new FileStream(downloadFileInfo.filePath, FileMode.Open, FileAccess.Read);

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

    [HttpHead("DownloadUpdaterZipFile")]
    public async Task<IActionResult> DownloadUpdaterZipFileHead(string updaterName, string updaterVersion,
        string channel, string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName),
                channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

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

    [HttpGet("DownloadUpdaterFile")]
    public async Task<IActionResult> DownloadUpdaterFile(string updaterName, string updaterVersion, string channel,
        string platform, string md5)
    {
        try
        {
            var downloadFileInfo =
                m_UpdaterUpdateSelfDataManager.GetFile(updaterName, updaterVersion, channel, platform, md5);

            var fileFullPath = downloadFileInfo.filePath;

            if (!System.IO.File.Exists(fileFullPath))
            {
                return new EmptyResult();
            }

            // 获取文件的大小
            var fileInfo = new FileInfo(downloadFileInfo.filePath);
            var fileLength = fileInfo.Length;

            // 读取请求中的Range头
            var rangeHeader = Request.GetTypedHeaders().Range;
            if (rangeHeader == null || rangeHeader.Ranges.Count != 1)
                // 如果没有Range头或者Range头不合法，返回整个文件
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);

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
                return PhysicalFile(downloadFileInfo.filePath, "application/octet-stream", true);
            }

            // 长度需要进行计算
            var contentLength = end - start + 1;

            // 设置状态码为206 Partial Content
            Response.StatusCode = (int)HttpStatusCode.PartialContent;

            // 设置Content Range
            Response.Headers.Add("Content-Range", $"bytes {start}-{end}/{fileLength}");

            // 设置Content Length
            Response.Headers.Add("Content-Length", contentLength.ToString());

            // 创建文件流
            var stream = new FileStream(downloadFileInfo.filePath, FileMode.Open, FileAccess.Read);

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

    [HttpHead("DownloadUpdaterFile")]
    public async Task<IActionResult> DownloadUpdaterFileHead(string updaterName, string updaterVersion, string channel,
        string platform, string md5)
    {
        try
        {
            var downloadFileInfo =
                m_UpdaterUpdateSelfDataManager.GetFile(updaterName, updaterVersion, channel, platform, md5);

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