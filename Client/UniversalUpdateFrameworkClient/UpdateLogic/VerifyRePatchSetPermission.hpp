#pragma once

#include <fstream>
#include <filesystem>
#include "UpdateCore/TypeDefinition.hpp"
#include "Network/ApiRequest.hpp"
#include <AppVersion.hpp>
#include <AppManifest.hpp>
#include "md5.h"
#include <FilePermission.hpp>

class VerifyRePatchSetPermission {
public:
    static ReturnWrapper
    execute(ApiRequest &api, AppVersionInfo &appVersion, AppManifestInfo &appManifest, const std::string &appPath) {
        std::unordered_set<string> fileRelativrPathSet;
        for (auto &fileManifest: appManifest.Manifests) {
            fileRelativrPathSet.insert(fileManifest.FilePath);
        }

        for (auto &dirEntry: std::filesystem::recursive_directory_iterator(appPath)) {
            if (!dirEntry.is_directory()) {
                auto iter = fileRelativrPathSet.find(std::filesystem::relative(dirEntry.path(), appPath).string());
                if (iter == fileRelativrPathSet.end()) {
                    if (!ProcessUtil::DeleteFileRecursiveForce(dirEntry.path())) {
//                        return {false, ErrorCode::DeleteFileFailed,
//                                std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed))};
                        std::cout << std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed)) << " FilePath: "<<dirEntry.path()<< "\n";
                    }
                }
            }
        }

        for (auto &fileManifest: appManifest.Manifests) {
            auto localFilePath = appPath + "/" + fileManifest.FilePath;
            if (!std::filesystem::exists(localFilePath)) {
                std::cout << "Not exist! Download:" << fileManifest.FilePath << "\n";
                auto [result, _] = api.DownloadFileFromFullPackage(appVersion.AppVersion,
                                                                   fileManifest.Md5, localFilePath);
                if (!result.getStatus()) {
                    return {false, ErrorCode::DownloadFileFailed,
                            std::string(magic_enum::enum_name(ErrorCode::DownloadFileFailed))};
                }
            }

            std::ifstream infile(localFilePath);
            if (!infile.is_open()) {
                return {false, ErrorCode::AccessFileFailed,
                        std::string(magic_enum::enum_name(ErrorCode::AccessFileFailed))};
            }

            if (fileManifest.Md5 != calcFileMd5(localFilePath)) {
                std::cout << "Md5 not equal! Download:" << fileManifest.FilePath << "\n";
                std::cout << "Download Path " << localFilePath << " ,server md5 = " << fileManifest.Md5<<" ,local md5="<< calcFileMd5(localFilePath)<< "\n";

                if (!ProcessUtil::DeleteFileRecursiveForce(localFilePath)) {
                    return {false, ErrorCode::DeleteFileFailed, "DeleteFileFailed! FilePath: " + localFilePath};
                }
                auto [result, _] = api.DownloadFileFromFullPackage(appVersion.AppVersion,
                                                                   fileManifest.Md5, localFilePath);
                if (!result.getStatus()) {
                    return {false, ErrorCode::DownloadFileFailed,
                            std::string(magic_enum::enum_name(ErrorCode::DownloadFileFailed))};
                }
            }

            FilePermission filePermission(localFilePath);
            filePermission.WritePermission(fileManifest.Permission);
        }
        return {true};
    }
};
