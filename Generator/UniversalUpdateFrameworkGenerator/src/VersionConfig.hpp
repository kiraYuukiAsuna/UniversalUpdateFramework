#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "AppManifest.hpp"

class AppVersionBriefInfo {
public:
    std::string version;
    std::vector<FileManifestInfo> filemanifests;
    std::string storage_request_url;

    friend void to_json(nlohmann::json&nlohmann_json_j, const AppVersionBriefInfo&nlohmann_json_t) {
        nlohmann_json_j["version"] = nlohmann_json_t.version;
        nlohmann_json_j["filemanifests"] = nlohmann_json_t.filemanifests;
        nlohmann_json_j["storage_request_url"] =
            nlohmann_json_t.storage_request_url;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, AppVersionBriefInfo&nlohmann_json_t) {
        AppVersionBriefInfo nlohmann_json_default_obj;
        nlohmann_json_t.version = nlohmann_json_j.value("version", nlohmann_json_default_obj.version);
        nlohmann_json_t.filemanifests = nlohmann_json_j.value(
            "filemanifests", nlohmann_json_default_obj.filemanifests);
        nlohmann_json_t.storage_request_url = nlohmann_json_j.value(
            "storage_request_url",
            nlohmann_json_default_obj.storage_request_url);
    }
};


class AppVersionConfigInfo {
public:
    std::string current_version;
    std::vector<AppVersionBriefInfo> versions;

    friend void to_json(nlohmann::json&nlohmann_json_j, const AppVersionConfigInfo&nlohmann_json_t) {
        nlohmann_json_j["current_version"] = nlohmann_json_t.current_version;
        nlohmann_json_j["versions"] = nlohmann_json_t.versions;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, AppVersionConfigInfo&nlohmann_json_t) {
        AppVersionConfigInfo nlohmann_json_default_obj;
        nlohmann_json_t.current_version = nlohmann_json_j.value("current_version", nlohmann_json_default_obj.current_version);
        nlohmann_json_t.versions = nlohmann_json_j.value("versions", nlohmann_json_default_obj.versions);
    }
};
