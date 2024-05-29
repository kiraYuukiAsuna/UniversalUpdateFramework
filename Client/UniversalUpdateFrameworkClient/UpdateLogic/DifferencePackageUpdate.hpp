#pragma once

#include <string>
#include <iostream>
#include <utility>
#include "Network/ApiRequest.hpp"
#include "util.hpp"
#include <AppVersion.hpp>
#include <AppManifest.hpp>
#include <DeleteFileForce.hpp>
#include <DifferencePackageManifest.hpp>
#include <Log.h>
#include <UpdateCore/UpdateStatus.hpp>

#include "VerifyRePatchSetPermission.hpp"

class DifferencePackageUpdate {
public:
    DifferencePackageUpdate(const std::string&host, const std::string&appName, std::string channel,
                            std::string platform, std::string appPath,
                            std::string downloadPath, std::string oldVersion, std::string newVersion)
        : m_Host(host), m_AppName(appName), m_AppPath(std::move(appPath)),
          m_ApiRequest(host, appName, channel, platform), m_Channel(channel), m_Platform(platform),
          m_DownloadPath(std::move(downloadPath)), m_OldVersion(std::move(oldVersion)),
          m_NewVersion(std::move(newVersion)) {
    }

    async_simple::coro::Lazy<ReturnWrapper> execute(std::function<void(UpdateStatusInfo)> updateStatusCallback) {
        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadAppVersionFile,
        });
        auto [result1, appVersionContent] = co_await m_ApiRequest.GetAppVersion(m_NewVersion);
        if (!result1.getStatus()) {
            co_return result1;
        }
        AppVersionInfo appVersion = nlohmann::json::parse(appVersionContent);

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadAppManifestFile,
        });
        auto [result2, appManifestContent] = co_await m_ApiRequest.GetAppManifest(m_NewVersion);
        if (!result2.getStatus()) {
            co_return result2;
        }
        AppManifestInfo appManifest = nlohmann::json::parse(appManifestContent);

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadDifferencePackageManifestFile,
        });
        auto [result3, appDifferencePackageManifestContent] = co_await m_ApiRequest.GetAppDifferencePackageManifest(
            m_NewVersion);
        if (!result3.getStatus()) {
            co_return result3;
        }
        DifferencePackageManifestInfo appDifferencePackageManifest = nlohmann::json::parse(
            appDifferencePackageManifestContent);

        ProcessUtil::TerminateProcessByFilePath(m_AppPath);

        std::filesystem::path downloadRootPath = m_DownloadPath + "/" + m_AppPath + "/" + m_AppName;
        ProcessUtil::DeleteFileRecursive(downloadRootPath);
        std::filesystem::create_directories(downloadRootPath);

        std::filesystem::path appVersionPath =
                downloadRootPath / ("appversion_" + appVersion.AppVersion + ".json");
        std::filesystem::path appManifestPath =
                downloadRootPath / ("appmanifest_" + appManifest.AppVersion + ".json");
        std::filesystem::path appDifferencePackageManifestPath =
                downloadRootPath /
                ("appdifferencepackagemanifest_" + appDifferencePackageManifest.AppBeforeVersion +
                 "_to_" + appDifferencePackageManifest.AppCurrentVersion + ".json");

        util::saveToFile(appVersionContent, appVersionPath);
        util::saveToFile(appManifestContent, appManifestPath);
        util::saveToFile(appDifferencePackageManifestContent, appDifferencePackageManifestPath);

        std::filesystem::path differencePackageFile = downloadRootPath / ("differencepackage_" +
                                                                          appDifferencePackageManifest.AppBeforeVersion
                                                                          + "_to_" +
                                                                          appDifferencePackageManifest.
                                                                          AppCurrentVersion);

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DownloadDifferencePackageFile,
        });
        auto [resultx, _] = co_await m_ApiRequest.DownloadDifferencePackage(
            m_NewVersion, differencePackageFile.string());
        if (!resultx.getStatus()) {
            co_return resultx;
        }

        auto uncompresssedPath = downloadRootPath /
                                 ("differencepackage_" + appVersion.AppVersion + "_uncompressed");
        std::string shellCommand = std::format(R"({} "" "{}" "{}")", hpatchzExecuable, differencePackageFile.string(),
                                               uncompresssedPath.string());
        SEELE_INFO_TAG(__func__, "{}", shellCommand);
        system(shellCommand.c_str());

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DifferencePackageUpdateInstalling,
        });

        for (auto&file: appDifferencePackageManifest.diff_deletedfiles) {
            auto localFilePath = m_AppPath + "/" + file;

            SEELE_INFO_TAG(__func__, "Delete File: {}", file);

            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::DeletingFile, .CurrentFileName = localFilePath
            });

            if (!ProcessUtil::DeleteFileRecursive(localFilePath)) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Warning,
                    .WarningMessage = std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed)) + " , File:" +
                                      localFilePath
                });
                SEELE_INFO_TAG(__func__, "{}", std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed)) + " , File:" +
                        localFilePath);
                // co_return ReturnWrapper{
                //     false, ErrorCode::DeleteFileFailed,
                //     std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed)) + " , File:" + localFilePath
                // };
            }
        }

        for (auto&file: appDifferencePackageManifest.diff_updatefiles) {
            std::filesystem::path localFilePath = m_AppPath + "/" + file;

            SEELE_INFO_TAG(__func__, "Update File:{}", file);

            auto diffFilePath = uncompresssedPath / file;
            std::filesystem::path tempUpdatePath = downloadRootPath / "DifferenceUpdate" / file;

            std::error_code ec;
            std::filesystem::create_directories(tempUpdatePath.parent_path(), ec);
            if (ec) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Failed,
                    .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed)) + " , File:" +
                                    tempUpdatePath.parent_path().string()
                });
                co_return ReturnWrapper{
                    false, ErrorCode::CreateAppDirFailed,
                    std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed)) + " , File:" + tempUpdatePath.
                    parent_path().string()
                };
            }

            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::UpdatingFile, .CurrentFileName = localFilePath.string()
            });

            std::string patchShellCommand = std::format(R"({} "{}" "{}" "{}")", hpatchzExecuable,
                                                        localFilePath.string(),
                                                        diffFilePath.string(), tempUpdatePath.string());
            SEELE_INFO_TAG(__func__, "{}",patchShellCommand);
            system(patchShellCommand.c_str());

            if (!ProcessUtil::DeleteFileRecursive(localFilePath)) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Failed,
                    .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed)) + " , File:" +
                                    localFilePath.string()
                });
                co_return ReturnWrapper{
                    false, ErrorCode::DeleteFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed)) + " , File:" +
                    localFilePath.string()
                };
            }
            std::filesystem::copy_file(tempUpdatePath, localFilePath, ec);
            if (ec) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Warning,
                    .WarningMessage = std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed)) + " , From File:" +
                                      tempUpdatePath.string() + " , To File:" + localFilePath.string()

                });
                SEELE_INFO_TAG(__func__, "{}",std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed)) + " , From File:" +
                        tempUpdatePath.string() + " , To File:" + localFilePath.string());
                // co_return ReturnWrapper{
                //     false, ErrorCode::CopyFileFailed,
                //     std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed)) + " , From File:" +
                // tempUpdatePath.string() + " , To File:" + localFilePath.string()
                // };
            }
        }

        for (auto&file: appDifferencePackageManifest.diff_newfiles) {
            std::filesystem::path localFilePath = m_AppPath + "/" + file;

            SEELE_INFO_TAG(__func__, "Add New File: {}",file);

            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::CreatingFile, .CurrentFileName = localFilePath.string()
            });

            auto diffFilePath = uncompresssedPath / file;

            std::error_code ec;
            std::filesystem::create_directories(localFilePath.parent_path(), ec);
            if (ec) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Failed,
                    .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed)) + " , File:" +
                                    localFilePath.parent_path().string()
                });
                co_return ReturnWrapper{
                    false, ErrorCode::CreateAppDirFailed,
                    std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed)) + " , File:" +
                    localFilePath.parent_path().string()
                };
            }
            std::filesystem::copy_file(diffFilePath, localFilePath,
                                       std::filesystem::copy_options::overwrite_existing,
                                       ec);
            if (ec) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Failed,
                    .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed)) + " , File:" +
                                    localFilePath.parent_path().string()
                });
                co_return ReturnWrapper{
                    false, ErrorCode::CopyFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed)) + " , File:" +
                    localFilePath.parent_path().string()
                };
            }
        }

        auto result = co_await VerifyRePatchSetPermission::execute(m_ApiRequest, appVersion, appManifest, m_AppPath,
                                                                   updateStatusCallback);
        if (!result.getStatus()) {
            co_return result;
        }

        updateStatusCallback(UpdateStatusInfo{
            .status = UpdateStatus::DeletingFile, .CurrentFileName = m_DownloadPath
        });
        if (!ProcessUtil::DeleteFileRecursive(m_DownloadPath)) {
            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::Failed,
                .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::DeleteDownloadDirFailed)) + " , File:" +
                                m_DownloadPath
            });
            co_return ReturnWrapper{
                false, ErrorCode::DeleteDownloadDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::DeleteDownloadDirFailed)) + " , File:" +
                m_DownloadPath
            };
        }

        co_return ReturnWrapper{true};
    }

    ApiRequest& getApi() {
        return m_ApiRequest;
    }

private:
    ApiRequest m_ApiRequest;
    std::string m_Host;
    std::string m_AppName;
    std::string m_Channel;
    std::string m_Platform;
    std::string m_AppPath;
    std::string m_DownloadPath;
    std::string m_OldVersion;
    std::string m_NewVersion;
};
