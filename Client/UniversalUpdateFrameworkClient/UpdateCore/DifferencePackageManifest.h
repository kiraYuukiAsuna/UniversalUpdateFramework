#pragma once

#include <string>
#include <exception>
#include "json.hpp"
#include "AppVersion.h"

class DifferencePackageManifest {
public:
    DifferencePackageManifest(nlohmann::json differencePackageManifestContent)
            : m_NewAppVersion(differencePackageManifestContent), m_OldVersion(
            differencePackageManifestContent.contains("appbeforeversion")
            ? [&differencePackageManifestContent](){
                std::string str = differencePackageManifestContent["appbeforeversion"];
                return str;
            }.operator()() : throw std::runtime_error(
                    "DifferencePackage Manifest Json Error!")) {
        auto str = differencePackageManifestContent.dump();
        if (differencePackageManifestContent.contains("filename")) {
            m_DifferencePackageFileName = differencePackageManifestContent["filename"];
        } else {
            throw std::runtime_error("DifferencePackage Manifest Json Error");
        }
        if (differencePackageManifestContent.contains("md5")) {
            m_DifferencePackageMd5 = differencePackageManifestContent["md5"];
        } else {
            throw std::runtime_error("DifferencePackage Manifest Json Error");
        }
        if (differencePackageManifestContent.contains("diff_deletedfiles")) {
            if(!differencePackageManifestContent["diff_deletedfiles"].is_null()){
                if (differencePackageManifestContent["diff_deletedfiles"].is_array()) {
                    for (auto &file: differencePackageManifestContent["diff_deletedfiles"]) {
                        m_DiffDeletedFiles.push_back(file);
                    }
                } else {
                    throw std::runtime_error("DifferencePackage Manifest Json Error");
                }
            }
        }
        if (differencePackageManifestContent.contains("diff_newfiles")) {
            if(!differencePackageManifestContent["diff_newfiles"].is_null()) {
                if (differencePackageManifestContent["diff_newfiles"].is_array()) {
                    for (auto &file: differencePackageManifestContent["diff_newfiles"]) {
                        m_DiffNewFiles.push_back(file);
                    }
                } else {
                    throw std::runtime_error("DifferencePackage Manifest Json Error");
                }
            }
        }
        if (differencePackageManifestContent.contains("diff_updatefiles")) {
            if(!differencePackageManifestContent["diff_updatefiles"].is_null()) {
                if (differencePackageManifestContent["diff_updatefiles"].is_array()) {
                    for (auto &file: differencePackageManifestContent["diff_updatefiles"]) {
                        m_DiffUpdateFiles.push_back(file);
                    }
                } else {
                    throw std::runtime_error("DifferencePackage Manifest Json Error");
                }
            }
        }
    }

    [[nodiscard]] AppVersion getNewAppVersion() const {
        return m_NewAppVersion;
    }

    [[nodiscard]] Version getOldVersion() const {
        return m_OldVersion;
    }

    [[nodiscard]] std::string getDifferencePackageFileName() const {
        return m_DifferencePackageFileName;
    }

    [[nodiscard]] std::string getDifferencePackageMd5() const {
        return m_DifferencePackageMd5;
    }

    [[nodiscard]] std::vector<std::string> getDiffDeletedFiles() const {
        return m_DiffDeletedFiles;
    }

    [[nodiscard]] std::vector<std::string> getDiffNewFiles() const {
        return m_DiffNewFiles;
    }

    [[nodiscard]] std::vector<std::string> getDiffUpdateFiles() const {
        return m_DiffUpdateFiles;
    }

private:
    AppVersion m_NewAppVersion;
    Version m_OldVersion;
    std::string m_DifferencePackageFileName;
    std::string m_DifferencePackageMd5;
    std::vector<std::string> m_DiffDeletedFiles;
    std::vector<std::string> m_DiffNewFiles;
    std::vector<std::string> m_DiffUpdateFiles;
};
