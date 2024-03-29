#pragma once

#include <string>
#include <iostream>
#include <utility>
#include "Network/ApiRequest.hpp"
#include "util.hpp"
#include "UpdateCore/AppVersion.hpp"
#include "UpdateCore/AppManifest.hpp"
#include "UpdateCore/DifferencePackageManifest.hpp"
#include "VerifyAndRePatch.hpp"

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

        auto [result3, appDifferencePackageManifestContent] = m_ApiRequest.GetAppDifferencePackageManifest(
            m_NewVersion);
        if (!result3.getStatus()) {
            return result3;
        }
        auto appDifferencePackageManifest = DifferencePackageManifest(
            nlohmann::json::parse(appDifferencePackageManifestContent));

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
        std::filesystem::path appDifferencePackageManifestPath =
                downloadRootPath /
                ("appdifferencepackagemanifest_" + appDifferencePackageManifest.getOldVersion().getVersionString() +
                 "_to_" + appDifferencePackageManifest.getNewAppVersion().getVersion().getVersionString() +
                 ".json");

        util::saveToFile(appVersionContent, appVersionPath);
        util::saveToFile(appManifestContent, appManifestPath);
        util::saveToFile(appDifferencePackageManifestContent, appDifferencePackageManifestPath);

        std::filesystem::path differencePackageFile = downloadRootPath / ("differencepackage_" +
                                                                          appDifferencePackageManifest.getOldVersion().
                                                                          getVersionString() +
                                                                          "_to_" +
                                                                          appDifferencePackageManifest.
                                                                          getNewAppVersion().getVersion().
                                                                          getVersionString());
        m_ApiRequest.DownloadDifferencePackage(m_NewVersion, differencePackageFile.string());

        auto uncompresssedPath = downloadRootPath /
                                 ("differencepackage_" + appVersion.getVersion().getVersionString() + "_uncompressed");
        std::string shellCommand = std::format(R"({} "" "{}" "{}")", hpatchzExecuable, differencePackageFile.string(),
                                               uncompresssedPath.string());
        std::cout << shellCommand << "\n";
        system(shellCommand.c_str());


        for (auto&file: appDifferencePackageManifest.getDiffDeletedFiles()) {
            std::cout << "Delete File:" << file << "\n";
            auto localFilePath = m_AppPath + "/" + file;
            std::error_code ec;
            std::filesystem::remove(localFilePath, ec);
            if (ec) {
                return {
                    false, ErrorCode::DeleteFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed))
                };
            }
        }

        for (auto&file: appDifferencePackageManifest.getDiffUpdateFiles()) {
            std::cout << "Update File:" << file << "\n";
            std::filesystem::path localFilePath = m_AppPath + "/" + file;
            auto diffFilePath = uncompresssedPath / file;
            std::filesystem::path tempUpdatePath = downloadRootPath / "DifferenceUpdate" / file;

            std::error_code ec;
            std::filesystem::create_directories(tempUpdatePath.parent_path(), ec);
            if (ec) {
                return {
                    false, ErrorCode::CreateAppDirFailed,
                    std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed))
                };
            }

            std::string patchShellCommand = std::format(R"({} "{}" "{}" "{}")", hpatchzExecuable,
                                                        localFilePath.string(),
                                                        diffFilePath.string(), tempUpdatePath.string());
            std::cout << patchShellCommand << "\n";
            system(patchShellCommand.c_str());

            std::filesystem::remove(localFilePath, ec);
            if (ec) {
                return {
                    false, ErrorCode::DeleteFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed))
                };
            }
            std::filesystem::copy_file(tempUpdatePath, localFilePath, ec);
            if (ec) {
                //                return {false, ErrorCode::CopyFileFailed,
                //                        std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed))};
                std::cout << std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed)) << "\n";
            }
        }

        for (auto&file: appDifferencePackageManifest.getDiffNewFiles()) {
            std::cout << "Add New File:" << file << "\n";
            std::filesystem::path localFilePath = m_AppPath + "/" + file;
            auto diffFilePath = uncompresssedPath / file;

            std::error_code ec;
            std::filesystem::create_directories(localFilePath.parent_path(), ec);
            if (ec) {
                return {
                    false, ErrorCode::CreateAppDirFailed,
                    std::string(magic_enum::enum_name(ErrorCode::CreateAppDirFailed))
                };
            }
            std::filesystem::copy_file(diffFilePath, localFilePath,
                                       std::filesystem::copy_options::overwrite_existing,
                                       ec);
            if (ec) {
                return {
                    false, ErrorCode::CopyFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::CopyFileFailed))
                };
            }
        }

        auto result = VerifyAndRePatch::execute(m_ApiRequest, appVersion, appManifest, m_AppPath);
        if (!result.getStatus()) {
            return result;
        }

        std::error_code ec;
        std::filesystem::remove_all(m_DownloadPath, ec);
        if (ec) {
            return {
                false, ErrorCode::DeleteDownloadDirFailed,
                std::string(magic_enum::enum_name(ErrorCode::DeleteDownloadDirFailed))
            };
        }

        return {true};
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
