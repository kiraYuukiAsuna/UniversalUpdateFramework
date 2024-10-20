#pragma once

#include <fstream>
#include <string>

#include "UpdateCore/TypeDefinition.hpp"
#include "nlohmann/json.hpp"

struct AppSpecification {
	std::string appUpdateConfigFile = "AppUpdateConfig.json";
	std::string appBackgroundPictureUrl = "Unused for now";
	std::string appExecutablePath;
	std::string appExecutableName;

	friend void to_json(nlohmann::json &j, const AppSpecification &value) {
		j["appUpdateConfigFile"] = value.appUpdateConfigFile;
		j["appBackgroundPictureUrl"] = value.appBackgroundPictureUrl;
		j["appExecutablePath"] = value.appExecutablePath;
		j["appExecutableName"] = value.appExecutableName;
	}

	friend void from_json(const nlohmann::json &j, AppSpecification &value) {
		AppSpecification defaultObj{};
		value.appUpdateConfigFile =
			j.value("appUpdateConfigFile", defaultObj.appUpdateConfigFile);
		value.appBackgroundPictureUrl = j.value(
			"appBackgroundPictureUrl", defaultObj.appBackgroundPictureUrl);
		value.appExecutablePath =
			j.value("appExecutablePath", defaultObj.appExecutablePath);
		value.appExecutableName =
			j.value("appExecutableName", defaultObj.appExecutableName);
	}
};

class AppSpecificationIo {
public:
	AppSpecificationIo(std::string configFilePath)
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
		m_Config = jsonObj.get<AppSpecification>();

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

	AppSpecification &getConfig() { return m_Config; }

	void setConfig(AppSpecification &config) { m_Config = config; }

private:
	std::string m_ConfigFilePath;
	AppSpecification m_Config;
};
