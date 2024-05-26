#pragma once

#include "Defination.hpp"
#include <filesystem>
#include <iostream>
#include "FullPackageGenerator.hpp"
#include "DifferencePackageGenerator.hpp"

inline int generateFullPackage(UpdatePackageBuildInfo &packageBuildInfo) {
    if (!std::filesystem::exists(packageBuildInfo.NewVersionPath)) {
        std::cout << "NewVersionPath:" << packageBuildInfo.NewVersionPath << " not exists.\n";
        return -1;
    }

    std::filesystem::path appdataFolder(packageBuildInfo.AppName);
    if (!std::filesystem::exists(appdataFolder)) {
        std::filesystem::create_directories(appdataFolder);
    }

    std::filesystem::path appChannelPlatformFolder = appdataFolder / packageBuildInfo.Channel / packageBuildInfo.Platform;
    if (!std::filesystem::exists(appChannelPlatformFolder)) {
        std::filesystem::create_directories(appChannelPlatformFolder);
    }

    std::filesystem::path appversionFolder = appChannelPlatformFolder / packageBuildInfo.AppCurrentVersion;
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

inline int generateDifferencePackage(UpdatePackageBuildInfo &packageBuildInfo) {
    if (!std::filesystem::exists(packageBuildInfo.NewVersionPath)) {
        std::cout << "NewVersionPath:" << packageBuildInfo.NewVersionPath << " not exists.\n";
        return -1;
    }

    if (!std::filesystem::exists(packageBuildInfo.OldVersionPath)) {
        std::cout << "OldVersionPath:" << packageBuildInfo.OldVersionPath << " not exists.\n";
        return -1;
    }

    std::filesystem::path appdataFolder(packageBuildInfo.AppName);
    if (!std::filesystem::exists(appdataFolder)) {
        std::filesystem::create_directories(appdataFolder);
    }

    std::filesystem::path appChannelPlatformFolder = appdataFolder / packageBuildInfo.Channel / packageBuildInfo.Platform;
    if (!std::filesystem::exists(appChannelPlatformFolder)) {
        std::filesystem::create_directories(appChannelPlatformFolder);
    }

    std::filesystem::path appversionFolder = appChannelPlatformFolder / packageBuildInfo.AppCurrentVersion;
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


