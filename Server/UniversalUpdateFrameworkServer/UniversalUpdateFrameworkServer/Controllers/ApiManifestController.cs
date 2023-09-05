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
public class AppManifestController : ControllerBase
{
    private readonly ILogger<AppManifestController> _logger;

    private AppManifestDataManager m_AppManifestDataManager;

    public AppManifestController(ILogger<AppManifestController> logger)
    {
        _logger = logger;

        m_AppManifestDataManager = new AppManifestDataManager("appdata");
    }

    [HttpGet("GetCurrentAppManifest")]
    public JsonResult GetCurrentAppManifest(string appname)
    {
        try
        {
            var jObject = m_AppManifestDataManager.GetCurrentData(appname);
            string jsonString = jObject.ToString();
            JsonDocument jsonDocument = JsonDocument.Parse(jsonString);
            JsonElement rootElement = jsonDocument.RootElement;

            return new JsonResult(rootElement);
        }
        catch(Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }
    
    [HttpGet("GetAppManifest")]
    public JsonResult GetAppManifest(string appname, string appversion)
    {
        try{
            var jObject = m_AppManifestDataManager.GetData(appname, appversion);
            string jsonString = jObject.ToString();
            JsonDocument jsonDocument = JsonDocument.Parse(jsonString);
            JsonElement rootElement = jsonDocument.RootElement;

            return new JsonResult(rootElement);
        }
        catch(Exception ex)
        {
            return new JsonResult(ex.ToString());
        }
    }
}