#pragma once

#include <nlohmann/json.hpp>
#include <string>

class DifferencePackageManifestInfo {
public:
	std::string AppName;
	std::string AppCurrentVersion;
	std::string AppBeforeVersion;
	std::string FileName;
	std::string Md5;
	std::vector<std::string> diff_updatefiles;
	std::vector<std::string> diff_newfiles;
	std::vector<std::string> diff_deletedfiles;

	friend void to_json(nlohmann::json& nlohmann_json_j,
						const DifferencePackageManifestInfo& nlohmann_json_t) {
		nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
		nlohmann_json_j["AppCurrentVersion"] =
			nlohmann_json_t.AppCurrentVersion;
		nlohmann_json_j["AppBeforeVersion"] = nlohmann_json_t.AppBeforeVersion;
		nlohmann_json_j["FileName"] = nlohmann_json_t.FileName;
		nlohmann_json_j["Md5"] = nlohmann_json_t.Md5;
		nlohmann_json_j["diff_updatefiles"] = nlohmann_json_t.diff_updatefiles;
		nlohmann_json_j["diff_newfiles"] = nlohmann_json_t.diff_newfiles;
		nlohmann_json_j["diff_deletedfiles"] =
			nlohmann_json_t.diff_deletedfiles;
	}

	friend void from_json(const nlohmann::json& nlohmann_json_j,
						  DifferencePackageManifestInfo& nlohmann_json_t) {
		DifferencePackageManifestInfo nlohmann_json_default_obj;
		nlohmann_json_t.AppName =
			nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
		nlohmann_json_t.AppCurrentVersion = nlohmann_json_j.value(
			"AppCurrentVersion", nlohmann_json_default_obj.AppCurrentVersion);
		nlohmann_json_t.AppBeforeVersion = nlohmann_json_j.value(
			"AppBeforeVersion", nlohmann_json_default_obj.AppBeforeVersion);
		nlohmann_json_t.FileName = nlohmann_json_j.value(
			"FileName", nlohmann_json_default_obj.FileName);
		nlohmann_json_t.Md5 =
			nlohmann_json_j.value("Md5", nlohmann_json_default_obj.Md5);
		nlohmann_json_t.diff_updatefiles = nlohmann_json_j.value(
			"diff_updatefiles", nlohmann_json_default_obj.diff_updatefiles);
		nlohmann_json_t.diff_newfiles = nlohmann_json_j.value(
			"diff_newfiles", nlohmann_json_default_obj.diff_newfiles);
		nlohmann_json_t.diff_deletedfiles = nlohmann_json_j.value(
			"diff_deletedfiles", nlohmann_json_default_obj.diff_deletedfiles);
	}
};
