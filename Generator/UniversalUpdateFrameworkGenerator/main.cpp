#include "src/GeneratorAppEntry.h"
#include "cxxopts.hpp"
#include "src/UpdatePackageBuild.hpp"

// .\UniversalUpdateFrameworkGenerator.exe -m DifferencePackage -o D:\WorkSpace\UnrealEngine\1.1.0 -n D:\WorkSpace\UnrealEngine\1.2.0 -a Aurora -v 1.2.0 -b 1.1.0
// .\UniversalUpdateFrameworkGenerator.exe -m FullPackage -n D:\WorkSpace\UnrealEngine\1.2.0 -a Aurora -v 1.2.0

int main(int argc, char* argv[]) {
    try {
        cxxopts::Options options("UniversalUpdateFramework Generator",
                                 "A tool that help you generate appversion, appmanifest, apppackage file.");

        options.add_options()
                ("m,mode", "Generation Mode. (-m FullPackage/DifferencePackage)", cxxopts::value<std::string>())
                ("o,oldPath", "Old app path.", cxxopts::value<std::string>())
                ("n,newPath", "New app path.", cxxopts::value<std::string>())
                ("a,appname", "App name.", cxxopts::value<std::string>())
                ("c,channel", "Channel.", cxxopts::value<std::string>())
                ("o,platform", "Platform.", cxxopts::value<std::string>())
                ("v,appversion", "App version.", cxxopts::value<std::string>())
                ("b,appbeforeversion", "App before version.", cxxopts::value<std::string>())
                ("f,forceupdate", "Force to update.", cxxopts::value<std::string>())
                ("h,help", "Print usage.");
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (result.count("mode")) {
            std::string mode = result["mode"].as<std::string>();
            if (mode == "FullPackage") {
                UpdatePackageBuildInfo info;
                if (result.count("appname")) {
                    info.AppName = result["appname"].as<std::string>();
                }
                else {
                    std::cout << "Missing appname.\n";
                    return -1;
                }
                if (result.count("channel")) {
                    info.Channel = result["channel"].as<std::string>();
                }
                else {
                    std::cout << "Missing channel.\n";
                    return -1;
                }
                if (result.count("platform")) {
                    info.Platform = result["platform"].as<std::string>();
                }
                else {
                    std::cout << "Missing platform.\n";
                    return -1;
                }
                if (result.count("appversion")) {
                    info.AppCurrentVersion = result["appversion"].as<std::string>();
                }
                else {
                    std::cout << "Missing appversion.\n";
                    return -1;
                }
                if (result.count("newPath")) {
                    info.NewVersionPath = result["newPath"].as<std::string>();
                }
                else {
                    std::cout << "Missing newPath.\n";
                    return -1;
                }
                if (result.count("forceupdate")) {
                    info.ForceUpdate = std::stoi(result["forceupdate"].as<std::string>());
                }
                else {
                    std::cout << "Missing forceupdate.\n";
                    return -1;
                }

                UpdatePackageBuildInfo packageBuildInfo;
                packageBuildInfo.PackageMode = "FullPackage";
                packageBuildInfo.NewVersionPath = info.NewVersionPath;
                packageBuildInfo.AppName = info.AppName;
                packageBuildInfo.Channel = info.Channel;
                packageBuildInfo.Platform = info.Platform;
                packageBuildInfo.AppCurrentVersion = info.AppCurrentVersion;

                return generateFullPackage(packageBuildInfo);
            }
            else if (mode == "DifferencePackage") {
                UpdatePackageBuildInfo info;
                if (result.count("appname")) {
                    info.AppName = result["appname"].as<std::string>();
                }
                else {
                    std::cout << "Missing appname.\n";
                    return -1;
                }
                if (result.count("channel")) {
                    info.Channel = result["channel"].as<std::string>();
                }
                else {
                    std::cout << "Missing channel.\n";
                    return -1;
                }
                if (result.count("platform")) {
                    info.Platform = result["platform"].as<std::string>();
                }
                else {
                    std::cout << "Missing platform.\n";
                    return -1;
                }
                if (result.count("appversion")) {
                    info.AppCurrentVersion = result["appversion"].as<std::string>();
                }
                else {
                    std::cout << "Missing appversion.\n";
                    return -1;
                }
                if (result.count("appbeforeversion")) {
                    info.AppBeforeVersion = result["appbeforeversion"].as<std::string>();
                }
                else {
                    std::cout << "Missing appbeforeversion.\n";
                    return -1;
                }
                if (result.count("newPath")) {
                    info.NewVersionPath = result["newPath"].as<std::string>();
                }
                else {
                    std::cout << "Missing newPath.\n";
                    return -1;
                }
                if (result.count("oldPath")) {
                    info.OldVersionPath = result["oldPath"].as<std::string>();
                }
                else {
                    std::cout << "Missing oldPath.\n";
                    return -1;
                }
                if (result.count("forceupdate")) {
                    info.ForceUpdate = std::stoi(result["forceupdate"].as<std::string>());
                }
                else {
                    std::cout << "Missing forceupdate.\n";
                    return -1;
                }

                UpdatePackageBuildInfo packageBuildInfo;
                packageBuildInfo.PackageMode = "DifferencePackage";
                packageBuildInfo.OldVersionPath = info.OldVersionPath;
                packageBuildInfo.NewVersionPath = info.NewVersionPath;
                packageBuildInfo.AppName = info.AppName;
                packageBuildInfo.Channel = info.Channel;
                packageBuildInfo.Platform = info.Platform;
                packageBuildInfo.AppCurrentVersion = info.AppCurrentVersion;
                packageBuildInfo.AppBeforeVersion = info.AppBeforeVersion;

                return generateDifferencePackage(packageBuildInfo);
            }
            else {
                std::cout << "Unknow mode.\n";
            }
        }
        else {
            std::cout << "Missing mode.\n";
        }
        return 0;
    }
    catch (std::exception&e) {
        std::cerr << "Exception:" << e.what() << "\n";
    }
}
