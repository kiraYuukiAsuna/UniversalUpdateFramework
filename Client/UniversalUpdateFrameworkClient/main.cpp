#include <iostream>

#include "UpdateLogic/FullPackageUpdate.hpp"
#include "UpdateLogic/DifferencePackageUpdate.hpp"
#include "UpdateLogic/MultiVersionDifferencePackageUpdate.hpp"
#include "UpdateLogic/DifferenceUpdate.hpp"
#include "cxxopts.hpp"
#include "UpdateCore/UpdateConfig.h"

int main(int argc, char *argv[]) {
    try {
        cxxopts::Options options("UniversalUpdateFramework Client",
                                 "A tool that help you update your app.");

        options.add_options()
                ("m,mode",
                 "Update Mode. (-m FullPackageUpdate/DifferencePackageUpdate/MultiVersionDifferencePackageUpdate/DifferenceUpdate)",
                 cxxopts::value<std::string>())
                ("c,config", "Config file path.", cxxopts::value<std::string>())
                ("h,help", "Print usage.");
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (result.count("mode")) {
            std::string modeString = result["mode"].as<std::string>();
            UpdateMode mode;
            mode = magic_enum::enum_cast<UpdateMode>(modeString).value_or(UpdateMode::Unknown);

            if (result.count("config")) {
                std::string configFilePath = result["config"].as<std::string>();
                UpdateConfig config(configFilePath);
                config.writeToFile();

                std::string updateToNewVersion = "x.x.x";

                switch (mode) {
                    case UpdateMode::Unknown: {
                        std::cout << "Unknown update mode!\n";
                        break;
                    }
                    case UpdateMode::FullPackageUpdate: {
                        FullPackageUpdate update(config.getConfig().host,
                                                 config.getConfig().appName,
                                                 config.getConfig().appPath,
                                                 config.getConfig().downloadPath,
                                                 updateToNewVersion);
                        auto res = update.execute();
                        std::cout << res.getErrorMessage() << "\n";

                        break;
                    }
                    case UpdateMode::DifferencePackageUpdate: {
                        DifferencePackageUpdate update(config.getConfig().host,
                                                       config.getConfig().appName,
                                                       config.getConfig().appPath,
                                                       config.getConfig().downloadPath,
                                                       config.getConfig().localCurrentVersion,
                                                       updateToNewVersion);
                        auto res = update.execute();
                        std::cout << res.getErrorMessage() << "\n";

                        break;
                    }
                    case UpdateMode::MultiVersionDifferencePackageUpdate: {
                        MultiVersionDifferencePackageUpdate update(config.getConfig().host,
                                                                   config.getConfig().appName,
                                                                   config.getConfig().appPath,
                                                                   config.getConfig().downloadPath,
                                                                   config.getConfig().localCurrentVersion,
                                                                   updateToNewVersion);
                        auto res = update.execute();
                        std::cout << res.getErrorMessage() << "\n";

                        break;
                    }
                    case UpdateMode::DifferenceUpdate: {
                        DifferenceUpdate update(config.getConfig().host,
                                                config.getConfig().appName,
                                                config.getConfig().appPath,
                                                config.getConfig().downloadPath,
                                                updateToNewVersion);
                        auto res = update.execute();
                        std::cout << res.getErrorMessage() << "\n";

                        break;
                    }
                }
            } else {
                std::cout << "No config file!\n";
            }

        } else {
            std::cout << "No update mode!\n";
        }

        return 0;
    } catch (std::exception &e) {
        std::cerr << "Exception:" << e.what() << "\n";
    }
}
