#pragma once

#include <string>
#include <nlohmann/json.hpp>

class UpdatePackageBuildInfo {
public:
    std::string PackageMode;
    std::string OldVersionPath;
    std::string NewVersionPath;
    std::string AppName;
    std::string Channel;
    std::string Platform;
    std::string AppCurrentVersion;
    std::string AppBeforeVersion;
    std::string UpdateReadMe;

    friend void to_json(nlohmann::json&nlohmann_json_j, const UpdatePackageBuildInfo&nlohmann_json_t) {
        nlohmann_json_j["PackageMode"] = nlohmann_json_t.PackageMode;
        nlohmann_json_j["OldVersionPath"] = nlohmann_json_t.OldVersionPath;
        nlohmann_json_j["NewVersionPath"] = nlohmann_json_t.NewVersionPath;
        nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
        nlohmann_json_j["Channel"] = nlohmann_json_t.Channel;
        nlohmann_json_j["Platform"] = nlohmann_json_t.Platform;
        nlohmann_json_j["AppCurrentVersion"] = nlohmann_json_t.AppCurrentVersion;
        nlohmann_json_j["AppBeforeVersion"] = nlohmann_json_t.AppBeforeVersion;
        nlohmann_json_j["UpdateReadMe"] = nlohmann_json_t.UpdateReadMe;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, UpdatePackageBuildInfo&nlohmann_json_t) {
        UpdatePackageBuildInfo nlohmann_json_default_obj;
        nlohmann_json_t.PackageMode = nlohmann_json_j.value("PackageMode", nlohmann_json_default_obj.PackageMode);
        nlohmann_json_t.OldVersionPath = nlohmann_json_j.value("OldVersionPath",
                                                               nlohmann_json_default_obj.OldVersionPath);
        nlohmann_json_t.NewVersionPath = nlohmann_json_j.value("NewVersionPath",
                                                               nlohmann_json_default_obj.NewVersionPath);
        nlohmann_json_t.AppName = nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
        nlohmann_json_t.Channel = nlohmann_json_j.value("Channel", nlohmann_json_default_obj.Channel);
        nlohmann_json_t.Platform = nlohmann_json_j.value("Platform", nlohmann_json_default_obj.Platform);
        nlohmann_json_t.AppCurrentVersion = nlohmann_json_j.value("AppCurrentVersion",
                                                                  nlohmann_json_default_obj.AppCurrentVersion);
        nlohmann_json_t.AppBeforeVersion = nlohmann_json_j.value("AppBeforeVersion",
                                                                 nlohmann_json_default_obj.AppBeforeVersion);
        nlohmann_json_t.UpdateReadMe = nlohmann_json_j.value("UpdateReadMe",
                                                         nlohmann_json_default_obj.UpdateReadMe);
    }
};
