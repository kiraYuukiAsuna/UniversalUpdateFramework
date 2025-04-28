#pragma once

#include <Defination.hpp>

#include "UpdateCore/UpdateConfig.hpp"
#include "UpdateCore/UpdateStatus.hpp"
#include "UpdateLogic/DifferencePackageUpdate.hpp"
#include "UpdateLogic/DifferenceUpdate.hpp"
#include "UpdateLogic/FullPackageUpdate.hpp"
#include "UpdateLogic/MultiVersionDifferencePackageUpdate.hpp"
#include "UpdateSelf.hpp"

inline ReturnWrapper handleUpdateMode(
	UpdateMode mode, UpdateConfigIo config, std::string updateToNewVersion,
	std::function<void(UpdateStatusInfo)> updateStatusCallback) {
	switch (mode) {
		case UpdateMode::Unknown: {
			SEELE_INFO_TAG(__func__, "{}", "Unknown update mode!");
			updateStatusCallback(
				{UpdateStatus::Failed, "Unknown update mode!"});
			return {false};
		}
		case UpdateMode::FullPackageUpdate: {
			FullPackageUpdate update(
				config.getConfig().host, config.getConfig().appName,
				config.getConfig().channel, config.getConfig().platform,
				config.getConfig().appPath, config.getConfig().downloadPath,
				updateToNewVersion);
			auto res = async_simple::coro::syncAwait(
				update.execute(updateStatusCallback));

			SEELE_INFO_TAG(__func__, "{}", res.getErrorMessage());

			if (res.getStatus()) {
				auto cfg = config.getConfig();
				cfg.localCurrentVersion = updateToNewVersion;
				config.setConfig(cfg);
				config.writeToFile();
				updateStatusCallback({UpdateStatus::Completed});
				return res;
			}
			updateStatusCallback({UpdateStatus::Failed, res.getErrorMessage()});
			return res;
		}
		case UpdateMode::DifferencePackageUpdate: {
			if (config.getConfig().localCurrentVersion == "x.x.x") {
				SEELE_INFO_TAG(__func__, "{}",
							   "Invalid local current version!");
				updateStatusCallback(
					{UpdateStatus::Failed, "Invalid local current version!"});
				return {false};
			}

			DifferencePackageUpdate update(
				config.getConfig().host, config.getConfig().appName,
				config.getConfig().channel, config.getConfig().platform,
				config.getConfig().appPath, config.getConfig().downloadPath,
				config.getConfig().localCurrentVersion, updateToNewVersion);
			auto res = async_simple::coro::syncAwait(
				update.execute(updateStatusCallback));

			SEELE_INFO_TAG(__func__, "{}", res.getErrorMessage());

			if (res.getStatus()) {
				auto cfg = config.getConfig();
				cfg.localCurrentVersion = updateToNewVersion;
				config.setConfig(cfg);
				config.writeToFile();
				updateStatusCallback({UpdateStatus::Completed});
				return res;
			}
			updateStatusCallback({UpdateStatus::Failed, res.getErrorMessage()});
			return res;
		}
		case UpdateMode::MultiVersionDifferencePackageUpdate: {
			if (config.getConfig().localCurrentVersion == "x.x.x") {
				SEELE_INFO_TAG(__func__, "{}", "Invaid local current version!");
				updateStatusCallback(
					{UpdateStatus::Failed, "Invaid local current version!"});
				return {false};
			}

			MultiVersionDifferencePackageUpdate update(
				config.getConfig().host, config.getConfig().appName,
				config.getConfig().channel, config.getConfig().platform,
				config.getConfig().appPath, config.getConfig().downloadPath,
				config.getConfig().localCurrentVersion, updateToNewVersion);
			auto res = async_simple::coro::syncAwait(
				update.execute(updateStatusCallback));

			SEELE_INFO_TAG(__func__, "{}", res.getErrorMessage());

			if (res.getStatus()) {
				auto cfg = config.getConfig();
				cfg.localCurrentVersion = updateToNewVersion;
				config.setConfig(cfg);
				config.writeToFile();
				updateStatusCallback({UpdateStatus::Completed});
				return res;
			}
			updateStatusCallback({UpdateStatus::Failed, res.getErrorMessage()});
			return res;
		}
		case UpdateMode::DifferenceUpdate: {
			DifferenceUpdate update(
				config.getConfig().host, config.getConfig().appName,
				config.getConfig().channel, config.getConfig().platform,
				config.getConfig().appPath, config.getConfig().downloadPath,
				updateToNewVersion);
			auto res = async_simple::coro::syncAwait(
				update.execute(updateStatusCallback));

			SEELE_INFO_TAG(__func__, "{}", res.getErrorMessage());

			if (res.getStatus()) {
				auto cfg = config.getConfig();
				cfg.localCurrentVersion = updateToNewVersion;
				config.setConfig(cfg);
				config.writeToFile();
				updateStatusCallback({UpdateStatus::Completed});
				return res;
			}
			updateStatusCallback({UpdateStatus::Failed, res.getErrorMessage()});
			return res;
		}
	}
}
