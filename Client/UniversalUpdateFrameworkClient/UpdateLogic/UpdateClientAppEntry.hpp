#pragma once

#include <iostream>

#include "UpdateLogic/FullPackageUpdate.hpp"
#include "UpdateLogic/DifferencePackageUpdate.hpp"
#include "UpdateLogic/MultiVersionDifferencePackageUpdate.hpp"
#include "UpdateLogic/DifferenceUpdate.hpp"
#include "cxxopts.hpp"
#include "UpdateCore/UpdateConfig.hpp"
#include "Network/ApiRequest.hpp"

inline void handleUpdateMode(UpdateMode &mode, UpdateConfigIo &config, std::string updateToNewVersion) {
    switch (mode) {
        case UpdateMode::Unknown: {
            std::cout << "Unknown update mode!\n";
            break;
        }
        case UpdateMode::FullPackageUpdate: {
            FullPackageUpdate update(config.getConfig().host,
                                     config.getConfig().appName,
                                     config.getConfig().channel,
                                     config.getConfig().platform,
                                     config.getConfig().appPath,
                                     config.getConfig().downloadPath,
                                     updateToNewVersion);
            auto res = update.execute();
            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
            }

            break;
        }
        case UpdateMode::DifferencePackageUpdate: {
            if (config.getConfig().localCurrentVersion == "x.x.x") {
                std::cout << "Invalid local current version!\n";
                return;
            }

            DifferencePackageUpdate update(config.getConfig().host,
                                           config.getConfig().appName,
                                           config.getConfig().channel,
                                           config.getConfig().platform,
                                           config.getConfig().appPath,
                                           config.getConfig().downloadPath,
                                           config.getConfig().localCurrentVersion,
                                           updateToNewVersion);
            auto res = update.execute();
            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
            }

            break;
        }
        case UpdateMode::MultiVersionDifferencePackageUpdate: {
            if (config.getConfig().localCurrentVersion == "x.x.x") {
                std::cout << "Invaid local current version!\n";
                return;
            }

            MultiVersionDifferencePackageUpdate update(config.getConfig().host,
                                                       config.getConfig().appName,
                                                       config.getConfig().channel,
                                                       config.getConfig().platform,
                                                       config.getConfig().appPath,
                                                       config.getConfig().downloadPath,
                                                       config.getConfig().localCurrentVersion,
                                                       updateToNewVersion);
            auto res = update.execute();
            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
            }

            break;
        }
        case UpdateMode::DifferenceUpdate: {
            DifferenceUpdate update(config.getConfig().host,
                                    config.getConfig().appName,
                                    config.getConfig().channel,
                                    config.getConfig().platform,
                                    config.getConfig().appPath,
                                    config.getConfig().downloadPath,
                                    updateToNewVersion);
            auto res = update.execute();
            std::cout << res.getErrorMessage() << "\n";

            if (res.getStatus()) {
                auto cfg = config.getConfig();
                cfg.localCurrentVersion = updateToNewVersion;
                config.setConfig(cfg);
                config.writeToFile();
            }

            break;
        }
    }
}
