#pragma once

#include "Network/ApiRequest.h"
#include "nlohmann/json.hpp"
#include "UpdateCore/AppVersion.h"
#include "UpdateCore/AppManifest.h"
#include "md5.h"
#include "UpdateLogic/VerifyAndRePatch.hpp"

class DifferenceUpdate {
public:
    DifferenceUpdate(const std::string &host, const std::string &appName, std::string appPath,
                     std::string downloadPath, std::string newVersion)
            : m_Host(host), m_AppName(appName), m_AppPath(std::move(appPath)), m_ApiRequest(host, appName),
              m_DownloadPath(std::move(downloadPath)),
              m_NewVersion(std::move(newVersion)) {

    }

    ReturnWrapper execute() {
        auto [result1, appVersionContent] = m_ApiRequest.GetAppVersion(m_NewVersion);
        if (!result1.getStatus()) {
            return result1;
        }
        auto appVersion = AppVersion(nlohmann::json::parse(appVersionContent));

        auto [result2, appManifestContent] = m_ApiRequest.GetAppManifest(m_NewVersion);
        if (!result2.getStatus()) {
            return result2;
        }
        auto appManifest = AppManifest(nlohmann::json::parse(appManifestContent));

        std::vector<std::string> newFiles;
        std::vector<std::string> oldFiles;

        std::unordered_map<std::string, std::string> serverMd5Map; // [filepath, md5]

        for (auto &fileManifest: appManifest.getManifestData()) {
            newFiles.push_back(fileManifest.filePath);
            serverMd5Map.insert({fileManifest.filePath, fileManifest.md5});
        }

        for (auto &directoryEntry: std::filesystem::recursive_directory_iterator(m_AppPath)) {
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
        } catch (std::exception &e) {
            std::cerr << "Exception in :" << e.what() << "\n";
        }

        for (auto &file: deleted_elements) {
            std::cout << "Delete File:" << file << "\n";
            auto localFilePath = m_AppPath + "/" + file;
            std::error_code ec;
            std::filesystem::remove(localFilePath, ec);
            if (ec) {
                return {false, ErrorCode::DeleteFileFailed, ec.message()};
            }
        }

        for (auto &fileRelativePath: same_elements) {
            std::string localFilePath = m_AppPath + "/" + fileRelativePath;
            auto localMd5 = calcFileMd5(localFilePath);

            auto serverMd5 = serverMd5Map[fileRelativePath];
            if (localMd5 != serverMd5) {
                std::cout << "Md5 not equal! Download File:" << fileRelativePath << "\n";
                std::cout << "Download Path " << localFilePath << " ,server md5 = " << serverMd5 << " ,local md5="
                          << localMd5 << "\n";

                std::error_code errorCode;
                std::filesystem::remove(localFilePath, errorCode);
                if (errorCode) {
                    return {false, ErrorCode::DeleteFileFailed, errorCode.message()};
                }
                auto [result, _] = m_ApiRequest.DownloadFileFromFullPackage(appVersion.getVersion().getVersionString(),
                                                                            serverMd5, localFilePath);
                if (!result.getStatus()) {
                    return {false, ErrorCode::DownloadFileFailed, result.getErrorMessage()};
                }
            }
        }

        for (auto &file: new_elements) {
            std::cout << "Download File:" << file << "\n";
            std::string localFilePath = m_AppPath + "/" + file;
            auto serverMd5 = serverMd5Map[localFilePath];
            auto [result, _] = m_ApiRequest.DownloadFileFromFullPackage(appVersion.getVersion().getVersionString(),
                                                                        serverMd5, localFilePath);
            if (!result.getStatus()) {
                return {false, ErrorCode::DownloadFileFailed,
                        std::string(magic_enum::enum_name(ErrorCode::DownloadFileFailed))};
            }
        }

        std::cout << "Start Verify...\n";

        auto result = VerifyAndRePatch::execute(m_ApiRequest, appVersion, appManifest, m_AppPath);
        if (!result.getStatus()) {
            return result;
        }

        std::error_code ec;
        std::filesystem::remove_all(m_DownloadPath, ec);
        if (ec) {
            return {false, ErrorCode::DeleteDownloadDirFailed,
                    std::string(magic_enum::enum_name(ErrorCode::DeleteDownloadDirFailed))};
        }

        return {true};
    }

private:
    ApiRequest m_ApiRequest;
    std::string m_Host;
    std::string m_AppName;
    std::string m_AppPath;
    std::string m_DownloadPath;
    std::string m_NewVersion;
};

