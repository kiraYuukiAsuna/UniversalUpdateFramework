#pragma once

#include <TypeDefination/AppVersion.hpp>
#include <TypeDefination/DifferencePackageManifest.hpp>
#include <Version.hpp>

#include "DifferencePackageUpdate.hpp"
#include "Network/ApiRequest.hpp"
#include "UpdateCore/TypeDefinition.hpp"

class MultiVersionDifferencePackageUpdate {
public:
	MultiVersionDifferencePackageUpdate(
		const std::string& host, const std::string& appName,
		std::string channel, std::string platform, std::string appPath,
		std::string downloadPath, std::string oldVersion,
		std::string newVersion)
		: m_Host(host),
		  m_AppName(appName),
		  m_AppPath(std::move(appPath)),
		  m_ApiRequest(host, appName, channel, platform),
		  m_Channel(channel),
		  m_Platform(platform),
		  m_DownloadPath(std::move(downloadPath)),
		  m_OldVersion(std::move(oldVersion)),
		  m_NewVersion(std::move(newVersion)) {}

	async_simple::coro::Lazy<ReturnWrapper> execute(
		std::function<void(UpdateStatusInfo)> updateStatusCallback) {
		// check versions number
		auto [res, versionsContent] = co_await m_ApiRequest.GetAppVersionList();
		if (!res.getStatus()) {
			updateStatusCallback(UpdateStatusInfo{
				.status = UpdateStatus::Failed,
				.ErrorMessage = std::string(magic_enum::enum_name(
									ErrorCode::HttpRequestError)) +
								"\n" + res.getErrorMessage()});
			co_return ReturnWrapper{false, ErrorCode::HttpRequestError,
									std::string(magic_enum::enum_name(
										ErrorCode::HttpRequestError)) +
										"\n" + res.getErrorMessage()};
		}

		auto versionsJson = nlohmann::json::parse(versionsContent);
		std::vector<Version> versions;
		for (auto& versionJson : versionsJson) {
			Version ver(versionJson.get<std::string>());
			versions.push_back(ver);
		}

		std::sort(versions.begin(), versions.end());
		std::for_each(versions.begin(), versions.end(), [](Version& ver) {
			SEELE_INFO_TAG(__func__, "{}", ver.getVersionString());
		});

		updateStatusCallback(UpdateStatusInfo{
			.status = UpdateStatus::DownloadAppVersionFile,
		});
		auto [res2, currentVersionContent] =
			co_await m_ApiRequest.GetCurrentAppVersion();
		if (!res2.getStatus()) {
			updateStatusCallback(UpdateStatusInfo{
				.status = UpdateStatus::Failed,
				.ErrorMessage = std::string(magic_enum::enum_name(
									ErrorCode::HttpRequestError)) +
								"\n" + res.getErrorMessage()});
			co_return ReturnWrapper{false, ErrorCode::HttpRequestError,
									std::string(magic_enum::enum_name(
										ErrorCode::HttpRequestError)) +
										"\n" + res.getErrorMessage()};
		}

		AppVersionInfo appCurrentVersion =
			nlohmann::json::parse(currentVersionContent);

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
			updateStatusCallback(UpdateStatusInfo{
				.status = UpdateStatus::Failed,
				.ErrorMessage = std::string(
					magic_enum::enum_name(ErrorCode::NoOldVersionOnServer))});
			co_return ReturnWrapper{false, ErrorCode::NoOldVersionOnServer,
									std::string(magic_enum::enum_name(
										ErrorCode::NoOldVersionOnServer))};
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
			updateStatusCallback(UpdateStatusInfo{
				.status = UpdateStatus::Failed,
				.ErrorMessage = std::string(
					magic_enum::enum_name(ErrorCode::NoNewVersionOnServer))});
			co_return ReturnWrapper{false, ErrorCode::NoNewVersionOnServer,
									std::string(magic_enum::enum_name(
										ErrorCode::NoNewVersionOnServer))};
		}

		if (oldVersionIdx >= currentVersionIdx ||
			oldVersionIdx == versions.size() - 1) {
			// current version is the newest version
			updateStatusCallback(UpdateStatusInfo{
				.status = UpdateStatus::Failed,
				.ErrorMessage = std::string(magic_enum::enum_name(
					ErrorCode::LocalVersionIsLatestVersion))});
			co_return ReturnWrapper{
				false, ErrorCode::LocalVersionIsLatestVersion,
				std::string(magic_enum::enum_name(
					ErrorCode::LocalVersionIsLatestVersion))};
		}

		for (int startIdx = oldVersionIdx; startIdx < currentVersionIdx;
			 startIdx++) {
			updateStatusCallback(UpdateStatusInfo{
				.status = UpdateStatus::MutilVersionUpdateInstalling,
			});
			DifferencePackageUpdate differencePackageUpdate(
				m_Host, m_AppName, m_Channel, m_Platform, m_AppPath,
				m_DownloadPath, versions.at(startIdx).getVersionString(),
				versions.at(startIdx + 1).getVersionString());

			auto diffUpdateResult =
				co_await differencePackageUpdate.execute(updateStatusCallback);
			if (!diffUpdateResult.getStatus()) {
				co_return diffUpdateResult;
			}
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
	std::string m_OldVersion;
	std::string m_NewVersion;
};
