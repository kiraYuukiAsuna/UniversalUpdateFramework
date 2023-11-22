#pragma once

#include <string>
#include <iostream>
#include "Network/ApiRequest.h"
#include "json.hpp"
#include "util.hpp"
#include "UpdateCore/AppVersion.h"
#include "UpdateCore/AppManifest.h"
#include "UpdateCore/FullPackageManifest.h"
#include "UpdateCore/DifferencePackageManifest.h"

class DifferencePackageUpdate {
public:
    DifferencePackageUpdate(std::string host, std::string appName, std::string appPath)
            : m_Host(host), m_AppName(appName), m_AppPath(appPath), m_ApiRequest(host, appName) {

    }

    ReturnWrapper execute(){
        auto [result1, appVersionContent] = m_ApiRequest.GetCurrentAppVersion();
        if(!result1.getStatus()){
            return result1;
        }

        auto appVersion = AppVersion(nlohmann::json::parse(appVersionContent));


        auto [result2, appManifestContent] = m_ApiRequest.GetCurrentAppManifest();
        if(!result2.getStatus()){
            return result2;
        }
        auto appManifest = AppManifest(nlohmann::json::parse(appManifestContent));


        auto [result3, appFullPackageManifestContent] = m_ApiRequest.GetCurrentAppFullPackageManifest();
        if(!result3.getStatus()){
            return result3;
        }
        auto appFullPackageManifest = FullPackageManifest(nlohmann::json::parse(appFullPackageManifestContent));

        auto [result4, appDifferencePackageManifestContent] = m_ApiRequest.GetAppDifferencePackageManifest("1.1.0");
        if(!result4.getStatus()){
            return result4;
        }
        auto appDifferencePackageManifest = DifferencePackageManifest(nlohmann::json::parse(appDifferencePackageManifestContent));


        std::filesystem::path downloadRootPath = "Download/" + m_AppPath + "/" + m_AppName;
        if(std::filesystem::exists(downloadRootPath)){
            std::filesystem::remove_all(downloadRootPath);
        }
        std::filesystem::create_directories(downloadRootPath);

        std::filesystem::path appVersionPath = downloadRootPath / ("appversion_" + appVersion.getVersion().getVersionString() + ".json");
        std::filesystem::path appManifestPath = downloadRootPath / ("appmanifest_" + appManifest.getAppVersion().getVersion().getVersionString()+ ".json");
        std::filesystem::path appFullPackageManifestPath = downloadRootPath / ("appfullpackagemanifest_" + appFullPackageManifest.getAppVersion().getVersion().getVersionString()+ ".json");
        std::filesystem::path appDifferencePackageManifestPath = downloadRootPath / ("appdifferencepackagemanifest_" + appDifferencePackageManifest.getNewAppVersion().getVersion().getVersionString()+ ".json");

        util::saveToFile(appVersionContent, appVersionPath);
        util::saveToFile(appManifestContent, appManifestPath);
        util::saveToFile(appFullPackageManifestContent, appFullPackageManifestPath);
        util::saveToFile(appDifferencePackageManifestContent, appDifferencePackageManifestPath);


        std::filesystem::path fullPackageFile = downloadRootPath / ("fullpackage_" + appVersion.getVersion().getVersionString());
        m_ApiRequest.DownloadCurrentFullPackage(fullPackageFile.string());

        std::string shellCommand1 = std::format(R"(hpatchz.exe "" {} {})", fullPackageFile.string(), (downloadRootPath / ("fullpackage_" + appVersion.getVersion().getVersionString() + "_uncompressed")).string());
        std::cout<<shellCommand1<<"\n";

        system(shellCommand1.c_str());

        std::filesystem::path differencePackageFile = downloadRootPath / ("differencepackage" + appVersion.getVersion().getVersionString());
        m_ApiRequest.DownloadCurrentDifferencePackage(differencePackageFile.string());

        std::string shellCommand2 = std::format(R"(hpatchz.exe "" {} {})", differencePackageFile.string(), (downloadRootPath / ("differencepackage_" + appVersion.getVersion().getVersionString() + "_uncompressed")).string());
        std::cout<<shellCommand2<<"\n";

        system(shellCommand2.c_str());

        return {true};

    }

private:
    ApiRequest m_ApiRequest;
    std::string m_Host;
    std::string m_AppName;
    std::string m_AppPath;

};

