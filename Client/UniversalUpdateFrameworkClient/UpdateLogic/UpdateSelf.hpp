#pragma once

#include <utility>

#include "Network/ApiRequest.hpp"
#include <VersionConfig.hpp>

/* Updater_Vx.x.x
 * fullpackage
 * Updater.exe
 * AppSpecification.json
 * AppUpdateConfig.json
 * OtherFiles...
 * Updater_Vx.x.x_zip.zip
 * Updater_Vx.x.x_installer.exe
 * ---- versionconfig.json
 *  {
 *      "current_version": "x.x.x",
 *      "versions": [
 *          {
 *              "version": "x.x.x",
 *              "filemanifests": [
 *                   {
 *                      "FilePath": ".";
 *                      "FileName": "Updater.exe",
 *                      "Md5": "xxxxxx",
 *                      "Permission": 511
 *                   }
 *              ]
 *          }
 *
 *      ]
 *  }
 *
 */

class UpdateSelf {
public:
    UpdateSelf(std::string updateIntermidiatePath, std::string updaterFinalPath)
        : m_UpdateIntermidiatePath(std::move(updateIntermidiatePath)), m_UpdaterFinalPath(std::move(updaterFinalPath)){

    }

    // ReturnWrapper execute() {
    //     auto apiRequest = ApiRequest();
    // }

private:
    std::string m_UpdateIntermidiatePath;
    std::string m_UpdaterFinalPath;
};
