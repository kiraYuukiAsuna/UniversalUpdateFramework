#pragma once

#include <fstream>
#include <string>

#include "TypeDefinition.hpp"
#include "nlohmann/json.hpp"

struct UpdateConfig {
	std::string host{"127.0.0.1:5275"};
	std::string appName{"DefaultAPPName"};
	std::string appPath{"DefaultAppPath"};
	std::string downloadPath{"Download"};
	std::string localCurrentVersion{"x.x.x"};
	std::string channel{"Release"};
	std::string platform{"Win"};
	std::string updaterName{"Updater"};
	std::string localUpdaterCurrentVersion{"x.x.x"};

	friend void to_json(nlohmann::json &j, const UpdateConfig &value) {
		j["host"] = value.host;
		j["appName"] = value.appName;
		j["appPath"] = value.appPath;
		j["downloadPath"] = value.downloadPath;
		j["localCurrentVersion"] = value.localCurrentVersion;
		j["channel"] = value.channel;
		j["platform"] = value.platform;
		j["updaterName"] = value.updaterName;
		j["localUpdaterCurrentVersion"] = value.localUpdaterCurrentVersion;
	}

	friend void from_json(const nlohmann::json &j, UpdateConfig &value) {
		UpdateConfig defaultObj{};
		value.host = j.value("host", defaultObj.host);
		value.appName = j.value("appName", defaultObj.appName);
		value.appPath = j.value("appPath", defaultObj.appPath);
		value.downloadPath = j.value("downloadPath", defaultObj.downloadPath);
		value.localCurrentVersion =
			j.value("localCurrentVersion", defaultObj.localCurrentVersion);
		value.channel = j.value("channel", defaultObj.channel);
		value.platform = j.value("platform", defaultObj.platform);
		value.updaterName = j.value("updaterName", defaultObj.updaterName);
		value.localUpdaterCurrentVersion =
			j.value("localUpdaterCurrentVersion",
					defaultObj.localUpdaterCurrentVersion);
	}
};

class UpdateConfigIo {
public:
	UpdateConfigIo(std::string configFilePath)
		: m_ConfigFilePath(configFilePath) {}

	ReturnWrapper readFromFile() {
		if (!std::filesystem::exists(m_ConfigFilePath)) {
			if (!std::filesystem::exists(
					std::filesystem::path(m_ConfigFilePath).parent_path())) {
				std::filesystem::create_directories(
					std::filesystem::path(m_ConfigFilePath).parent_path());
			}
			writeToFile();
		}

		std::ifstream infile(m_ConfigFilePath);
		if (!infile.is_open()) {
			return {false, ErrorCode::OpenConfigFileFailed,
					std::string(magic_enum::enum_name(
						ErrorCode::OpenConfigFileFailed))};
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
					std::string(magic_enum::enum_name(
						ErrorCode::OpenConfigFileFailed))};
		}

		outfile << jsonObj.dump(4);
		outfile.close();

		return {true};
	}

	UpdateConfig &getConfig() { return m_Config; }

	void setConfig(UpdateConfig &config) { m_Config = config; }

private:
	std::string m_ConfigFilePath;
	UpdateConfig m_Config;
};
