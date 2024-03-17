#pragma once

#include "Defination.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include "md5.h"
#include <format>

inline bool generateFullPackageAppVersionFile(std::filesystem::path appversionFolder, PackageBuildInfo info) {
    std::filesystem::path appverionFilePath = appversionFolder / "appversion.json";
    std::ofstream appversionFileStream;
    appversionFileStream.open(appverionFilePath);
    if (!appversionFileStream.is_open()) {
        std::cout << "Error on open " << appverionFilePath.string() << "\n";
        return false;
    }

    nlohmann::json appversionJson;
    appversionJson["appname"] = info.appname;
    appversionJson["appversion"] = info.appversion;

    appversionFileStream << appversionJson;
    appversionFileStream.close();

    return true;
}


inline bool generateFullPackageAppManifestFile(std::filesystem::path appversionFolder, PackageBuildInfo info) {
    std::filesystem::path appmanifestFilePath = appversionFolder / "appmanifest.json";
    std::ofstream appmanifestFileStream;
    appmanifestFileStream.open(appmanifestFilePath);
    if (!appmanifestFileStream.is_open()) {
        std::cout << "Error on open " << appmanifestFilePath.string() << "\n";
        return false;
    }

    nlohmann::json appmanifestJson;
    appmanifestJson["appname"] = info.appname;
    appmanifestJson["appversion"] = info.appversion;
    appmanifestJson["manifest"];

    std::filesystem::path newPath(info.newPath);
    for (auto&directoryEntry: std::filesystem::recursive_directory_iterator(newPath)) {
        if (!directoryEntry.is_directory()) {
            auto relativePath = std::filesystem::relative(directoryEntry.path(), std::filesystem::path(info.newPath));
            nlohmann::json fileInfo;
            fileInfo["filepath"] = relativePath.string();
            fileInfo["filename"] = relativePath.filename();
            fileInfo["md5"] = calcFileMd5(directoryEntry.path().string());

            appmanifestJson["manifest"].push_back(fileInfo);
        }
    }

    appmanifestFileStream << appmanifestJson;
    appmanifestFileStream.close();

    return true;
}

inline bool generateFullPackageManifestFile(std::filesystem::path appversionFolder, PackageBuildInfo info) {
    std::filesystem::path fullPackageFile = appversionFolder / "appfullpackage";
    std::cout << fullPackageFile.string() << "\n";
    std::string shellCommand = std::format(R"({} -c-zlib "" "{}" "{}")", hdiffzExecuable, info.newPath,
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

    nlohmann::json appfullpackagemanifestJson;
    appfullpackagemanifestJson["appname"] = info.appname;
    appfullpackagemanifestJson["appversion"] = info.appversion;
    appfullpackagemanifestJson["filename"] = "appfullpackage";
    appfullpackagemanifestJson["md5"] = calcFileMd5(fullPackageFile.string());

    appfullpackagemanifestFileStream << appfullpackagemanifestJson;
    appfullpackagemanifestFileStream.close();

    std::filesystem::path fullpackageFolderPath = appversionFolder / "fullpackage";

    std::filesystem::copy(info.newPath, fullpackageFolderPath, std::filesystem::copy_options::recursive);

    return true;
}
