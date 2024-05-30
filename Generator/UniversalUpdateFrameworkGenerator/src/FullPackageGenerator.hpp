#pragma once

#include "Defination.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include "md5.h"
#include <format>
#include "FilePermission.hpp"
#include "AppVersion.hpp"
#include "UpdatePackageBuild.hpp"
#include "AppManifest.hpp"
#include "FullPackageManifest.hpp"
#include "util.hpp"

inline bool generateFullPackageAppVersionFile(std::filesystem::path appversionFolder, UpdatePackageBuildInfo info) {
    std::filesystem::path appverionFilePath = appversionFolder / "appversion.json";
    std::ofstream appversionFileStream;
    appversionFileStream.open(appverionFilePath);
    if (!appversionFileStream.is_open()) {
        std::cout << "Error on open " << appverionFilePath.string() << "\n";
        return false;
    }

    AppVersionInfo appVersionInfo;
    appVersionInfo.AppName = info.AppName;
    appVersionInfo.AppVersion = info.AppCurrentVersion;

    nlohmann::json appVersionInfoJson = appVersionInfo;

    appversionFileStream << appVersionInfoJson.dump(4);
    appversionFileStream.close();

    return true;
}


inline bool generateFullPackageAppManifestFile(std::filesystem::path appversionFolder, UpdatePackageBuildInfo info) {
    std::filesystem::path appmanifestFilePath = appversionFolder / "appmanifest.json";
    std::ofstream appmanifestFileStream;
    appmanifestFileStream.open(appmanifestFilePath);
    if (!appmanifestFileStream.is_open()) {
        std::cout << "Error on open " << appmanifestFilePath.string() << "\n";
        return false;
    }

    AppManifestInfo appMainifestInfo;
    appMainifestInfo.AppName = info.AppName;
    appMainifestInfo.AppVersion = info.AppCurrentVersion;
    appMainifestInfo.UpdateReadMe = info.UpdateReadMe;

    std::filesystem::path newPath(info.NewVersionPath);
    for (auto&directoryEntry: std::filesystem::recursive_directory_iterator(newPath)) {
        if (!directoryEntry.is_directory()) {
            auto relativePath = std::filesystem::relative(directoryEntry.path(), std::filesystem::path(info.NewVersionPath));
            FileManifestInfo manifest;
            manifest.FileName = relativePath.filename().string();
            manifest.FilePath = util::subreplace(relativePath.string(), "\\", "/");
            manifest.Md5 = calcFileMd5(directoryEntry.path().string());

            FilePermission filePermission(directoryEntry.path());
            manifest.Permission = filePermission.ReadPermission();

            appMainifestInfo.Manifests.push_back(manifest);
        }
    }

    nlohmann::json appMainifestInfoJson = appMainifestInfo;

    appmanifestFileStream << appMainifestInfoJson.dump(4);
    appmanifestFileStream.close();

    return true;
}

inline bool generateFullPackageManifestFile(std::filesystem::path appversionFolder, UpdatePackageBuildInfo info) {
    std::filesystem::path fullPackageFile = appversionFolder / "appfullpackage";
    std::cout << fullPackageFile.string() << "\n";
    std::string shellCommand = std::format(R"({} -c-zlib "" "{}" "{}")", hdiffzExecuable, info.NewVersionPath,
                                           fullPackageFile.string());
    std::cout << shellCommand << "\n";

    system(shellCommand.c_str());

    std::filesystem::path appfullpackagemanifestFile = appversionFolder / "appfullpackagemanifest.json";
    if (std::filesystem::exists(appfullpackagemanifestFile)) {
        std::filesystem::remove(appfullpackagemanifestFile);
    }

    std::ofstream appfullpackagemanifestFileStream;
    appfullpackagemanifestFileStream.open(appfullpackagemanifestFile);
    if (!appfullpackagemanifestFileStream.is_open()) {
        std::cout << "Error on open " << appfullpackagemanifestFile.string() << "\n";
        return false;
    }

    FullPackageManifestInfo fullPackageManifestInfo;

    fullPackageManifestInfo.AppName = info.AppName;
    fullPackageManifestInfo.AppVersion = info.AppCurrentVersion;
    fullPackageManifestInfo.FileName = "appfullpackage";
    fullPackageManifestInfo.Md5 = calcFileMd5(fullPackageFile.string());

    nlohmann::json appfullpackagemanifestJson = fullPackageManifestInfo;

    appfullpackagemanifestFileStream << appfullpackagemanifestJson.dump(4);
    appfullpackagemanifestFileStream.close();

    std::filesystem::path fullpackageFolderPath = appversionFolder / "fullpackage";

    std::filesystem::copy(info.NewVersionPath, fullpackageFolderPath, std::filesystem::copy_options::recursive);

    return true;
}
