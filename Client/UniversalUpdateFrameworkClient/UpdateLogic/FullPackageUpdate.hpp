#pragma once

#include <string>
#include <iostream>
#include <utility>
#include "Network/ApiRequest.hpp"
#include "util.hpp"
#include <AppVersion.hpp>
#include <AppManifest.hpp>
#include <DeleteFileForce.hpp>
#include <FullPackageManifest.hpp>
#include <UpdateCore/UpdateStatus.hpp>

#include "VerifyRePatchSetPermission.hpp"

class FullPackageUpdate {
public:
    FullPackageUpdate(const std::string&host, const std::string&appName, std::string channel, std::string platform,
                      std::string appPath,
                      std::string downloadPath, std::string newVersion)
        : m_ApiRequest(host, appName, channel, platform), m_Host(host), m_AppName(appName), m_Channel(channel),
          m_Platform(platform), m_AppPath(std::move(appPath)),
          m_DownloadPath(std::move(downloadPath)), m_NewVersion(std::move(newVersion)) {
    }

    async_simple::coro::Lazy<ReturnWrapper> execute(std::function<void(UpdateStatusInfo)> updateStatusCallback) {
        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadAppVersionFile,
        });
        auto [result1, appVersionContent] = co_await (m_NewVersion.empty()
                                                          ? m_ApiRequest.GetCurrentAppVersion()
                                                          : m_ApiRequest.GetAppVersion(m_NewVersion));
        if (!result1.getStatus()) {
            co_return result1;
        }
        AppVersionInfo appVersion = nlohmann::json::parse(appVersionContent);

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadAppManifestFile,
        });
        auto [result2, appManifestContent] = co_await (m_NewVersion.empty()
                                                           ? m_ApiRequest.GetCurrentAppManifest()
                                                           : m_ApiRequest.GetAppManifest(m_NewVersion));
        if (!result2.getStatus()) {
            co_return result2;
        }
        AppManifestInfo appManifest = nlohmann::json::parse(appManifestContent);

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadFullPackageManifestFile,
        });
        auto [result3, appFullPackageManifestContent] = co_await (m_NewVersion.empty()
                                                                      ? m_ApiRequest.GetCurrentAppFullPackageManifest()
                                                                      : m_ApiRequest.GetAppFullPackageManifest(
                                                                          m_NewVersion));
        if (!result3.getStatus()) {
            co_return result3;
        }
        FullPackageManifestInfo appFullPackageManifest = nlohmann::json::parse(appFullPackageManifestContent);

        ProcessUtil::TerminateProcessByFilePath(m_AppPath);

        std::filesystem::path downloadRootPath = m_DownloadPath + "/" + m_AppPath + "/" + m_AppName;
        ProcessUtil::DeleteFileRecursive(downloadRootPath);
        std::filesystem::create_directories(downloadRootPath);

        std::filesystem::path appVersionPath =
                downloadRootPath / ("appversion_" + appVersion.AppVersion + ".json");
        std::filesystem::path appManifestPath =
                downloadRootPath / ("appmanifest_" + appManifest.AppVersion +
                                    ".json");
        std::filesystem::path appFullPackageManifestPath =
                downloadRootPath / ("appfullpackagemanifest_" + appFullPackageManifest.AppVersion + ".json");

        util::saveToFile(appVersionContent, appVersionPath);
        util::saveToFile(appManifestContent, appManifestPath);
        util::saveToFile(appFullPackageManifestContent, appFullPackageManifestPath);

        std::filesystem::path fullPackageFile =
                downloadRootPath / ("fullpackage_" + appVersion.AppVersion);

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadFullPackageFile,
        });
        auto [resultx, _] = co_await (m_NewVersion.empty()
                                          ? m_ApiRequest.DownloadCurrentFullPackage(fullPackageFile.string())
                                          : m_ApiRequest.DownloadFullPackage(m_NewVersion, fullPackageFile.string()));
        if (!resultx.getStatus()) {
            co_return resultx;
        }

        auto uncompresssedPath =
                downloadRootPath / ("fullpackage_" + appVersion.AppVersion + "_uncompressed");
        std::string shellCommand = std::format(R"({} "" "{}" "{}")", hpatchzExecuable, fullPackageFile.string(),
                                               uncompresssedPath.string());
        SEELE_INFO_TAG(__func__, "{}", shellCommand);
        system(shellCommand.c_str());

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::FullPackageUpdateInstalling,
        });

        if (!ProcessUtil::DeleteFileRecursive(m_AppPath)) {
            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::Failed,
                .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::RemoveOldVersionDirFailed)) + " , File:" +
                                m_AppPath
            });
            co_return ReturnWrapper{
                false, ErrorCode::RemoveOldVersionDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::RemoveOldVersionDirFailed)) + " , File:" +
                m_AppPath
            };
        }

        std::error_code ec;
        std::filesystem::create_directories(m_AppPath, ec);
        if (ec) {
            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::Failed,
                .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed)) + " , File:" +
                                m_AppPath
            });
            co_return ReturnWrapper{
                false, ErrorCode::CreateAppDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed))
            };
        }

        std::filesystem::copy(uncompresssedPath, m_AppPath, std::filesystem::copy_options::recursive |
                                                            std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::Failed,
                .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::CopyNewVersionDirFailed)) + " , From File:"
                                + uncompresssedPath.string() + " , To File:" + m_AppPath
            });
            co_return ReturnWrapper{
                false, ErrorCode::CopyNewVersionDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::CopyNewVersionDirFailed)) + " , From File:"
                + uncompresssedPath.string() + " , To File:" + m_AppPath
            };
        } {
            auto result = co_await
                    VerifyRePatchSetPermission::execute(m_ApiRequest, appVersion, appManifest, m_AppPath,
                                                        updateStatusCallback);
            if (!result.getStatus()) {
                co_return result;
            }
        }
        ProcessUtil::DeleteFileRecursive(m_DownloadPath);
        if (ec) {
            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::Failed,
                .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::DeleteDownloadDirFailed)) + " File:" +
                                m_DownloadPath
            });
            co_return ReturnWrapper{
                false, ErrorCode::DeleteDownloadDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::DeleteDownloadDirFailed)) + " File:" + m_DownloadPath
            };
        }
        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::Completed
        });
        co_return ReturnWrapper{true};
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
