#include "UpdateLogic/UpdateClientAppEntry.hpp"

inline int handleArgument(int argc, char *argv[]) {
    try {
        cxxopts::Options options("UniversalUpdateFramework Client",
                                 "A tool that help you update your app.");

        options.add_options()
                ("m,mode",
                 "Update Mode. (-m FullPackageUpdate/DifferencePackageUpdate/MultiVersionDifferencePackageUpdate/DifferenceUpdate)",
                 cxxopts::value<std::string>())
                ("c,config", "Config file path.", cxxopts::value<std::string>())
                ("v,newversion", "New version to update. If not given by default will update to the newest version.",
                 cxxopts::value<std::string>())
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
                UpdateConfigIo config(configFilePath);
                config.readFromFile();

                std::string updateToNewVersion;
                if (result.count("newversion")) {
                    updateToNewVersion = result["newversion"].as<std::string>();
                }
                ApiRequest api(config.getConfig().host, config.getConfig().appName, config.getConfig().channel,
                               config.getConfig().platform);
                auto [result1, appVersionContent] = updateToNewVersion.empty() ? api.GetCurrentAppVersion()
                                                                               : api.GetAppVersion(updateToNewVersion);
                if (!result1.getStatus()) {
                    std::cout << "Cannot connect to serevr!\n";
                    return -1;
                }

                try {
                    auto appVersion = AppVersion(nlohmann::json::parse(appVersionContent));
                    updateToNewVersion = appVersion.getVersion().getVersionString();
                } catch (std::exception &e) {
                    std::cout << "Decode version info failed! Please check if the version given exist in the server!\n";
                    return -1;
                }

                handleUpdateMode(mode, config, updateToNewVersion);
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

int main(int argc, char *argv[]) {
    return handleArgument(argc, argv);
}
