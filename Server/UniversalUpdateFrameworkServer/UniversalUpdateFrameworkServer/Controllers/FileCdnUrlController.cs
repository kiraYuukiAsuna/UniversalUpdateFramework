using System.Text.Json;
using Microsoft.AspNetCore.Mvc;
using UniversalUpdateFrameworkServer.DataManager;

namespace UniversalUpdateFrameworkServer.Controllers;

[ApiController]
[Route("api/v1")]
public class FileCdnUrlController : ControllerBase
{
    private readonly ILogger<FileCdnUrlController> _logger;

    private FileCdnUrlDataManager m_FileCdnUrlDataManager;
    
    public FileCdnUrlController(ILogger<FileCdnUrlController> logger)
    {
        _logger = logger;
    }    
    
    [HttpGet("GetCurrentFileCdnUrlList")]
    public async Task<IActionResult> GetCurrentFileCdnUrlList(string appname, string appversion, string channel,
        string platform)
    {
        try
        {
            var jObject = m_FileCdnUrlDataManager.GetCurrentData(appname, channel, platform);
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
    
    [HttpGet("GetFileCdnUrlList")]
    public async Task<IActionResult> GetFileCdnUrlList(string appname, string appversion, string channel,
        string platform)
    {
        try
        {
            var jObject = m_FileCdnUrlDataManager.GetData(appname, appversion, channel, platform);
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