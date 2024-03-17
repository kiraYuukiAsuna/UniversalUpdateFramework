#pragma once

#include <string>
#include <iostream>
#include <utility>
#include "Network/ApiRequest.hpp"
#include "util.hpp"
#include "UpdateCore/AppVersion.hpp"
#include "UpdateCore/AppManifest.hpp"
#include "UpdateCore/FullPackageManifest.hpp"
#include "VerifyAndRePatch.hpp"

class FullPackageUpdate {
public:
    FullPackageUpdate(const std::string&host, const std::string&appName, std::string channel, std::string platform,
                      std::string appPath,
                      std::string downloadPath, std::string newVersion)
        : m_ApiRequest(host, appName, channel, platform), m_Host(host), m_AppName(appName), m_Channel(channel),
          m_Platform(platform), m_AppPath(std::move(appPath)),
          m_DownloadPath(std::move(downloadPath)), m_NewVersion(std::move(newVersion)) {
    }

    ReturnWrapper execute() {
        auto [result1, appVersionContent] = m_NewVersion.empty()
                                                ? m_ApiRequest.GetCurrentAppVersion()
                                                : m_ApiRequest.GetAppVersion(m_NewVersion);
        if (!result1.getStatus()) {
            return result1;
        }
        auto appVersion = AppVersion(nlohmann::json::parse(appVersionContent));

        auto [result2, appManifestContent] = m_NewVersion.empty()
                                                 ? m_ApiRequest.GetCurrentAppManifest()
                                                 : m_ApiRequest.GetAppManifest(m_NewVersion);
        if (!result2.getStatus()) {
            return result2;
        }
        auto appManifest = AppManifest(nlohmann::json::parse(appManifestContent));

        auto [result3, appFullPackageManifestContent] = m_NewVersion.empty()
                                                            ? m_ApiRequest.GetCurrentAppFullPackageManifest()
                                                            : m_ApiRequest.GetAppFullPackageManifest(m_NewVersion);
        if (!result3.getStatus()) {
            return result3;
        }
        auto appFullPackageManifest = FullPackageManifest(nlohmann::json::parse(appFullPackageManifestContent));

        std::filesystem::path downloadRootPath = m_DownloadPath + "/" + m_AppPath + "/" + m_AppName;
        if (std::filesystem::exists(downloadRootPath)) {
            std::filesystem::remove_all(downloadRootPath);
        }
        std::filesystem::create_directories(downloadRootPath);

        std::filesystem::path appVersionPath =
                downloadRootPath / ("appversion_" + appVersion.getVersion().getVersionString() + ".json");
        std::filesystem::path appManifestPath =
                downloadRootPath / ("appmanifest_" + appManifest.getAppVersion().getVersion().getVersionString() +
                                    ".json");
        std::filesystem::path appFullPackageManifestPath =
                downloadRootPath / ("appfullpackagemanifest_" + appFullPackageManifest.getAppVersion().getVersion().
                                    getVersionString() + ".json");

        util::saveToFile(appVersionContent, appVersionPath);
        util::saveToFile(appManifestContent, appManifestPath);
        util::saveToFile(appFullPackageManifestContent, appFullPackageManifestPath);

        std::filesystem::path fullPackageFile =
                downloadRootPath / ("fullpackage_" + appVersion.getVersion().getVersionString());
        m_NewVersion.empty()
            ? m_ApiRequest.DownloadCurrentFullPackage(fullPackageFile.string())
            : m_ApiRequest.DownloadFullPackage(m_NewVersion, fullPackageFile.string());

        auto uncompresssedPath =
                downloadRootPath / ("fullpackage_" + appVersion.getVersion().getVersionString() + "_uncompressed");
        std::string shellCommand = std::format(R"({} "" "{}" "{}")", hpatchzExecuable, fullPackageFile.string(),
                                               uncompresssedPath.string());
        std::cout << shellCommand << "\n";
        system(shellCommand.c_str());

        std::error_code ec;
        std::filesystem::remove_all(m_AppPath, ec);
        if (ec) {
            return {
                false, ErrorCode::RemoveOldVersionDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::RemoveOldVersionDirFailed))
            };
        }

        std::filesystem::create_directories(m_AppPath, ec);
        if (ec) {
            return {
                false, ErrorCode::CreateAppDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed))
            };
        }

        std::filesystem::copy(uncompresssedPath, m_AppPath, std::filesystem::copy_options::recursive |
                                                            std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            return {
                false, ErrorCode::CopyNewVersionDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::CopyNewVersionDirFailed))
            };
        } {
            auto result = VerifyAndRePatch::execute(m_ApiRequest, appVersion, appManifest, m_AppPath);
            if (!result.getStatus()) {
                return result;
            }
        }
        std::filesystem::remove_all(m_DownloadPath, ec);
        if (ec) {
            return {
                false, ErrorCode::DeleteDownloadDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::DeleteDownloadDirFailed))
            };
        }

        return {true};
    }

private:
    ApiRequest m_ApiRequest;
    std::string m_Host;
    std::string m_AppName;
    std::string m_Channel;
    std::string m_Platform;
    std::string m_AppPath;
    std::string m_DownloadPath;
    std::string m_NewVersion;
};
