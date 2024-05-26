#pragma once

#include <string>
#include <nlohmann/json.hpp>

class FullPackageManifestInfo {
public:
    std::string AppName;
    std::string AppVersion;
    std::string FileName;
    std::string Md5;


    friend void to_json(nlohmann::json&nlohmann_json_j, const FullPackageManifestInfo&nlohmann_json_t) {
        nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
        nlohmann_json_j["AppVersion"] = nlohmann_json_t.AppVersion;
        nlohmann_json_j["FileName"] = nlohmann_json_t.FileName;
        nlohmann_json_j["Md5"] = nlohmann_json_t.Md5;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, FullPackageManifestInfo&nlohmann_json_t) {
        FullPackageManifestInfo nlohmann_json_default_obj;
        nlohmann_json_t.AppName = nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
        nlohmann_json_t.AppVersion = nlohmann_json_j.value("AppVersion", nlohmann_json_default_obj.AppVersion);
        nlohmann_json_t.FileName = nlohmann_json_j.value("FileName", nlohmann_json_default_obj.FileName);
        nlohmann_json_t.Md5 = nlohmann_json_j.value("Md5", nlohmann_json_default_obj.Md5);
    }
};
