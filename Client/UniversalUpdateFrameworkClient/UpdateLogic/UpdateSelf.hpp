#pragma once

#include <TypeDefination/VersionConfig.hpp>
#include <utility>

#include "Network/ApiRequest.hpp"

/* Updater_Vx.x.x
 * fullpackage (folder)
 * AppSpecification.json
 * AppUpdateConfig.json
 * OtherFiles...
 * Updater_zip
 * Updater_installer
 * versionconfig.json
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
/* Updater_Vx.x.x
 * Updater.exe
 * AppSpecification.json
 * AppUpdateConfig.json
 * OtherFiles...
 * Updater_zip_Vx.x.x.zip
 * Updater_installer_Vx.x.x.exe
 */

class UpdateSelf {
public:
	UpdateSelf(std::string updateIntermidiatePath, std::string updaterFinalPath)
		: m_UpdateIntermidiatePath(std::move(updateIntermidiatePath)),
		  m_UpdaterFinalPath(std::move(updaterFinalPath)) {}

	// ReturnWrapper execute() {
	//     auto apiRequest = ApiRequest();
	// }

private:
	std::string m_UpdateIntermidiatePath;
	std::string m_UpdaterFinalPath;
};
