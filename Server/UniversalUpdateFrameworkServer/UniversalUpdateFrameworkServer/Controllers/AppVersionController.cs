﻿using System.Net;
using System.Runtime.InteropServices.JavaScript;
using System.Text.Json;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using UniversalUpdateFrameworkServer.DataManager;

namespace UniversalUpdateFrameworkServer.Controllers;

[ApiController]
[Route("api/v1")]
public class AppVersionController : ControllerBase
{
    private readonly ILogger<AppVersionController> _logger;

    private AppVersionDataManager m_AppVersionDataManager;

    public AppVersionController(ILogger<AppVersionController> logger)
    {
        _logger = logger;

        m_AppVersionDataManager = new AppVersionDataManager("appdata");
    }

    [HttpGet("GetCurrentAppVersion")]
    public JsonResult GetCurrentAppVersion(string appname, string channel, string platform)
    {
        try
        {
            var jObject = m_AppVersionDataManager.GetCurrentData(appname, channel, platform);
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

    [HttpGet("GetAppVersion")]
    public JsonResult GetAppVersion(string appname, string appversion, string channel, string platform)
    {
        try
        {
            var jObject = m_AppVersionDataManager.GetData(appname, appversion, channel, platform);
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

    [HttpGet("GetAppVersionList")]
    public JsonResult GetAppVersionList(string appname, string channel, string platform)
    {
        try
        {
            var appVersionList = m_AppVersionDataManager.GetAppVersionList(appname, channel, platform);

            string jObject = JsonConvert.SerializeObject(appVersionList);
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