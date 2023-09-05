#pragma once

#include <iostream>
#include <format>
#include "httplib.h"
#include "json.hpp"
#include "util.hpp"

class Version{
public:
    Version(std::string major, std::string minor, std::string fix)
    : m_Major(std::stoi(major)), m_Minor(std::stoi(minor)), m_Fix(std::stoi(fix)) {

    }

    Version(nlohmann::json versionJson){
        if(versionJson.contains("version")){
            if(versionJson["version"].is_string()){
                auto versionString = versionJson["version"];
                auto result = util::string_split(versionJson, '.');
                
                if(result.size() != 3){
                    throw std::runtime_error("Version Json Error!");
                }

                m_Major = std::stoi(result[0]);
                m_Minor = std::stoi(result[1]);
                m_Fix = std::stoi(result[2]);
            }else{
                throw std::runtime_error("Version Json Error!");

            }
        }else{
            throw std::runtime_error("Version Json Error!");
        }
    }

    bool operator>(Version& version) const{
        if(this->m_Major > version.getMajorVersion()){
            return true;
        }else if(this->m_Major == version.getMajorVersion()) {
            if (this->m_Minor > version.getMinorVersion()) {
                return true;
            } else if (this->m_Minor == version.getMinorVersion())
            {
                if(this->m_Fix > version.getFixVersion()){
                    return true;
                }else if(this->m_Fix == version.getFixVersion()){
                    return false;
                }
            }
        }
        return false;
    }

    bool operator<(Version& version) const{
        if(this->m_Major < version.getMajorVersion()){
            return true;
        }else if(this->m_Major == version.getMajorVersion()) {
            if (this->m_Minor < version.getMinorVersion()) {
                return true;
            } else if (this->m_Minor == version.getMinorVersion())
            {
                if(this->m_Fix < version.getFixVersion()){
                    return true;
                }else if(this->m_Fix == version.getFixVersion()){
                    return false;
                }
            }
        }
        return false;
    }

