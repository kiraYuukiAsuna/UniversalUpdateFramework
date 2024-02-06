#pragma once

#include <fstream>
#include <filesystem>
#include "TypeDefinition.h"
#include "Network/ApiRequest.h"
#include "UpdateCore/AppVersion.h"
#include "UpdateCore/AppManifest.h"
#include "md5.h"

class VerifyAndRePatch {
public:
    static ReturnWrapper
    execute(ApiRequest &api, AppVersion &appVersion, AppManifest &appManifest, const std::string &appPath) {
        std::unordered_set<string> fileRelativrPathSet;
        for (auto &fileManifest: appManifest.getManifestData()) {
            fileRelativrPathSet.insert(fileManifest.filePath);
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

        for (auto &fileManifest: appManifest.getManifestData()) {
            auto localFilePath = appPath + "/" + fileManifest.filePath;
            if (!std::filesystem::exists(localFilePath)) {
                std::cout << "Not exist! Download:" << fileManifest.filePath << "\n";
                auto [result, _] = api.DownloadFileFromFullPackage(appVersion.getVersion().getVersionString(),
                                                                   fileManifest.md5, localFilePath);
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

            if (fileManifest.md5 != calcFileMd5(localFilePath)) {
                std::cout << "Md5 not equal! Download:" << fileManifest.filePath << "\n";
                std::cout << "Download Path " << localFilePath << " ,server md5 = " << fileManifest.md5<<" ,local md5="<< calcFileMd5(localFilePath)<< "\n";

                std::error_code errorCode;
                std::filesystem::remove(localFilePath, errorCode);
                if (errorCode) {
                    return {false, ErrorCode::DeleteFileFailed, errorCode.message()};
                }
                auto [result, _] = api.DownloadFileFromFullPackage(appVersion.getVersion().getVersionString(),
                                                                   fileManifest.md5, localFilePath);
                if (!result.getStatus()) {
                    return {false, ErrorCode::DownloadFileFailed,
                            std::string(magic_enum::enum_name(ErrorCode::DownloadFileFailed))};
                }
            }
        }
        return {true};
    }
};
