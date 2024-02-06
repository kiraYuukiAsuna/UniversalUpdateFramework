#pragma once

#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include "TypeDefinition.h"

struct Config {
    std::string host;
    std::string appName;
    std::string appPath;
    std::string downloadPath;
    std::string localCurrentVersion;

    friend void to_json(nlohmann::json &j, const Config &value) {
        j["host"] = value.host;
        j["appName"] = value.appName;
        j["appPath"] = value.appPath;
        j["downloadPath"] = value.downloadPath;
        j["localCurrentVersion"] = value.localCurrentVersion;
    }

    friend void from_json(const nlohmann::json &j, Config &value) {
        Config defaultObj{
                .host = "127.0.0.1",
                .appName = "DefaultAPPName",
                .appPath = "DefaultAppPath",
                .downloadPath = "Download",
                .localCurrentVersion = "x.x.x"
        };
        value.host = j.value("host", defaultObj.host);
        value.appName = j.value("appName", defaultObj.appName);
        value.appPath = j.value("appPath", defaultObj.appPath);
        value.downloadPath = j.value("downloadPath", defaultObj.downloadPath);
        value.localCurrentVersion = j.value("localCurrentVersion", defaultObj.localCurrentVersion);
    }
};


class UpdateConfig {
public:
    UpdateConfig(std::string configFilePath)
            : m_ConfigFilePath(configFilePath) {

    }

    ReturnWrapper readFromFile() {
        std::ifstream infile(m_ConfigFilePath);
        if (!infile.is_open()) {
            return {false, ErrorCode::OpenConfigFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::OpenConfigFileFailed))};
        }

        auto jsonObj = nlohmann::json::parse(infile);
        m_Config = jsonObj.get<Config>();

        infile.close();

        return {true};
    }

    ReturnWrapper writeToFile() {
        nlohmann::json jsonObj = m_Config;

        std::ofstream outfile(m_ConfigFilePath);
        if (!outfile.is_open()) {
            return {false, ErrorCode::OpenConfigFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::OpenConfigFileFailed))};
        }

        outfile << jsonObj.dump(4);
        outfile.close();

        return {true};
    }

    Config &getConfig() {
        return m_Config;
    }

    void setConfig(Config &config) {
        m_Config = config;
    }

private:
    std::string m_ConfigFilePath;
    Config m_Config;
};