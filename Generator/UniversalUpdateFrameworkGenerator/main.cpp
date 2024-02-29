#include "src/GeneratorAppEntry.h"
#include "cxxopts.hpp"

// .\UniversalUpdateFrameworkGenerator.exe -m DifferencePackage -o D:\WorkSpace\UnrealEngine\1.1.0 -n D:\WorkSpace\UnrealEngine\1.2.0 -a Aurora -v 1.2.0 -b 1.1.0
// .\UniversalUpdateFrameworkGenerator.exe -m FullPackage -n D:\WorkSpace\UnrealEngine\1.2.0 -a Aurora -v 1.2.0

int main(int argc, char *argv[]) {
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
                ("h,help", "Print usage.");
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (result.count("mode")) {
            std::string mode = result["mode"].as<std::string>();
            if (mode == "FullPackage") {
                PackageBuildInfo info;
                if (result.count("appname")) {
                    info.appname = result["appname"].as<std::string>();
                } else {
                    std::cout << "Missing appname.\n";
                    return -1;
                }
                if (result.count("channel")) {
                    info.channel = result["channel"].as<std::string>();
                } else {
                    std::cout << "Missing channel.\n";
                    return -1;
                }
                if (result.count("platform")) {
                    info.platform = result["platform"].as<std::string>();
                } else {
                    std::cout << "Missing platform.\n";
                    return -1;
                }
                if (result.count("appversion")) {
                    info.appversion = result["appversion"].as<std::string>();
                } else {
                    std::cout << "Missing appversion.\n";
                    return -1;
                }
                if (result.count("newPath")) {
                    info.newPath = result["newPath"].as<std::string>();
                } else {
                    std::cout << "Missing newPath.\n";
                    return -1;
                }



                PackageBuildInfo packageBuildInfo;
                packageBuildInfo.PackageMode = "FullPackage";
                packageBuildInfo.newPath = info.newPath;
                packageBuildInfo.appname = info.appname;
                packageBuildInfo.channel = info.channel;
                packageBuildInfo.platform = info.platform;
                packageBuildInfo.appversion = info.appversion;

                return generateFullPackage(packageBuildInfo);
            } else if (mode == "DifferencePackage") {
                PackageBuildInfo info;
                if (result.count("appname")) {
                    info.appname = result["appname"].as<std::string>();
                } else {
                    std::cout << "Missing appname.\n";
                    return -1;
                }
                if (result.count("channel")) {
                    info.channel = result["channel"].as<std::string>();
                } else {
                    std::cout << "Missing channel.\n";
                    return -1;
                }
                if (result.count("platform")) {
                    info.platform = result["platform"].as<std::string>();
                } else {
                    std::cout << "Missing platform.\n";
                    return -1;
                }
                if (result.count("appversion")) {
                    info.appversion = result["appversion"].as<std::string>();
                } else {
                    std::cout << "Missing appversion.\n";
                    return -1;
                }
                if (result.count("appbeforeversion")) {
                    info.appbeforeversion = result["appbeforeversion"].as<std::string>();
                } else {
                    std::cout << "Missing appbeforeversion.\n";
                    return -1;
                }
                if (result.count("newPath")) {
                    info.newPath = result["newPath"].as<std::string>();
                } else {
                    std::cout << "Missing newPath.\n";
                    return -1;
                }
                if (result.count("oldPath")) {
                    info.oldPath = result["oldPath"].as<std::string>();
                } else {
                    std::cout << "Missing oldPath.\n";
                    return -1;
                }

                PackageBuildInfo packageBuildInfo;
                packageBuildInfo.PackageMode = "DifferencePackage";
                packageBuildInfo.oldPath = info.oldPath;
                packageBuildInfo.newPath = info.newPath;
                packageBuildInfo.appname = info.appname;
                packageBuildInfo.channel = info.channel;
                packageBuildInfo.platform = info.platform;
                packageBuildInfo.appversion = info.appversion;
                packageBuildInfo.appbeforeversion = info.appbeforeversion;

                return generateDifferencePackage(packageBuildInfo);
            } else {
                std::cout << "Unknow mode.\n";
            }
        } else {
            std::cout << "Missing mode.\n";
        }
        return 0;
    } catch (std::exception &e) {
        std::cerr << "Exception:" << e.what() << "\n";
    }
}
