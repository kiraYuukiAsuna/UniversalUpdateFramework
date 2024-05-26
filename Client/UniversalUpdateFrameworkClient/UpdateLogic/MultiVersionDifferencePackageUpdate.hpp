#pragma once

#include "UpdateCore/TypeDefinition.hpp"
#include "Network/ApiRequest.hpp"
#include <Version.hpp>
#include <AppVersion.hpp>
#include <DifferencePackageManifest.hpp>
#include "DifferencePackageUpdate.hpp"

class MultiVersionDifferencePackageUpdate {
public:
    MultiVersionDifferencePackageUpdate(const std::string &host, const std::string &appName, std::string channel,
                                        std::string platform, std::string appPath,
                                        std::string downloadPath, std::string oldVersion, std::string newVersion)
            : m_Host(host), m_AppName(appName), m_AppPath(std::move(appPath)),
              m_ApiRequest(host, appName, channel, platform), m_Channel(channel), m_Platform(platform),
              m_DownloadPath(std::move(downloadPath)), m_OldVersion(std::move(oldVersion)),
              m_NewVersion(std::move(newVersion)) {

    }

    ReturnWrapper execute() {
        // check versions number
        auto [res, versionsContent] = m_ApiRequest.GetAppVersionList();
        if (!res.getStatus()) {
            return {false, ErrorCode::HttpRequestError,
                    std::string(magic_enum::enum_name(ErrorCode::HttpRequestError))};
        }

        auto versionsJson = nlohmann::json::parse(versionsContent);
        std::vector<Version> versions;
        for (auto &versionJson: versionsJson) {
            Version ver(versionJson.get<std::string>());
            versions.push_back(ver);
        }

        std::sort(versions.begin(), versions.end());
        std::for_each(versions.begin(), versions.end(), [](Version &ver) {
            std::cout << ver.getVersionString() << "\n";
        });

        auto [res2, currentVersionContent] = m_ApiRequest.GetCurrentAppVersion();
        if (!res2.getStatus()) {
            return {false, ErrorCode::HttpRequestError,
                    std::string(magic_enum::enum_name(ErrorCode::HttpRequestError))};
        }

        AppVersionInfo appCurrentVersion = nlohmann::json::parse(currentVersionContent);

        bool bFindOldVersion{false};
        int oldVersionIdx = -1;
        for (int idx = 0; idx < versions.size(); idx++) {
            if (versions.at(idx) == Version(m_OldVersion)) {
                bFindOldVersion = true;
                oldVersionIdx = idx;
            }
        }
        if (!bFindOldVersion) {
            // cannot find oldverion in server
            return {false, ErrorCode::NoOldVersionOnServer,
                    std::string(magic_enum::enum_name(ErrorCode::NoOldVersionOnServer))};
        }

        bool bFindCurrentVersion{false};
        int currentVersionIdx = -1;
        for (int idx = 0; idx < versions.size(); idx++) {
            if (versions.at(idx) == Version{appCurrentVersion.AppVersion}) {
                bFindCurrentVersion = true;
                currentVersionIdx = idx;
            }
        }
        if (!bFindCurrentVersion) {
            // cannot find currentversion in server
            return {false, ErrorCode::NoNewVersionOnServer,
                    std::string(magic_enum::enum_name(ErrorCode::NoNewVersionOnServer))};
        }

        if (oldVersionIdx >= currentVersionIdx || oldVersionIdx == versions.size() - 1) {
            // current version is the newest version
            return {false, ErrorCode::LocalVersionIsLatestVersion,
                    std::string(magic_enum::enum_name(ErrorCode::LocalVersionIsLatestVersion))};
        }

        for (int startIdx = oldVersionIdx; startIdx < currentVersionIdx; startIdx++) {

            DifferencePackageUpdate differencePackageUpdate(m_Host, m_AppName, m_Channel, m_Platform, m_AppPath,
                                                            m_DownloadPath,
                                                            versions.at(startIdx).getVersionString(),
                                                            versions.at(startIdx + 1).getVersionString()
            );

            auto res = differencePackageUpdate.execute();
            if (!res.getStatus()) {
                return res;
            }
        }

        return {true};
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
