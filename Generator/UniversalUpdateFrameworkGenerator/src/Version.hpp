#pragma once

#include <string>

#include "nlohmann/json.hpp"
#include "util.hpp"

class Version {
public:
	Version(std::string major, std::string minor, std::string fix)
		: m_Major(std::stoi(major)),
		  m_Minor(std::stoi(minor)),
		  m_Fix(std::stoi(fix)) {}

	Version(std::string versionString) {
		if (versionString.empty()) {
			versionString = "0.0.0";
		}
		auto result = util::string_split(versionString, '.');

		if (result.size() != 3) {
			throw std::runtime_error("Version String Error!");
		}

		m_Major = std::stoi(result[0]);
		m_Minor = std::stoi(result[1]);
		m_Fix = std::stoi(result[2]);
	}

	Version(nlohmann::json versionJson) {
		if (versionJson.contains("appversion")) {
			if (versionJson["appversion"].is_string()) {
				std::string versionString = versionJson["appversion"];
				auto result = util::string_split(versionString, '.');

				if (result.size() != 3) {
					throw std::runtime_error("Version Json Error!");
				}

				m_Major = std::stoi(result[0]);
				m_Minor = std::stoi(result[1]);
				m_Fix = std::stoi(result[2]);
			} else {
				throw std::runtime_error("Version Json Error!");
			}
		} else {
			throw std::runtime_error("Version Json Error!");
		}
	}

	bool operator>(Version &version) const {
		if (this->m_Major > version.getMajorVersion()) {
			return true;
		} else if (this->m_Major == version.getMajorVersion()) {
			if (this->m_Minor > version.getMinorVersion()) {
				return true;
			} else if (this->m_Minor == version.getMinorVersion()) {
				if (this->m_Fix > version.getFixVersion()) {
					return true;
				} else if (this->m_Fix == version.getFixVersion()) {
					return false;
				}
			}
		}
		return false;
	}

	bool operator<(Version &version) const {
		if (this->m_Major < version.getMajorVersion()) {
			return true;
		} else if (this->m_Major == version.getMajorVersion()) {
			if (this->m_Minor < version.getMinorVersion()) {
				return true;
			} else if (this->m_Minor == version.getMinorVersion()) {
				if (this->m_Fix < version.getFixVersion()) {
					return true;
				} else if (this->m_Fix == version.getFixVersion()) {
					return false;
				}
			}
		}
		return false;
	}

	bool operator==(Version &version) const {
		if (this->m_Major == version.getMajorVersion()) {
			if (this->m_Minor == version.getMinorVersion()) {
				if (this->m_Fix == version.getFixVersion()) {
					return true;
				}
			}
		}
		return false;
	}

	std::string getVersionString() {
		return std::to_string(m_Major) + "." + std::to_string(m_Minor) + "." +
			   std::to_string(m_Fix);
	}

	[[nodiscard]] int32_t getMajorVersion() const { return m_Major; }

	[[nodiscard]] int32_t getMinorVersion() const { return m_Minor; }

	[[nodiscard]] int32_t getFixVersion() const { return m_Fix; }

private:
	int32_t m_Major;
	int32_t m_Minor;
	int32_t m_Fix;
};
