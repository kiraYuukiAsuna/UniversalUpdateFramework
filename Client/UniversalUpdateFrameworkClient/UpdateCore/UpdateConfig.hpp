#pragma once

#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include "TypeDefinition.hpp"

struct UpdateConfig {
    std::string host{"127.0.0.1"};
    std::string appName{"DefaultAPPName"};
    std::string appPath{"DefaultAppPath"};
    std::string downloadPath{"Download"};
    std::string localCurrentVersion{"x.x.x"};
    std::string channel{"Release"};
    std::string platform{"Win"};

    friend void to_json(nlohmann::json &j, const UpdateConfig &value) {
        j["host"] = value.host;
        j["appName"] = value.appName;
        j["appPath"] = value.appPath;
        j["downloadPath"] = value.downloadPath;
        j["localCurrentVersion"] = value.localCurrentVersion;
        j["channel"] = value.channel;
        j["platform"] = value.platform;
    }

    friend void from_json(const nlohmann::json &j, UpdateConfig &value) {
        UpdateConfig defaultObj{};
        value.host = j.value("host", defaultObj.host);
        value.appName = j.value("appName", defaultObj.appName);
        value.appPath = j.value("appPath", defaultObj.appPath);
        value.downloadPath = j.value("downloadPath", defaultObj.downloadPath);
        value.localCurrentVersion = j.value("localCurrentVersion", defaultObj.localCurrentVersion);
        value.channel = j.value("channel", defaultObj.channel);
        value.platform = j.value("platform", defaultObj.platform);
    }
};


class UpdateConfigIo {
public:
    UpdateConfigIo(std::string configFilePath)
            : m_ConfigFilePath(configFilePath) {

    }

    ReturnWrapper readFromFile() {
        std::ifstream infile(m_ConfigFilePath);
        if (!infile.is_open()) {
            return {false, ErrorCode::OpenConfigFileFailed,
                    std::string(magic_enum::enum_name(ErrorCode::OpenConfigFileFailed))};
        }

        auto jsonObj = nlohmann::json::parse(infile);
        m_Config = jsonObj.get<UpdateConfig>();

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

    UpdateConfig &getConfig() {
        return m_Config;
    }

    void setConfig(UpdateConfig &config) {
        m_Config = config;
    }

private:
    std::string m_ConfigFilePath;
    UpdateConfig m_Config;
};
