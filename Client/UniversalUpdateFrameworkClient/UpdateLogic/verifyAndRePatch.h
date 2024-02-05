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
        for (auto &fileManifest: appManifest.getManifestData()) {
            auto localFilePath = appPath + "/" + fileManifest.filePath;
            if (!std::filesystem::exists(localFilePath)) {
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

            auto a = calcFileMd5(localFilePath);
            if (fileManifest.md5 != a) {
                std::error_code errorCode;
                std::filesystem::remove(localFilePath, errorCode);
                if (errorCode) {
                    return {false, ErrorCode::DeleteFileFailed,
                            std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed))};
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
