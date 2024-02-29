#pragma once

#include "Defination.hpp"
#include <filesystem>
#include <iostream>
#include "FullPackageGenerator.hpp"
#include "DifferencePackageGenerator.hpp"

inline int generateFullPackage(PackageBuildInfo &packageBuildInfo) {
    if (!std::filesystem::exists(packageBuildInfo.newPath)) {
        std::cout << "newPath:" << packageBuildInfo.newPath << " not exists.\n";
        return -1;
    }

    std::filesystem::path appdataFolder(packageBuildInfo.appname);
    if (!std::filesystem::exists(appdataFolder)) {
        std::filesystem::create_directories(appdataFolder);
    }

    std::filesystem::path appChannelPlatformFolder = appdataFolder / packageBuildInfo.channel / packageBuildInfo.platform;
    if (!std::filesystem::exists(appChannelPlatformFolder)) {
        std::filesystem::create_directories(appChannelPlatformFolder);
    }

    std::filesystem::path appversionFolder = appChannelPlatformFolder / packageBuildInfo.appversion;
    if (std::filesystem::exists(appversionFolder)) {
        std::filesystem::remove_all(appversionFolder);
    }
    std::filesystem::create_directories(appversionFolder);

    if (!generateFullPackageAppVersionFile(appversionFolder, packageBuildInfo)) {
        std::cout << "Error on generate appversion.json.\n";
        return -1;
    }

    if (!generateFullPackageAppManifestFile(appversionFolder, packageBuildInfo)) {
        std::cout << "Error on generate appmanifest.json.\n";
        return -1;
    }

    if (!generateFullPackageManifestFile(appversionFolder, packageBuildInfo)) {
        std::cout << "Error on generate fullpackage.\n";
        return -1;
    }

    std::cout << "Full package finished!\n";
    return 0;
}

inline int generateDifferencePackage(PackageBuildInfo &packageBuildInfo) {
    if (!std::filesystem::exists(packageBuildInfo.newPath)) {
        std::cout << "newPath:" << packageBuildInfo.newPath << " not exists.\n";
        return -1;
    }

    if (!std::filesystem::exists(packageBuildInfo.oldPath)) {
        std::cout << "oldPath:" << packageBuildInfo.oldPath << " not exists.\n";
        return -1;
    }

    std::filesystem::path appdataFolder(packageBuildInfo.appname);
    if (!std::filesystem::exists(appdataFolder)) {
        std::filesystem::create_directories(appdataFolder);
    }

    std::filesystem::path appChannelPlatformFolder = appdataFolder / packageBuildInfo.channel / packageBuildInfo.platform;
    if (!std::filesystem::exists(appChannelPlatformFolder)) {
        std::filesystem::create_directories(appChannelPlatformFolder);
    }

    std::filesystem::path appversionFolder = appChannelPlatformFolder / packageBuildInfo.appversion;
    if (!std::filesystem::exists(appversionFolder)) {
        std::filesystem::create_directories(appversionFolder);
    }

    if (!generateDifferencePackageManifestFile(appversionFolder, packageBuildInfo)) {
        std::cout << "Error on generate differencepackage.\n";
        return -1;
    }

    std::cout << "Difference package finished!\n";
    return 0;
}


