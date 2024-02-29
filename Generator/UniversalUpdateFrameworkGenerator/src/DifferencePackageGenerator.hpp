#pragma once

#include "Defination.hpp"
#include <filesystem>
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include "md5.h"

inline bool generateDifferencePackageManifestFile(std::filesystem::path appversionFolder, PackageBuildInfo info) {
    std::filesystem::path appdifferencepackagemanifestFile = appversionFolder / "appdifferencepackagemanifest.json";
    std::ofstream appdifferencepackagemanifestFileFileStream;
    appdifferencepackagemanifestFileFileStream.open(appdifferencepackagemanifestFile);
    if (!appdifferencepackagemanifestFileFileStream.is_open()) {
        std::cout << "Error on open " << appdifferencepackagemanifestFile.string() << "\n";
        return false;
    }

    nlohmann::json appfullpackagemanifestJson;
    appfullpackagemanifestJson["appname"] = info.appname;
    appfullpackagemanifestJson["appversion"] = info.appversion;
    appfullpackagemanifestJson["filename"] = "appdifferencepackage";
    appfullpackagemanifestJson["appbeforeversion"] = info.appbeforeversion;

    appfullpackagemanifestJson["diff_updatefiles"];
    appfullpackagemanifestJson["diff_newfiles"];
    appfullpackagemanifestJson["diff_deletedfiles"];

    std::vector<std::string> newFiles;
    std::vector<std::string> oldFiles;
    for (auto &directoryEntry: std::filesystem::recursive_directory_iterator(info.newPath)) {
        if (!directoryEntry.is_directory()) {
            auto relativePath = std::filesystem::relative(directoryEntry.path().string(),
                                                          std::filesystem::path(info.newPath));
            newFiles.push_back(relativePath.string());
        }
    }

    for (auto &directoryEntry: std::filesystem::recursive_directory_iterator(info.oldPath)) {
        if (!directoryEntry.is_directory()) {
            auto relativePath = std::filesystem::relative(directoryEntry.path().string(),
                                                          std::filesystem::path(info.oldPath));
            oldFiles.push_back(relativePath.string());
        }
    }
    std::vector<std::string> same_elements;
    std::vector<std::string> new_elements;
    std::vector<std::string> deleted_elements;

    // 对 newFiles 和 oldFiles 进行排序
    sort(newFiles.begin(), newFiles.end());
    sort(oldFiles.begin(), oldFiles.end());

    // 找到两个容器中相同的元素
    try {
        std::set_intersection(newFiles.begin(), newFiles.end(), oldFiles.begin(), oldFiles.end(),
                              std::back_inserter(same_elements));

        // 找到newFiles比oldFiles新增的元素
        std::set_difference(newFiles.begin(), newFiles.end(), oldFiles.begin(), oldFiles.end(),
                            std::back_inserter(new_elements));

        // 找到newFiles比oldFiles缺少的元素
        std::set_difference(oldFiles.begin(), oldFiles.end(), newFiles.begin(), newFiles.end(),
                            std::back_inserter(deleted_elements));
    } catch (std::exception &e) {
        std::cerr << "Exception in :" << e.what() << "\n";
    }

    std::filesystem::path differencepackageFolderPath = appversionFolder / "differencepackage";
    if (std::filesystem::exists(differencepackageFolderPath)) {
        std::filesystem::remove_all(differencepackageFolderPath);
    }
    std::filesystem::create_directories(differencepackageFolderPath);

    // 输出结果
    std::cout << "Same elements (Update): ";
    for (const auto &element: same_elements) {
        std::cout << element << "\n";
        appfullpackagemanifestJson["diff_updatefiles"].push_back(element);

        std::filesystem::path oldPath = std::filesystem::path(info.oldPath) / element;
        std::filesystem::path newPath = std::filesystem::path(info.newPath) / element;
        std::filesystem::path outputPath = differencepackageFolderPath / element;

        auto a = std::hash<std::string>{}(outputPath.string());
        auto out = outputPath.parent_path() / std::to_string(a);

        if (!std::filesystem::exists(out.parent_path())) {
            std::filesystem::create_directories(out.parent_path());
        }

        std::string shellCommand = std::format(R"(hdiffz.exe "{}" "{}" "{}")", oldPath.string(), newPath.string(), outputPath.string());

        system(shellCommand.c_str());
    }
    std::cout << std::endl;

    std::cout << "New elements in newFiles: ";
    for (const auto &element: new_elements) {
        std::cout << element << "\n";
        appfullpackagemanifestJson["diff_newfiles"].push_back(element);
        std::filesystem::path from = std::filesystem::path(info.newPath) / element;
        std::filesystem::path to = differencepackageFolderPath / element;

        if (!std::filesystem::exists(to.parent_path())) {
            std::filesystem::create_directories(to.parent_path());
        }

        std::filesystem::copy_file(from, to);
    }
    std::cout << std::endl;

    std::cout << "Deleted elements in newFiles: ";
    for (const auto &element: deleted_elements) {
        std::cout << element << "\n";
        appfullpackagemanifestJson["diff_deletedfiles"].push_back(element);
    }
    std::cout << std::endl;

    std::filesystem::path differencePackageFile = appversionFolder / "appdifferencepackage";
    if (std::filesystem::exists(differencePackageFile)) {
        std::filesystem::remove(differencePackageFile);
    }

    std::string shellCommand = std::format(R"(hdiffz.exe -c-zlib "" "{}" "{}")", differencepackageFolderPath.string(),
                                           differencePackageFile.string());

    system(shellCommand.c_str());

    appfullpackagemanifestJson["md5"] = calcFileMd5(differencePackageFile.string());

    appdifferencepackagemanifestFileFileStream << appfullpackagemanifestJson;
    appdifferencepackagemanifestFileFileStream.close();

    return true;
}
