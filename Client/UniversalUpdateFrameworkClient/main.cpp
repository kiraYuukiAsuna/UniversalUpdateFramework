#include <iostream>

#include "UpdateLogic/FullPackageUpdate.hpp"
#include "UpdateLogic/DifferencePackageUpdate.hpp"
#include "UpdateLogic/MultiVersionDifferencePackageUpdate.hpp"
#include "UpdateLogic/DifferenceUpdate.hpp"
#include "cxxopts.hpp"
#include "UpdateCore/UpdateConfig.h"
#include "Network/ApiRequest.h"

int main(int argc, char *argv[]) {
    try {
        cxxopts::Options options("UniversalUpdateFramework Client",
                                 "A tool that help you update your app.");

        options.add_options()
                ("m,mode",
                 "Update Mode. (-m FullPackageUpdate/DifferencePackageUpdate/MultiVersionDifferencePackageUpdate/DifferenceUpdate)",
                 cxxopts::value<std::string>())
                ("c,config", "Config file path.", cxxopts::value<std::string>())
                ("v,newversion", "New version to update. If not given by default will update to the newest version.", cxxopts::value<std::string>())
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
                config.readFromFile();

                std::string updateToNewVersion;
                if(result.count("newversion")){
                    updateToNewVersion = result["newversion"].as<std::string>();
                }
                ApiRequest api(config.getConfig().host,config.getConfig().appName);
                auto [result1, appVersionContent] = updateToNewVersion.empty() ? api.GetCurrentAppVersion()
                                                                               : api.GetAppVersion(updateToNewVersion);
                if (!result1.getStatus()) {
                    std::cout<<"Cannot connect to serevr!\n";
                    return -1;
                }

                try {
                    auto appVersion = AppVersion(nlohmann::json::parse(appVersionContent));
                    updateToNewVersion = appVersion.getVersion().getVersionString();
                }catch(std::exception& e){
                    std::cout<<"Decode version info failed! Please check if the version given exist in the server!\n";
                    return -1;
                }

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

                        if(res.getStatus()){
                            auto cfg = config.getConfig();
                            cfg.localCurrentVersion = updateToNewVersion;
                            config.setConfig(cfg);
                            config.writeToFile();
                        }

                        break;
                    }
                    case UpdateMode::DifferencePackageUpdate: {
                        if (config.getConfig().localCurrentVersion == "x.x.x") {
                            std::cout<<"Invalid local current version!\n";
                        }

                        DifferencePackageUpdate update(config.getConfig().host,
                                                       config.getConfig().appName,
                                                       config.getConfig().appPath,
                                                       config.getConfig().downloadPath,
                                                       config.getConfig().localCurrentVersion,
                                                       updateToNewVersion);
                        auto res = update.execute();
                        std::cout << res.getErrorMessage() << "\n";

                        if(res.getStatus()){
                            auto cfg = config.getConfig();
                            cfg.localCurrentVersion = updateToNewVersion;
                            config.setConfig(cfg);
                            config.writeToFile();
                        }

                        break;
                    }
                    case UpdateMode::MultiVersionDifferencePackageUpdate: {
                        if (config.getConfig().localCurrentVersion == "x.x.x") {
                            std::cout<<"Invaid local current version!\n";
                        }

                        MultiVersionDifferencePackageUpdate update(config.getConfig().host,
                                                                   config.getConfig().appName,
                                                                   config.getConfig().appPath,
                                                                   config.getConfig().downloadPath,
                                                                   config.getConfig().localCurrentVersion,
                                                                   updateToNewVersion);
                        auto res = update.execute();
                        std::cout << res.getErrorMessage() << "\n";

                        if(res.getStatus()){
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
                                                config.getConfig().appPath,
                                                config.getConfig().downloadPath,
                                                updateToNewVersion);
                        auto res = update.execute();
                        std::cout << res.getErrorMessage() << "\n";

                        if(res.getStatus()){
                            auto cfg = config.getConfig();
                            cfg.localCurrentVersion = updateToNewVersion;
                            config.setConfig(cfg);
                            config.writeToFile();
                        }

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
