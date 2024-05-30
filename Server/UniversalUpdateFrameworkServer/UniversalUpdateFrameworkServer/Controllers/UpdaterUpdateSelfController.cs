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
    public JsonResult GetUpdaterVersionConfig(string appname, string channel, string platform)
    {
        try
        {
            var versionconfigFileContent = m_UpdaterUpdateSelfDataManager.GetUpdaterVersionConfig(appname, channel, platform);

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
    public async Task<IActionResult> DownloadCurrentUpdaterInstaller(string updaterName, string channel, string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);

            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

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
                Response.StatusCode = (int) HttpStatusCode.PartialContent;
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
    
    [HttpHead("DownloadCurrentUpdaterInstaller")]
    public async Task<IActionResult> DownloadCurrentUpdaterInstallerHead(string updaterName, string channel, string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);

            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;

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
    
    [HttpGet("DownloadUpdaterInstaller")]
    public async Task<IActionResult> DownloadUpdaterInstaller(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;
            
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
                Response.StatusCode = (int) HttpStatusCode.PartialContent;
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
    
    [HttpHead("DownloadUpdaterInstaller")]
    public async Task<IActionResult> DownloadUpdaterInstallerHead(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_installer");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;
            
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
    
    [HttpGet("DownloadCurrentUpdaterZipFile")]
    public async Task<IActionResult> DownloadCurrentUpdaterZipFile(string updaterName, string channel, string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);
            
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;
            
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
                Response.StatusCode = (int) HttpStatusCode.PartialContent;
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
    
     [HttpHead("DownloadCurrentUpdaterZipFile")]
    public async Task<IActionResult> DownloadCurrentUpdaterZipFileHead(string updaterName, string channel, string platform)
    {
        try
        {
            var updaterVersion = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);
            
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;
            
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
    
    [HttpGet("DownloadUpdaterZipFile")]
    public async Task<IActionResult> DownloadUpdaterZipFile(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;
            
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
                Response.StatusCode = (int) HttpStatusCode.PartialContent;
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
    
    [HttpHead("DownloadUpdaterZipFile")]
    public async Task<IActionResult> DownloadUpdaterZipFileHead(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            DownloadFileInfo downloadFileInfo;
            downloadFileInfo.filePath = Path.Combine(m_UpdaterUpdateSelfDataManager.getUpdaterFolderPath(updaterName), channel, platform, updaterVersion, "updater_zip");
            downloadFileInfo.md5 = Md5Util.CalculateFileMD5(downloadFileInfo.filePath);
            downloadFileInfo.version = updaterVersion;
            
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
    
    [HttpGet("DownloadUpdaterFile")]
    public async Task<IActionResult> DownloadUpdaterFile(string updaterName, string updaterVersion, string channel, string platform, string md5)
    {
        try
        {
            var downloadFileInfo =
                m_UpdaterUpdateSelfDataManager.GetFile(updaterName, updaterVersion, channel, platform, md5);

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
                Response.StatusCode = (int) HttpStatusCode.PartialContent;
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
    
     [HttpHead("DownloadUpdaterFile")]
    public async Task<IActionResult> DownloadUpdaterFileHead(string updaterName, string updaterVersion, string channel, string platform, string md5)
    {
        try
        {
            var downloadFileInfo =
                m_UpdaterUpdateSelfDataManager.GetFile(updaterName, updaterVersion, channel, platform, md5);

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