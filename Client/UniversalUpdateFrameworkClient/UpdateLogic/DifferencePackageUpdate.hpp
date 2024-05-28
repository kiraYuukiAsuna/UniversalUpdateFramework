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

    ReturnWrapper execute() {
        auto [result1, appVersionContent] = m_ApiRequest.GetAppVersion(m_NewVersion);
        if (!result1.getStatus()) {
            return result1;
        }
        AppVersionInfo appVersion = nlohmann::json::parse(appVersionContent);

        auto [result2, appManifestContent] = m_ApiRequest.GetAppManifest(m_NewVersion);
        if (!result2.getStatus()) {
            return result2;
        }
        AppManifestInfo appManifest = nlohmann::json::parse(appManifestContent);

        auto [result3, appDifferencePackageManifestContent] = m_ApiRequest.GetAppDifferencePackageManifest(
            m_NewVersion);
        if (!result3.getStatus()) {
            return result3;
        }
        DifferencePackageManifestInfo appDifferencePackageManifest = nlohmann::json::parse(appDifferencePackageManifestContent);

        std::filesystem::path downloadRootPath = m_DownloadPath + "/" + m_AppPath + "/" + m_AppName;
        ProcessUtil::DeleteFileRecursiveForce(downloadRootPath);
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
                                                                          appDifferencePackageManifest.AppBeforeVersion + "_to_" +
                                                                          appDifferencePackageManifest.AppCurrentVersion);
        m_ApiRequest.DownloadDifferencePackage(m_NewVersion, differencePackageFile.string());

        auto uncompresssedPath = downloadRootPath /
                                 ("differencepackage_" + appVersion.AppVersion + "_uncompressed");
        std::string shellCommand = std::format(R"({} "" "{}" "{}")", hpatchzExecuable, differencePackageFile.string(),
                                               uncompresssedPath.string());
        std::cout << shellCommand << "\n";
        system(shellCommand.c_str());


        for (auto&file: appDifferencePackageManifest.diff_deletedfiles) {
            std::cout << "Delete File:" << file << "\n";
            auto localFilePath = m_AppPath + "/" + file;
            if (!ProcessUtil::DeleteFileRecursiveForce(localFilePath)) {
                return {
                    false, ErrorCode::DeleteFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::DeleteFileFailed))
                };
            }
        }

        for (auto&file: appDifferencePackageManifest.diff_updatefiles) {
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

            if (!ProcessUtil::DeleteFileRecursiveForce(localFilePath)) {
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

        for (auto&file: appDifferencePackageManifest.diff_newfiles) {
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

        auto result = VerifyRePatchSetPermission::execute(m_ApiRequest, appVersion, appManifest, m_AppPath);
        if (!result.getStatus()) {
            return result;
        }

        if (!ProcessUtil::DeleteFileRecursiveForce(m_DownloadPath)) {
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
