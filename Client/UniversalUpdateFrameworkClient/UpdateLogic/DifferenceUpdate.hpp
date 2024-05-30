#pragma once

#include "Network/ApiRequest.hpp"
#include "nlohmann/json.hpp"
#include <AppVersion.hpp>
#include <AppManifest.hpp>
#include "md5.h"
#include "UpdateLogic/VerifyRePatchSetPermission.hpp"

class DifferenceUpdate {
public:
    DifferenceUpdate(const std::string&host, const std::string&appName, std::string channel,
                     std::string platform, std::string appPath,
                     std::string downloadPath, std::string newVersion)
        : m_Host(host), m_AppName(appName), m_AppPath(std::move(appPath)),
          m_ApiRequest(host, appName, channel, platform), m_Channel(channel), m_Platform(platform),
          m_DownloadPath(std::move(downloadPath)),
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
            .status = UpdateStatus::DifferenceUpdateInstalling,
        });

        std::vector<std::string> newFiles;
        std::vector<std::string> oldFiles;

        std::unordered_map<std::string, std::string> serverMd5Map; // [filepath, md5]

        for (auto&fileManifest: appManifest.Manifests) {
            newFiles.push_back(fileManifest.FilePath);
            serverMd5Map.insert({fileManifest.FilePath, fileManifest.Md5});
        }

        ProcessUtil::TerminateProcessByFilePath(m_AppPath);

        if (!std::filesystem::exists(m_AppPath)) {
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
                    std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed)) + " , File:" + m_AppPath
                };
            }
        }

        for (auto&directoryEntry: std::filesystem::recursive_directory_iterator(m_AppPath)) {
            if (!directoryEntry.is_directory()) {
                auto relativePath = std::filesystem::relative(directoryEntry.path().string(),
                                                              std::filesystem::path(m_AppPath));
                oldFiles.push_back(relativePath.string());
            }
        }
        std::vector<std::string> same_elements;
        std::vector<std::string> new_elements;
        std::vector<std::string> deleted_elements;

        // 对 newFiles 和 oldFiles 进行排序
        sort(newFiles.begin(), newFiles.end());
        sort(oldFiles.begin(), oldFiles.end());

        // 找到两个容器中相同的元素
        try {
            std::set_intersection(newFiles.begin(), newFiles.end(), oldFiles.begin(), oldFiles.end(),
                                  std::back_inserter(same_elements));

            // 找到newFiles比oldFiles新增的元素
            std::set_difference(newFiles.begin(), newFiles.end(), oldFiles.begin(), oldFiles.end(),
                                std::back_inserter(new_elements));

            // 找到newFiles比oldFiles缺少的元素
            std::set_difference(oldFiles.begin(), oldFiles.end(), newFiles.begin(), newFiles.end(),
                                std::back_inserter(deleted_elements));
        }
        catch (std::exception&e) {
            std::cerr << "Exception in :" << e.what() << "\n";
        }

        for (auto&file: deleted_elements) {
            auto localFilePath = m_AppPath + "/" + file;

            SEELE_INFO_TAG(__func__, "Delete File: {}", file);

            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::DeletingFile, .CurrentFileName = localFilePath
            });

            if (!ProcessUtil::DeleteFileRecursive(localFilePath)) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Warning,
                    .WarningMessage = "Delete File Failed! FilePath: " + localFilePath

                });
                SEELE_INFO_TAG(__func__, "Delete File Failed! FilePath: {}", localFilePath);
                // co_return ReturnWrapper{
                //     false, ErrorCode::DeleteFileFailed, "Delete File Failed! FilePath: " + localFilePath
                // };
            }
        }

        for (auto&fileRelativePath: same_elements) {
            std::string localFilePath = m_AppPath + "/" + fileRelativePath;

            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::UpdatingFile, .CurrentFileName = localFilePath
            });

            auto localMd5 = calcFileMd5(localFilePath);
            auto serverMd5 = serverMd5Map[fileRelativePath];
            if (localMd5 != serverMd5) {
                SEELE_INFO_TAG(__func__, "Md5 not equal! Download File: {}", fileRelativePath);

                SEELE_INFO_TAG(__func__, "{}", "Download Path " + localFilePath + " ,server md5 = " + serverMd5 + " ,local md5="
                        + localMd5);

                if (!ProcessUtil::DeleteFileRecursive(localFilePath)) {
                    updateStatusCallback(UpdateStatusInfo{
                        .status = UpdateStatus::Warning,
                        .WarningMessage = "Delete File Failed! FilePath: " + localFilePath

                    });
                    SEELE_INFO_TAG(__func__, "Delete File Failed! FilePath: {}", localFilePath);
                    continue;
                    // co_return ReturnWrapper{
                    //     false, ErrorCode::DeleteFileFailed, "Delete File Failed! FilePath: " + localFilePath
                    // };
                }
                auto [result, _] = co_await m_ApiRequest.DownloadFileFromFullPackage(appVersion.AppVersion,
                    serverMd5, localFilePath);
                if (!result.getStatus()) {
                    updateStatusCallback(UpdateStatusInfo{
                        .status = UpdateStatus::Failed,
                        .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::DownloadFileFailed)) + "\n" +
                                        result.getErrorMessage()
                    });
                    co_return ReturnWrapper{false, ErrorCode::DownloadFileFailed, result.getErrorMessage()};
                }
            }
        }

        for (auto&file: new_elements) {
            std::filesystem::path localFilePath = m_AppPath + "/" + file;

            SEELE_INFO_TAG(__func__, "Download File: {}", file);

            updateStatusCallback(UpdateStatusInfo{
                .status = UpdateStatus::UpdatingFile, .CurrentFileName = localFilePath.string()
            });

            if (auto directory = localFilePath.parent_path(); !std::filesystem::exists(directory)) {
                std::error_code ec;
                std::filesystem::create_directories(directory, ec);
                if (ec) {
                    updateStatusCallback(UpdateStatusInfo{
                        .status = UpdateStatus::Failed,
                        .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::CreateDirFailed)) + "\n" + ec.
                                        message()
                    });
                    co_return ReturnWrapper{false, ErrorCode::CreateDirFailed, ec.message()};
                }
            }

            auto serverMd5 = serverMd5Map[file];
            auto [result, _] = co_await m_ApiRequest.DownloadFileFromFullPackage(appVersion.AppVersion,
                serverMd5, localFilePath.string());
            if (!result.getStatus()) {
                updateStatusCallback(UpdateStatusInfo{
                    .status = UpdateStatus::Failed,
                    .ErrorMessage = std::string(magic_enum::enum_name(ErrorCode::DownloadFileFailed)) + "\n" + result.
                                    getErrorMessage()
                });
                co_return ReturnWrapper{false, ErrorCode::DownloadFileFailed, result.getErrorMessage()};
            }
        }

        SEELE_INFO_TAG(__func__, "{}", "Start Verify...");

        auto result = co_await VerifyRePatchSetPermission::execute(m_ApiRequest, appVersion, appManifest, m_AppPath,
                                                                   updateStatusCallback);
        if (!result.getStatus()) {
            co_return result;
        }

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
