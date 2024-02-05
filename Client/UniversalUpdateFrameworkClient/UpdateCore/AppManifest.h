#pragma once

#include <string>
#include "json.hpp"
#include "util.hpp"
#include "AppVersion.h"

struct FileManifestData {
    std::string fileName;
    std::string filePath;
    std::string md5;
};

class AppManifest {
public:
    AppManifest(nlohmann::json appManifestContent)
            : m_AppVersion(appManifestContent) {
        if (appManifestContent.contains("manifest")) {
            if (appManifestContent["manifest"].is_array()) {
                for (auto &fileManifest: appManifestContent["manifest"]) {
                    FileManifestData data;
                    if (fileManifest.contains("filename")) {
                        data.fileName = fileManifest["filename"];
                    } else {
                        throw std::runtime_error("Manifest Json Error!");
                    }
                    if (fileManifest.contains("filepath")) {
                        data.filePath = fileManifest["filepath"];
                    } else {
                        throw std::runtime_error("Manifest Json Error!");
                    }
                    if (fileManifest.contains("md5")) {
                        data.md5 = fileManifest["md5"];
                    } else {
                        throw std::runtime_error("Manifest Json Error!");
                    }
                    m_ManifestData.push_back(data);
                }
            } else {
                throw std::runtime_error("Manifest Json Error!");
            }
        } else {
            throw std::runtime_error("Manifest Json Error!");
        }
    }

    [[nodiscard]] std::vector<FileManifestData> getManifestData() const {
        return m_ManifestData;
    }

    [[nodiscard]] FileManifestData getFileManifest(std::string filePath) const {
        for (auto &fileManifest: m_ManifestData) {
            if (fileManifest.filePath == filePath) {
                return fileManifest;
            }
        }
        return {};
    }

    [[nodiscard]] AppVersion getAppVersion() const{
        return m_AppVersion;
    }

private:
    AppVersion m_AppVersion;
    std::vector<FileManifestData> m_ManifestData;
};
