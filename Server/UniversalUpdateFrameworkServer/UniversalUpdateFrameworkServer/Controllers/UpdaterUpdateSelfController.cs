using System.Net;
using System.Runtime.InteropServices.JavaScript;
using System.Text.Json;
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

        m_UpdaterUpdateSelfDataManager = new UpdaterUpdateSelfDataManager("appdata");
    }

    [HttpGet("GetCurrentUpdaterVersion")]
    public JsonResult GetCurrentUpdaterVersion(string updaterName, string channel, string platform)
    {
        try
        {
            var currentVersionString = m_UpdaterUpdateSelfDataManager.GetCurrentData(updaterName, channel, platform);
            JsonDocument jsonDocument = JsonDocument.Parse(currentVersionString);
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
    public JsonResult DownloadCurrentUpdaterInstaller(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            var jObject = m_UpdaterUpdateSelfDataManager.GetData(updaterName, updaterVersion, channel, platform);
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
    
    [HttpGet("DownloadUpdaterInstaller")]
    public JsonResult DownloadUpdaterInstaller(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            var jObject = m_UpdaterUpdateSelfDataManager.GetData(updaterName, updaterVersion, channel, platform);
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
    
    [HttpGet("DownloadCurrentUpdaterZipFile")]
    public JsonResult DownloadCurrentUpdaterZipFile(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            var jObject = m_UpdaterUpdateSelfDataManager.GetData(updaterName, updaterVersion, channel, platform);
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
    
    [HttpGet("DownloadUpdaterZipFile")]
    public JsonResult DownloadUpdaterZipFile(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            var jObject = m_UpdaterUpdateSelfDataManager.GetData(updaterName, updaterVersion, channel, platform);
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
    
    [HttpGet("DownloadUpdaterFile")]
    public JsonResult DownloadUpdaterFile(string updaterName, string updaterVersion, string channel, string platform)
    {
        try
        {
            var jObject = m_UpdaterUpdateSelfDataManager.GetData(updaterName, updaterVersion, channel, platform);
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
    
}