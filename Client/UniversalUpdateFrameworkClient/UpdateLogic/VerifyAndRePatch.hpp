#pragma once

#include <fstream>
#include <filesystem>
#include "UpdateCore/TypeDefinition.hpp"
#include "Network/ApiRequest.hpp"
#include <AppVersion.hpp>
#include <AppManifest.hpp>
#include "md5.h"

class VerifyAndRePatch {
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
                    std::error_code ec;
                    std::filesystem::remove(dirEntry.path(), ec);
                    if (ec) {
//                        return {false, ErrorCode::DeleteFileFailed,
//                                std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed))};
                        std::cout << std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed)) << " with "<<ec.message()<< "\n";
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

                std::error_code errorCode;
                std::filesystem::remove(localFilePath, errorCode);
                if (errorCode) {
                    return {false, ErrorCode::DeleteFileFailed, errorCode.message()};
                }
                auto [result, _] = api.DownloadFileFromFullPackage(appVersion.AppVersion,
                                                                   fileManifest.Md5, localFilePath);
                if (!result.getStatus()) {
                    return {false, ErrorCode::DownloadFileFailed,
                            std::string(magic_enum::enum_name(ErrorCode::DownloadFileFailed))};
                }
            }
        }
        return {true};
    }
};
