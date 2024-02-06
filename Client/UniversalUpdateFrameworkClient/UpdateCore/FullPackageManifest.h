#pragma once

#include <string>
#include "nlohmann/json.hpp"
#include <exception>
#include "AppVersion.h"

class FullPackageManifest {
public:
    FullPackageManifest(nlohmann::json fullPackageManifestContent)
            : m_AppVersion(fullPackageManifestContent) {
        if (fullPackageManifestContent.contains("filename")) {
            m_FullPackageFileName = fullPackageManifestContent["filename"];
        } else {
            throw std::runtime_error("FullPackage Manifest Json Error");
        }
        if (fullPackageManifestContent.contains("md5")) {
            m_FullPackageMd5 = fullPackageManifestContent["md5"];
        } else {
            throw std::runtime_error("FullPackage Manifest Json Error");
        }
    }

    [[nodiscard]] AppVersion getAppVersion() const {
        return m_AppVersion;
    }

    [[nodiscard]] std::string getFullPackageFileName() const {
        return m_FullPackageFileName;
    }

    [[nodiscard]] std::string getFullPackageMd5() const {
        return m_FullPackageMd5;
    }

private:
    AppVersion m_AppVersion;
    std::string m_FullPackageFileName;
    std::string m_FullPackageMd5;
};
