#pragma once

#include <string>
#include "nlohmann/json.hpp"
#include "Version.hpp"

class AppVersion {
public:
    AppVersion(nlohmann::json appVersionContent)
            : m_Version(appVersionContent) {
        if (appVersionContent.contains("appname")) {
            m_AppName = appVersionContent["appname"];
        }
    }

    [[nodiscard]] std::string getAppName() const {
        return m_AppName;
    }

    [[nodiscard]] Version getVersion() const {
        return m_Version;
    }

private:
    std::string m_AppName;
    Version m_Version;
};
