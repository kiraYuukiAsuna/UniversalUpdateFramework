#pragma once

#include "FilePermission.hpp"
#include "VersionConfig.hpp"
#include "md5.h"

AppVersionBriefInfo genereateUpdaterVersionInfo(const std::string& version, const std::string& updaterFullPackagePath) {
    AppVersionBriefInfo appVersionBriefInfo;
    appVersionBriefInfo.version = version;

    for(const auto& file : std::filesystem::directory_iterator(updaterFullPackagePath)) {
        if (!file.is_directory()) {
            auto relativePath = std::filesystem::relative(file.path(), std::filesystem::path(updaterFullPackagePath));

            FileManifestInfo manifest;
            manifest.FileName = relativePath.filename().string();
            manifest.FilePath = relativePath.string();
            manifest.Md5 = calcFileMd5(file.path().string());

            FilePermission filePermission(file.path());
            manifest.Permission = filePermission.ReadPermission();

            appVersionBriefInfo.filemanifests.push_back(manifest);
        }
    }
    return appVersionBriefInfo;
}
