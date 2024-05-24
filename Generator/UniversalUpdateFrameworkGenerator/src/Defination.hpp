#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class PackageBuildInfo {
public:
    std::string PackageMode;
    std::string OldVersionPath;
    std::string NewVersionPath;
    std::string AppName;
    std::string Channel;
    std::string Platform;
    std::string AppCurrentVersion;
    std::string AppBeforeVersion;

    friend void to_json(nlohmann::json&nlohmann_json_j, const PackageBuildInfo&nlohmann_json_t) {
        nlohmann_json_j["PackageMode"] = nlohmann_json_t.PackageMode;
        nlohmann_json_j["OldVersionPath"] = nlohmann_json_t.OldVersionPath;
        nlohmann_json_j["NewVersionPath"] = nlohmann_json_t.NewVersionPath;
        nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
        nlohmann_json_j["Channel"] = nlohmann_json_t.Channel;
        nlohmann_json_j["Platform"] = nlohmann_json_t.Platform;
        nlohmann_json_j["AppCurrentVersion"] = nlohmann_json_t.AppCurrentVersion;
        nlohmann_json_j["AppBeforeVersion"] = nlohmann_json_t.AppBeforeVersion;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, PackageBuildInfo&nlohmann_json_t) {
        PackageBuildInfo nlohmann_json_default_obj;
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
    }
};

class DifferencePackageManifest {
public:
    std::string AppName;
    std::string AppVersion;
    std::string FileName;
    std::string AppBeforeVersion;
    std::vector<std::string> diff_updatefiles;
    std::vector<std::string> diff_newfiles;
    std::vector<std::string> diff_deletedfiles;

    friend void to_json(nlohmann::json&nlohmann_json_j, const DifferencePackageManifest&nlohmann_json_t) {
        nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
        nlohmann_json_j["AppVersion"] = nlohmann_json_t.AppVersion;
        nlohmann_json_j["FileName"] = nlohmann_json_t.FileName;
        nlohmann_json_j["AppBeforeVersion"] = nlohmann_json_t.AppBeforeVersion;
        nlohmann_json_j["diff_updatefiles"] = nlohmann_json_t.diff_updatefiles;
        nlohmann_json_j["diff_newfiles"] = nlohmann_json_t.diff_newfiles;
        nlohmann_json_j["diff_deletedfiles"] = nlohmann_json_t.diff_deletedfiles;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, DifferencePackageManifest&nlohmann_json_t) {
        DifferencePackageManifest nlohmann_json_default_obj;
        nlohmann_json_t.AppName = nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
        nlohmann_json_t.AppVersion = nlohmann_json_j.value("AppVersion", nlohmann_json_default_obj.AppVersion);
        nlohmann_json_t.FileName = nlohmann_json_j.value("FileName", nlohmann_json_default_obj.FileName);
        nlohmann_json_t.AppBeforeVersion = nlohmann_json_j.value("AppBeforeVersion",
                                                                 nlohmann_json_default_obj.AppBeforeVersion);
        nlohmann_json_t.diff_updatefiles = nlohmann_json_j.value("diff_updatefiles",
                                                                 nlohmann_json_default_obj.diff_updatefiles);
        nlohmann_json_t.diff_newfiles = nlohmann_json_j.value("diff_newfiles", nlohmann_json_default_obj.diff_newfiles);
        nlohmann_json_t.diff_deletedfiles = nlohmann_json_j.value("diff_deletedfiles",
                                                                  nlohmann_json_default_obj.diff_deletedfiles);
    }
};

class Manifest {
public:
    std::string FilePath;
    std::string FileName;
    std::string Md5;

    friend void to_json(nlohmann::json&nlohmann_json_j, const Manifest&nlohmann_json_t) {
        nlohmann_json_j["FilePath"] = nlohmann_json_t.FilePath;
        nlohmann_json_j["FileName"] = nlohmann_json_t.FileName;
        nlohmann_json_j["Md5"] = nlohmann_json_t.Md5;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, Manifest&nlohmann_json_t) {
        Manifest nlohmann_json_default_obj;
        nlohmann_json_t.FilePath = nlohmann_json_j.value("FilePath", nlohmann_json_default_obj.FilePath);
        nlohmann_json_t.FileName = nlohmann_json_j.value("FileName", nlohmann_json_default_obj.FileName);
        nlohmann_json_t.Md5 = nlohmann_json_j.value("Md5", nlohmann_json_default_obj.Md5);
    }
};

class FullPackageManifest {
public:
    std::string AppName;
    std::string AppVersion;
    std::string FileName;
    std::vector<Manifest> Manifests;

    friend void to_json(nlohmann::json&nlohmann_json_j, const FullPackageManifest&nlohmann_json_t) {
        nlohmann_json_j["AppName"] = nlohmann_json_t.AppName;
        nlohmann_json_j["AppVersion"] = nlohmann_json_t.AppVersion;
        nlohmann_json_j["FileName"] = nlohmann_json_t.FileName;
        nlohmann_json_j["Manifests"] = nlohmann_json_t.Manifests;
    }

    friend void from_json(const nlohmann::json&nlohmann_json_j, FullPackageManifest&nlohmann_json_t) {
        FullPackageManifest nlohmann_json_default_obj;
        nlohmann_json_t.AppName = nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
        nlohmann_json_t.AppVersion = nlohmann_json_j.value("AppVersion", nlohmann_json_default_obj.AppVersion);
        nlohmann_json_t.FileName = nlohmann_json_j.value("FileName", nlohmann_json_default_obj.FileName);
        nlohmann_json_t.AppName = nlohmann_json_j.value("AppName", nlohmann_json_default_obj.AppName);
        nlohmann_json_t.Manifests = nlohmann_json_j.value("Manifests", nlohmann_json_default_obj.Manifests);
    }
};

#ifdef WIN32
    constexpr std::string hdiffzExecuable = "hdiffz.exe";
#elif defined __linux
constexpr std::string hdiffzExecuable = "./hdiffz";
#endif
