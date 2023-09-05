#include <iostream>
#include <fstream>
#include <format>
#include "json.hpp"
#include "cxxopts.hpp"
#include "md5.h"

struct PackageBuildInfo{
    std::string appname;
    std::string appversion;
    std::string appbeforeversion;
    std::string newPath;
    std::string oldPath;
};

bool generateFullPackageAppVersionFile(std::filesystem::path appversionFolder, PackageBuildInfo info){
    std::filesystem::path appverionFilePath = appversionFolder / "appversion.json";
    std::ofstream appversionFileStream;
    appversionFileStream.open(appverionFilePath);
    if(!appversionFileStream.is_open()){
        std::cout<<"Error on open "<<appverionFilePath.string()<<"\n";
        return false;
    }

    nlohmann::json appversionJson;
    appversionJson["appname"] = info.appname;
    appversionJson["appversion"] = info.appversion;

    appversionFileStream << appversionJson;
    appversionFileStream.close();

    return true;
}

std::string calcMd5(std::string filePath){
    MD5 md5;
    ifstream in(filePath, std::ios::binary);
    if (!in) {
        std::cout << "Error on open " << filePath << " to calc md5\n";
        throw std::runtime_error("Open file error!");
    }
    md5.update(in);

    return md5.toString();
}

bool generateFullPackageAppManifestFile(std::filesystem::path appversionFolder, PackageBuildInfo info){
    std::filesystem::path appmanifestFilePath = appversionFolder / "appmanifest.json";
    std::ofstream appmanifestFileStream;
    appmanifestFileStream.open(appmanifestFilePath);
    if(!appmanifestFileStream.is_open()){
        std::cout<<"Error on open "<<appmanifestFilePath.string()<<"\n";
        return false;
    }

    nlohmann::json appmanifestJson;
    appmanifestJson["appname"] = info.appname;
    appmanifestJson["appversion"] = info.appversion;
    appmanifestJson["manifest"];

    std::filesystem::path newPath(info.newPath);
    for(auto& directoryEntry : std::filesystem::recursive_directory_iterator(newPath)){
        if(!directoryEntry.is_directory()) {
            auto relativePath = std::filesystem::relative(directoryEntry.path(), std::filesystem::path(info.newPath));
            nlohmann::json fileInfo;
            fileInfo["filepath"] = relativePath.string();
            fileInfo["filename"] = relativePath.filename();
            fileInfo["md5"] = calcMd5(directoryEntry.path().string());

            appmanifestJson["manifest"].push_back(fileInfo);
        }
    }

    appmanifestFileStream << appmanifestJson;
    appmanifestFileStream.close();

    return true;
}

bool generateFullPackageManifestFile(std::filesystem::path appversionFolder, PackageBuildInfo info) {
    std::filesystem::path fullPackageFile = appversionFolder / "appfullpackage";
    std::cout<<fullPackageFile.string()<<"\n";
    std::string shellCommand = std::format(R"(hdiffz.exe -c-zlib "" {} {})", info.newPath, fullPackageFile.string());
    std::cout<<shellCommand<<"\n";

    system(shellCommand.c_str());

    std::filesystem::path appfullpackagemanifestFile = appversionFolder / "appfullpackagemanifest.json";
    if(std::filesystem::exists(appfullpackagemanifestFile)){
        std::filesystem::remove(appfullpackagemanifestFile);
    }

    std::ofstream appfullpackagemanifestFileStream;
    appfullpackagemanifestFileStream.open(appfullpackagemanifestFile);
    if(!appfullpackagemanifestFileStream.is_open()){
        std::cout<<"Error on open "<<appfullpackagemanifestFile.string()<<"\n";
        return false;
    }

    nlohmann::json appfullpackagemanifestJson;
    appfullpackagemanifestJson["appname"] = info.appname;
    appfullpackagemanifestJson["appversion"] = info.appversion;
    appfullpackagemanifestJson["filename"] = "appfullpackage";
    appfullpackagemanifestJson["md5"] = calcMd5(fullPackageFile.string());

    appfullpackagemanifestFileStream << appfullpackagemanifestJson;
    appfullpackagemanifestFileStream.close();

    std::filesystem::path fullpackageFolderPath = appversionFolder / "fullpackage";

    std::filesystem::copy(info.newPath, fullpackageFolderPath, std::filesystem::copy_options::recursive);

    return true;
}

