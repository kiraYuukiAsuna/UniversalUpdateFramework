#pragma once

#include <nlohmann/json.hpp>
#include <string>

class FileManifestInfo {
public:
	std::string FilePath;
	std::string FileName;
	std::string Md5;
	uint16_t Permission;

	friend void to_json(nlohmann::json& nlohmann_json_j,
						const FileManifestInfo& nlohmann_json_t) {
		nlohmann_json_j["FilePath"] = nlohmann_json_t.FilePath;
		nlohmann_json_j["FileName"] = nlohmann_json_t.FileName;
		nlohmann_json_j["Md5"] = nlohmann_json_t.Md5;
		nlohmann_json_j["Permission"] = nlohmann_json_t.Permission;
	}

	friend void from_json(const nlohmann::json& nlohmann_json_j,
						  FileManifestInfo& nlohmann_json_t) {
		FileManifestInfo nlohmann_json_default_obj;
		nlohmann_json_t.FilePath = nlohmann_json_j.value(
			"FilePath", nlohmann_json_default_obj.FilePath);
		nlohmann_json_t.FileName = nlohmann_json_j.value(
			"FileName", nlohmann_json_default_obj.FileName);
		nlohmann_json_t.Md5 =
			nlohmann_json_j.value("Md5", nlohmann_json_default_obj.Md5);
		nlohmann_json_t.Permission = nlohmann_json_j.value(
			"Permission", nlohmann_json_default_obj.Permission);
	}
};

class AppManifestInfo {
public:
	std::string AppName;
	std::string AppVersion;
	std::string FileName;
	std::vector<FileManifestInfo> Manifests;
	std::string UpdateReadMe;

	friend void to_json(nlohmann::json& nlohmann_json_j,
						const AppManifestInfo& nlohmann_json_t) {
		nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
		nlohmann_json_j["AppVersion"] = nlohmann_json_t.AppVersion;
		nlohmann_json_j["FileName"] = nlohmann_json_t.FileName;
		nlohmann_json_j["Manifests"] = nlohmann_json_t.Manifests;
		nlohmann_json_j["UpdateReadMe"] = nlohmann_json_t.UpdateReadMe;
	}

	friend void from_json(const nlohmann::json& nlohmann_json_j,
						  AppManifestInfo& nlohmann_json_t) {
		AppManifestInfo nlohmann_json_default_obj;
		nlohmann_json_t.AppName =
			nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
		nlohmann_json_t.AppVersion = nlohmann_json_j.value(
			"AppVersion", nlohmann_json_default_obj.AppVersion);
		nlohmann_json_t.FileName = nlohmann_json_j.value(
			"FileName", nlohmann_json_default_obj.FileName);
		nlohmann_json_t.Manifests = nlohmann_json_j.value(
			"Manifests", nlohmann_json_default_obj.Manifests);
		nlohmann_json_t.UpdateReadMe = nlohmann_json_j.value(
			"UpdateReadMe", nlohmann_json_default_obj.UpdateReadMe);
	}
};