    bool operator==(Version& version) const{
        if(this->m_Major == version.getMajorVersion()){
            if (this->m_Minor == version.getMinorVersion()) {
                if(this->m_Fix == version.getFixVersion()){
                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] int32_t getMajorVersion() const{
        return m_Major;
    }

    [[nodiscard]] int32_t getMinorVersion() const{
        return m_Minor;
    }

    [[nodiscard]] int32_t getFixVersion() const{
        return m_Fix;
    }

private:
    int32_t m_Major;
    int32_t m_Minor;
    int32_t m_Fix;
};

struct FileManifestData{
    std::string fileName;
    std::string filePath;
    std::string md5;
};

class AppVersion{
public:
    AppVersion(nlohmann::json appVersionContent)
    : m_Version(appVersionContent.contains("version") ? appVersionContent["version"] : throw std::runtime_error("Version Json Error!")) {
        if(appVersionContent.contains("appname")){
            m_AppName = appVersionContent["appname"];
        }
    }

    [[nodiscard]] std::string getAppName() const{
        return m_AppName;
    }

    [[nodiscard]] Version getVersion() const{
        return m_Version;
    }

private:
    std::string m_AppName;
    Version m_Version;
};

class AppManifest{
public:
    AppManifest(nlohmann::json appManifestContent)
    : m_AppVersion(appManifestContent) {
        if(appManifestContent.contains("manifest")){
            if(appManifestContent["manifest"].is_array()){
                for(auto& fileManifest : appManifestContent["manifest"]){
                    FileManifestData data;
                    if(fileManifest.contains("filename")){
                        data.fileName = fileManifest["filename"];
                    }else{
                        throw std::runtime_error("Manifest Json Error!");
                    }
                    if(fileManifest.contains("filepath")){
                        data.filePath = fileManifest["filepath"];
                    }else{
                        throw std::runtime_error("Manifest Json Error!");
                    }
                    if(fileManifest.contains("md5")){
                        data.md5 = fileManifest["md5"];
                    }else{
                        throw std::runtime_error("Manifest Json Error!");
                    }
                }
            }else{
                throw std::runtime_error("Manifest Json Error!");
            }
        }else{
            throw std::runtime_error("Manifest Json Error!");
        }
    }

    [[nodiscard]] std::vector<FileManifestData> getManifestData() const{
        return m_ManifestData;
    }

    [[nodiscard]] FileManifestData getFileManifest(std::string filePath) const{
        for(auto& fileManifest : m_ManifestData){
            if(fileManifest.filePath == filePath){
                return fileManifest;
            }
        }
        return {};
    }

private:
    AppVersion m_AppVersion;
    std::vector<FileManifestData> m_ManifestData;
};


class FullPackageManifest{
public:
    FullPackageManifest(nlohmann::json fullPackageManifestContent)
    : m_AppVersion(fullPackageManifestContent) {
        if(fullPackageManifestContent.contains("filename")){
            m_FullPackageFileName = fullPackageManifestContent["filename"];
        }else{
            throw std::runtime_error("FullPackage Manifest Json Error");
        }
        if(fullPackageManifestContent.contains("md5")){
            m_FullPackageMd5 = fullPackageManifestContent["md5"];
        }else{
            throw std::runtime_error("FullPackage Manifest Json Error");
        }
    }

    [[nodiscard]] AppVersion getAppVersion() const{
        return m_AppVersion;
    }

    [[nodiscard]] std::string getFullPackageFileName() const{
        return m_FullPackageFileName;
    }

    [[nodiscard]] std::string getFullPackageMd5() const{
        return m_FullPackageMd5;
    }
    
private:
    AppVersion m_AppVersion;
    std::string m_FullPackageFileName;
    std::string m_FullPackageMd5;
};

class DifferencePackageManifest{
public:
    DifferencePackageManifest(nlohmann::json differencePackageManifestContent)
    : m_NewAppVersion(differencePackageManifestContent), m_OldVersion(differencePackageManifestContent.contains("appbeforeversion") ? differencePackageManifestContent["appbeforeversion"] : throw std::runtime_error("DifferencePackage Manifest Json Error!") ) {
        if(differencePackageManifestContent.contains("filename")){
            m_DifferencePackageFileName = differencePackageManifestContent["filename"];
        }else{
            throw std::runtime_error("DifferencePackage Manifest Json Error");
        }
        if(differencePackageManifestContent.contains("md5")){
            m_DifferencePackageMd5 = differencePackageManifestContent["md5"];
        }else{
            throw std::runtime_error("DifferencePackage Manifest Json Error");
        }
        if(differencePackageManifestContent.contains("diff_deletedfiles")){
            if(differencePackageManifestContent["diff_deletedfiles"].is_array()){
                for(auto& file : differencePackageManifestContent["diff_deletedfiles"]){
                    m_DiffDeletedFiles.push_back(file);
                }
            }else{
                throw std::runtime_error("DifferencePackage Manifest Json Error");
            }
        }
        if(differencePackageManifestContent.contains("diff_newfiles")){
            if(differencePackageManifestContent["diff_newfiles"].is_array()){
                for(auto& file : differencePackageManifestContent["diff_newfiles"]){
                    m_DiffNewFiles.push_back(file);
                }
            }else{
                throw std::runtime_error("DifferencePackage Manifest Json Error");
            }
        }
        if(differencePackageManifestContent.contains("diff_updatefiles")){
            if(differencePackageManifestContent["diff_updatefiles"].is_array()){
                for(auto& file : differencePackageManifestContent["diff_updatefiles"]){
                    m_DiffUpdateFiles.push_back(file);
                }
            }else{
                throw std::runtime_error("DifferencePackage Manifest Json Error");
            }
        }
    }

    [[nodiscard]] AppVersion getNewAppVersion() const{
        return m_NewAppVersion;
    }

    [[nodiscard]] Version getOldVersion() const{
        return m_OldVersion;
    }

    [[nodiscard]] std::string getDifferencePackageFileName() const{
        return m_DifferencePackageFileName;
    }

    [[nodiscard]] std::string getDifferencePackageMd5() const{
        return m_DifferencePackageMd5;
    }

    [[nodiscard]] std::vector<std::string> getDiffDeletedFiles() const{
        return m_DiffDeletedFiles;
    }

    [[nodiscard]] std::vector<std::string> getDiffNewFiles() const{
        return m_DiffNewFiles;
    }

    [[nodiscard]] std::vector<std::string> getDiffUpdateFiles() const{
        return m_DiffUpdateFiles;
    }

private:
    AppVersion m_NewAppVersion;
    Version m_OldVersion;
    std::string m_DifferencePackageFileName;
    std::string m_DifferencePackageMd5;
    std::vector<std::string> m_DiffDeletedFiles;
    std::vector<std::string> m_DiffNewFiles;
    std::vector<std::string> m_DiffUpdateFiles;
};

class ReturnWrapper{
public:
    ReturnWrapper(bool status)
    : m_Status(status) {

    }

    ReturnWrapper(bool status, int32_t errorNumber, std::string errorMessage)
    : m_Status(status), m_ErrorNumber(errorNumber), m_ErrorMessage(errorMessage) {

    }

    [[nodiscard]] bool getStatus() const{
        return m_Status;
    }

    [[nodiscard]] int32_t getErrorNumber() const{
        return m_ErrorNumber;
    }

    [[nodiscard]] std::string getErrorMessage() const{
        return m_ErrorMessage;
    }

private:
    bool m_Status{false};
    int32_t m_ErrorNumber{-1};
    std::string m_ErrorMessage{"DefaultErrorMessage"};
};



enum class UpdateMode{
    eFullPackageUpdate,
    eDifferencePackageUpdate
};

class UpdateBase{
public:
    Version getVersion(Version version){

    }

    ReturnWrapper getManifest(Version version){

    }
};

class ApiRequest{
public:
    ApiRequest(std::string host, std::string appname)
    : m_Host(host), m_AppName(appname){

    }

    std::pair<ReturnWrapper, std::string> GetCurrentAppVersion(){
        std::string url = std::format("/api/v1/GetCurrentAppVersion?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppVersion(std::string version){
        std::string url = std::format("/api/v1/GetAppVersion?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetCurrentAppManifest(){
        std::string url = std::format("/api/v1/GetCurrentAppManifest?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppManifest(std::string version){
        std::string url = std::format("/api/v1/GetAppManifest?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetCurrentAppFullPackage(){
        std::string url = std::format("/api/v1/GetCurrentAppFullPackage?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppFullPackage(std::string version){
        std::string url = std::format("/api/v1/GetAppFullPackage?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    ReturnWrapper DownloadCurrentFullPackage(std::string localSaveFilePath){
        std::string url = std::format("/api/v1/DownloadCurrentFullPackage?appname={}", m_AppName);
        apiRequestDownload(url, localSaveFilePath);
    }

    ReturnWrapper DownloadFullPackage(std::string version, std::string localSaveFilePath){
        std::string url = std::format("/api/v1/DownloadFullPackage?appname={}&appversion={}", m_AppName, version);
        apiRequestDownload(url, localSaveFilePath);
    }

    ReturnWrapper DownloadFileFromFullPackage(std::string version, std::string md5, std::string localSaveFilePath){
        std::string url = std::format("/api/v1/DownloadFileFromFullPackage?appname={}&appversion={}", m_AppName, version);
        apiRequestDownload(url, localSaveFilePath);
    }
    std::pair<ReturnWrapper, std::string> GetCurrentAppDifferencePackage(){
        std::string url = std::format("/api/v1/GetCurrentAppDifferencePackage?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppDifferencePackage(std::string version){
        std::string url = std::format("/api/v1/GetAppDifferencePackage?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    ReturnWrapper DownloadCurrentDifferencePackage(std::string localSaveFilePath){
        std::string url = std::format("/api/v1/GetCurrentAppDifferencePackage?appname={}", m_AppName);
        apiRequestDownload(url, localSaveFilePath);
    }

    ReturnWrapper DownloadDifferencePackage(std::string version, std::string localSaveFilePath){
        std::string url = std::format("/api/v1/GetAppDifferencePackage?appname={}&appversion={}", m_AppName, version);
        apiRequestDownload(url, localSaveFilePath);
    }

private:
    std::pair<ReturnWrapper, std::string> apiRequest(std::string requestUrl){
//        std::string host = "http://api.kirayuukiasuna.cloud";
//        std::string appname = "GameApp1";
//        std::string requestUrl = std::format("/api/v1/GetCurrentAppVersion?appname={}", appname);

        httplib::Client client(m_Host);
        httplib::Result result = client.Get(requestUrl);
        if (result && result->status == 200) {
            std::string content = result->body;
            std::cout << "Response content: " << content << std::endl;
            return {{true},content};

        } else {
            std::cout << "Request failed with status code: " << (result ? result->status : -1) << std::endl;
            return {{false, (result ? result->status : -1), "Http Request Error!"},""};
        }
    }

    std::pair<ReturnWrapper, std::string> apiRequestDownload(std::string requestUrl, std::string localSaveFilePath){
//        std::string host = "localhost:5200";
//        std::string filePath = "/WeatherForecast/download?filePath=Sanctum.7z";
//        std::string localFilePath = "test.zip";

        httplib::Client client(m_Host);
        httplib::Headers headers;
        if (std::filesystem::exists(localSaveFilePath)) {
            std::ifstream file(localSaveFilePath, std::ios::binary | std::ios::ate);
            if(file.is_open()){
                auto fileSize = file.tellg();
                headers.emplace("Range", "bytes=" + std::to_string(fileSize) + "-");
            }
        }

        auto res = client.Get(requestUrl, headers);
        if (res && res->status == 206) {
            std::ofstream file(localSaveFilePath, std::ios::binary | std::ios::app);
            file.write(res->body.data(), res->body.size());
            return {true, ""};
        } else if (res && res->status == 200) {
            std::ofstream file(localSaveFilePath, std::ios::binary | std::ios::trunc);
            file.write(res->body.data(), res->body.size());
            return {true, ""};
        } else {
            return {false, res ? std::to_string(res->status) : "Http No Response Error!"};
        }
    }

private:
    std::string m_Host;
    std::string m_AppName;
};

class FullPackageUpdate : public UpdateBase {
public:
    FullPackageUpdate(std::string appPath)
    : m_AppPath(appPath) {

    }

private:


private:
    std::string m_AppPath;

};


class UpdateCore{
public:
    void getCurrentVersion(){

    }

private:

    void download(){


    }
};

