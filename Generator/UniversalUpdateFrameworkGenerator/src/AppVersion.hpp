#pragma once

#include <string>
#include <nlohmann/json.hpp>

class AppVersionInfo {
public:
    std::string AppName;
    std::string AppVersion;

    friend void to_json(nlohmann::json&nlohmann_json_j, const AppVersionInfo&nlohmann_json_t) {
        nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
        nlohmann_json_j["AppVersion"] = nlohmann_json_t.AppVersion;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, AppVersionInfo&nlohmann_json_t) {
        AppVersionInfo nlohmann_json_default_obj;
        nlohmann_json_t.AppName = nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
        nlohmann_json_t.AppVersion = nlohmann_json_j.value("AppVersion", nlohmann_json_default_obj.AppVersion);
    }
};
