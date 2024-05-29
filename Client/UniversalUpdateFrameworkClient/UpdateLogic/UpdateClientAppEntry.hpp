#pragma once

#include <iostream>

#include "UpdateLogic/FullPackageUpdate.hpp"
#include "UpdateLogic/DifferencePackageUpdate.hpp"
#include "UpdateLogic/MultiVersionDifferencePackageUpdate.hpp"
#include "UpdateLogic/DifferenceUpdate.hpp"
#include "UpdateCore/UpdateConfig.hpp"
#include <Defination.hpp>
#include "UpdateSelf.hpp"

inline ReturnWrapper handleUpdateMode(UpdateMode &mode, UpdateConfigIo &config, std::string updateToNewVersion) {
    switch (mode) {
        case UpdateMode::Unknown: {
            std::cout << "Unknown update mode!\n";
            return {false};
        }
        case UpdateMode::FullPackageUpdate: {
            FullPackageUpdate update(config.getConfig().host,
                                     config.getConfig().appName,
                                     config.getConfig().channel,
                                     config.getConfig().platform,
                                     config.getConfig().appPath,
                                     config.getConfig().downloadPath,
                                     updateToNewVersion);
            auto res = async_simple::coro::syncAwait(update.execute());

            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
                return res;
            }
            return res;
        }
        case UpdateMode::DifferencePackageUpdate: {
            if (config.getConfig().localCurrentVersion == "x.x.x") {
                std::cout << "Invalid local current version!\n";
                return {false};
            }

            DifferencePackageUpdate update(config.getConfig().host,
                                           config.getConfig().appName,
                                           config.getConfig().channel,
                                           config.getConfig().platform,
                                           config.getConfig().appPath,
                                           config.getConfig().downloadPath,
                                           config.getConfig().localCurrentVersion,
                                           updateToNewVersion);
            auto res = async_simple::coro::syncAwait(update.execute());
            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
                return res;
            }
            return res;
        }
        case UpdateMode::MultiVersionDifferencePackageUpdate: {
            if (config.getConfig().localCurrentVersion == "x.x.x") {
                std::cout << "Invaid local current version!\n";
                return {false};
            }

            MultiVersionDifferencePackageUpdate update(config.getConfig().host,
                                                       config.getConfig().appName,
                                                       config.getConfig().channel,
                                                       config.getConfig().platform,
                                                       config.getConfig().appPath,
                                                       config.getConfig().downloadPath,
                                                       config.getConfig().localCurrentVersion,
                                                       updateToNewVersion);
            auto res = async_simple::coro::syncAwait(update.execute());
            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
                return res;
            }
            return res;
        }
        case UpdateMode::DifferenceUpdate: {
            DifferenceUpdate update(config.getConfig().host,
                                    config.getConfig().appName,
                                    config.getConfig().channel,
                                    config.getConfig().platform,
                                    config.getConfig().appPath,
                                    config.getConfig().downloadPath,
                                    updateToNewVersion);
            auto res = async_simple::coro::syncAwait(update.execute());
            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
                return res;
            }
            return res;
        }
    }
}