bool generateDifferencePackageManifestFile(std::filesystem::path appversionFolder, PackageBuildInfo info) {
    std::filesystem::path appdifferencepackagemanifestFile = appversionFolder / "appdifferencepackagemanifest.json";
    std::ofstream appdifferencepackagemanifestFileFileStream;
    appdifferencepackagemanifestFileFileStream.open(appdifferencepackagemanifestFile);
    if(!appdifferencepackagemanifestFileFileStream.is_open()){
        std::cout<<"Error on open "<<appdifferencepackagemanifestFile.string()<<"\n";
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
    for(auto& directoryEntry : std::filesystem::recursive_directory_iterator(info.newPath)){
        if(!directoryEntry.is_directory()){
            auto relativePath = std::filesystem::relative(directoryEntry.path().string(), std::filesystem::path(info.newPath));
            newFiles.push_back(relativePath.string());
        }
    }

    for(auto& directoryEntry : std::filesystem::recursive_directory_iterator(info.oldPath)){
        if(!directoryEntry.is_directory()){
            auto relativePath = std::filesystem::relative(directoryEntry.path().string(), std::filesystem::path(info.oldPath));
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
    }catch (std::exception& e){
        std::cerr<<"Exception in :" << e.what()<<"\n";
    }

    std::filesystem::path differencepackageFolderPath = appversionFolder / "differencepackage";
    if(std::filesystem::exists(differencepackageFolderPath)){
        std::filesystem::remove_all(differencepackageFolderPath);
    }
    std::filesystem::create_directories(differencepackageFolderPath);

    // 输出结果
    std::cout << "Same elements (Update): ";
    for (const auto& element : same_elements) {
        std::cout << element << "\n";
        appfullpackagemanifestJson["diff_updatefiles"].push_back(element);

        std::filesystem::path oldPath = std::filesystem::path(info.oldPath) / element;
        std::filesystem::path newPath = std::filesystem::path(info.newPath) / element;
        std::filesystem::path outputPath = differencepackageFolderPath / element;

        auto a = std::hash<std::string>{}(outputPath.string());
        auto out = outputPath.parent_path() / std::to_string(a);

        if(!std::filesystem::exists(out.parent_path())){
            std::filesystem::create_directories(out.parent_path());
        }

        std::string shellCommand = std::format(R"(hdiffz.exe {} {} {})", oldPath.string(), newPath.string(),outputPath.string());

        system(shellCommand.c_str());
    }
    std::cout << std::endl;

    std::cout << "New elements in newFiles: ";
    for (const auto& element : new_elements) {
        std::cout << element << "\n";
        appfullpackagemanifestJson["diff_newfiles"].push_back(element);
        std::filesystem::path from = std::filesystem::path(info.newPath) / element;
        std::filesystem::path to = differencepackageFolderPath / element;

        if(!std::filesystem::exists(to.parent_path())){
            std::filesystem::create_directories(to.parent_path());
        }

        std::filesystem::copy_file(from, to);
    }
    std::cout << std::endl;

    std::cout << "Deleted elements in newFiles: ";
    for (const auto& element : deleted_elements) {
        std::cout << element << "\n";
        appfullpackagemanifestJson["diff_deletedfiles"].push_back(element);
    }
    std::cout << std::endl;

    appdifferencepackagemanifestFileFileStream << appfullpackagemanifestJson;
    appdifferencepackagemanifestFileFileStream.close();

    std::filesystem::path differencePackageFile = appversionFolder / "appdifferencepackage";
    if(std::filesystem::exists(differencePackageFile)){
        std::filesystem::remove(differencePackageFile);
    }

    std::string shellCommand = std::format(R"(hdiffz.exe -c-zlib "" {} {})", differencepackageFolderPath.string(), differencePackageFile.string());

    system(shellCommand.c_str());

    appfullpackagemanifestJson["md5"] = calcMd5(differencePackageFile.string());

    return true;
}

int main(int argc, char* argv[]) {
    try {
        cxxopts::Options options("UniversalUpdateFramework Generator",
                                 "A tool that help you generate appversion, appmanifest, apppackage file.");

        options.add_options()
                ("m,mode", "Generation Mode. (-m FullPackage/DifferencePackage)", cxxopts::value<std::string>())
                ("o,oldPath", "Old app path.", cxxopts::value<std::string>())
                ("n,newPath", "New app path.", cxxopts::value<std::string>())
                ("a,appname", "App name.", cxxopts::value<std::string>())
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

                if (!std::filesystem::exists(info.newPath)) {
                    std::cout << "newPath:" << info.newPath << " not exists.\n";
                    return -1;
                }

                std::filesystem::path appdataFolder(info.appname);
                if (!std::filesystem::exists(appdataFolder)) {
                    std::filesystem::create_directories(appdataFolder);
                }

                std::filesystem::path appversionFolder = appdataFolder / info.appversion;
                if (std::filesystem::exists(appversionFolder)) {
                    std::filesystem::remove_all(appversionFolder);
                }
                std::filesystem::create_directories(appversionFolder);

                if (!generateFullPackageAppVersionFile(appversionFolder, info)) {
                    std::cout << "Error on generate appversion.json.\n";
                    return -1;
                }

                if (!generateFullPackageAppManifestFile(appversionFolder, info)) {
                    std::cout << "Error on generate appmanifest.json.\n";
                    return -1;
                }

                if(!generateFullPackageManifestFile(appversionFolder, info)){
                    std::cout << "Error on generate fullpackage.\n";
                    return -1;
                }

                std::cout<<"Full package finished!\n";

                return 0;
            } else if (mode == "DifferencePackage") {
                PackageBuildInfo info;
                if (result.count("appname")) {
                    info.appname = result["appname"].as<std::string>();
                } else {
                    std::cout << "Missing appname.\n";
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

                if (!std::filesystem::exists(info.newPath)) {
                    std::cout << "newPath:" << info.newPath << " not exists.\n";
                    return -1;
                }

                if (!std::filesystem::exists(info.oldPath)) {
                    std::cout << "oldPath:" << info.oldPath << " not exists.\n";
                    return -1;
                }

                std::filesystem::path appdataFolder(info.appname);
                if (!std::filesystem::exists(appdataFolder)) {
                    std::filesystem::create_directories(appdataFolder);
                }

                std::filesystem::path appversionFolder = appdataFolder / info.appversion;
                if (!std::filesystem::exists(appversionFolder)) {
                    std::filesystem::create_directories(appversionFolder);
                }

                if(!generateDifferencePackageManifestFile(appversionFolder, info)){
                    std::cout << "Error on generate differencepackage.\n";
                    return -1;
                }

                std::cout<<"Difference package finished!\n";

                return 0;
            } else {
                std::cout << "Unknow mode.\n";
            }
        } else {
            std::cout << "Missing mode.\n";
        }
        return 0;
    }catch(std::exception& e){
        std::cerr<<"Exception:" << e.what()<<"\n";
    }
}
