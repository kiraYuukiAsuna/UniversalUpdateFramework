#pragma once

#include <string>
#include <format>
#include <future>
#include <iostream>
#include <filesystem>
#include "httplib.h"
#include "TypeDefinition.h"

class ApiRequest {
public:
    ApiRequest(std::string host, std::string appname)
            : m_Host(host), m_AppName(appname) {

    }

    std::pair<ReturnWrapper, std::string> GetCurrentAppVersion() {
        std::string url = std::format("/api/v1/GetCurrentAppVersion?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppVersion(std::string version) {
        std::string url = std::format("/api/v1/GetAppVersion?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppVersionList() {
        std::string url = std::format("/api/v1/GetAppVersionList?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetCurrentAppManifest() {
        std::string url = std::format("/api/v1/GetCurrentAppManifest?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppManifest(std::string version) {
        std::string url = std::format("/api/v1/GetAppManifest?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetCurrentAppFullPackageManifest() {
        std::string url = std::format("/api/v1/GetCurrentAppFullPackage?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppFullPackageManifest(std::string version) {
        std::string url = std::format("/api/v1/GetAppFullPackage?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> DownloadCurrentFullPackage(std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadCurrentFullPackage?appname={}", m_AppName);
        size_t bytes_received = 0;
        size_t bytes_total = 0;

        auto future = std::async(std::launch::async, &ApiRequest::apiRequestDownload, this, url, localSaveFilePath,
                                 std::ref(bytes_received), std::ref(bytes_total));

        std::chrono::milliseconds timeout(100);
        while (future.wait_for(timeout) != std::future_status::ready) {
            checkProgress(bytes_received, bytes_total);
        }

        auto res = future.get();
        checkProgress(bytes_received, bytes_total);
        std::cout << std::endl;
        return res;
    }

    std::pair<ReturnWrapper, std::string> DownloadFullPackage(std::string version, std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadFullPackage?appname={}&appversion={}", m_AppName, version);
        size_t bytes_received = 0;
        size_t bytes_total = 0;

        auto future = std::async(std::launch::async, &ApiRequest::apiRequestDownload, this, url, localSaveFilePath,
                                 std::ref(bytes_received), std::ref(bytes_total));

        std::chrono::milliseconds timeout(100);
        while (future.wait_for(timeout) != std::future_status::ready) {
            checkProgress(bytes_received, bytes_total);
        }

        auto res = future.get();
        checkProgress(bytes_received, bytes_total);
        std::cout << std::endl;
        return res;
    }

    std::pair<ReturnWrapper, std::string>
    DownloadFileFromFullPackage(std::string version, std::string md5, std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadFileFromFullPackage?appname={}&appversion={}&md5={}", m_AppName,
                                      version, md5);
        size_t bytes_received = 0;
        size_t bytes_total = 0;

        auto future = std::async(std::launch::async, &ApiRequest::apiRequestDownload, this, url, localSaveFilePath,
                                 std::ref(bytes_received), std::ref(bytes_total));

        std::chrono::milliseconds timeout(100);
        while (future.wait_for(timeout) != std::future_status::ready) {
            checkProgress(bytes_received, bytes_total);
        }

        auto res = future.get();
        checkProgress(bytes_received, bytes_total);
        std::cout << std::endl;
        return res;
    }

    std::pair<ReturnWrapper, std::string> GetCurrentAppDifferencePackageManifest() {
        std::string url = std::format("/api/v1/GetCurrentAppDifferencePackage?appname={}", m_AppName);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> GetAppDifferencePackageManifest(std::string version) {
        std::string url = std::format("/api/v1/GetAppDifferencePackage?appname={}&appversion={}", m_AppName, version);
        return apiRequest(url);
    }

    std::pair<ReturnWrapper, std::string> DownloadCurrentDifferencePackage(std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadCurrentDifferencePackage?appname={}", m_AppName);
        size_t bytes_received = 0;
        size_t bytes_total = 0;

        auto future = std::async(std::launch::async, &ApiRequest::apiRequestDownload, this, url, localSaveFilePath,
                                 std::ref(bytes_received), std::ref(bytes_total));

        std::chrono::milliseconds timeout(100);
        while (future.wait_for(timeout) != std::future_status::ready) {
            checkProgress(bytes_received, bytes_total);
        }

        auto res = future.get();
        checkProgress(bytes_received, bytes_total);
        std::cout << std::endl;
        return res;
    }

    std::pair<ReturnWrapper, std::string>
    DownloadDifferencePackage(std::string version, std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadDifferencePackage?appname={}&appversion={}", m_AppName, version);
        size_t bytes_received = 0;
        size_t bytes_total = 0;

        auto future = std::async(std::launch::async, &ApiRequest::apiRequestDownload, this, url, localSaveFilePath,
                                 std::ref(bytes_received), std::ref(bytes_total));

        std::chrono::milliseconds timeout(100);
        while (future.wait_for(timeout) != std::future_status::ready) {
            checkProgress(bytes_received, bytes_total);
        }

        auto res = future.get();
        checkProgress(bytes_received, bytes_total);
        std::cout << std::endl;
        return res;
    }

private:
    std::pair<ReturnWrapper, std::string> apiRequest(std::string requestUrl) {
//        std::string host = "http://api.kirayuukiasuna.cloud";
//        std::string appname = "GameApp1";
//        std::string requestUrl = std::format("/api/v1/GetCurrentAppVersion?appname={}", appname);

        httplib::Client client(m_Host);
        httplib::Result result = client.Get(requestUrl);
        if (result && result->status == 200) {
            std::string content = result->body;
            std::cout << "Response content: " << content << std::endl;
            return {{true}, content};

        } else {
            std::cout << "Request failed with status code: " << (result ? result->status : -1) << std::endl;
            return {{false, ErrorCode::HttpRequestError,
                     "Http Request Error!" + std::string(magic_enum::enum_name(result.error()))}, ""};
        }
    }

    std::pair<ReturnWrapper, std::string>
    apiRequestDownload(std::string requestUrl, std::string localSaveFilePath, size_t &bytes_received,
                       size_t &bytes_total) {
//        const char* server_address = "http://192.168.0.114:5275";
//        const char* endpoint = "/api/v1/DownloadFullPackage?appname=GameApp1&appversion=1.0.0";

// Create an HTTP client instance
        httplib::Client client(m_Host);

        std::string filename = localSaveFilePath;
        std::ofstream file;
        bool file_opened = false;

// Send a GET request to the server
        httplib::Headers headers;
        bytes_received = getFileSize(filename);
        headers.emplace("Range", "bytes=" + std::to_string(bytes_received) + "-");

        auto res = client.Get(requestUrl, headers, [&](const httplib::Response &res) {
            // Get the filename from the Content-Disposition header
            if (!file_opened) {
//            auto header = res.get_header_value("Content-Disposition");
//            if (!header.empty()) {
//                auto pos = header.find("filename=");
//                if (pos != std::string::npos) {
//                    filename = header.substr(pos + 10);
//                    // Remove quotes from the filename
//                    filename.erase(std::remove(filename.begin(), filename.end(), '\"'), filename.end());
//                }
//            }

                bytes_total = std::stoul(res.get_header_value("Content-Length")) + bytes_received;

                // Open a file stream to save the downloaded file
                file.open(filename, std::ios::binary | std::ios::app);
                if (!file.is_open()) {
                    std::cerr << "Failed to open file for writing: " << filename << std::endl;
                    return false;
                }

                file_opened = true;
            }
            return true;
        }, [&](const char *data, size_t data_length) {
            // Write the received data to the file
            file.write(data, data_length);
            bytes_received += data_length;
            return true;
        });

// Check if the request was successful
        if (res && (res->status == 200 || res->status == 206)) {
            file.close();
//            std::cout << "File Downloaded Successfully: " << filename << std::endl;
            return {{true}, localSaveFilePath};
        } else {
            std::cerr << "Failed To Download File: " << (res ? res->status : -1) << std::endl;
            return {{false, ErrorCode::HttpResponseError,
                     "Http Response Error!" + std::string(magic_enum::enum_name(res.error()))}, ""};
        }
    }

    size_t getFileSize(const std::string &filePath) {
        if (std::filesystem::exists(filePath)) {
            std::filesystem::directory_entry path(filePath);
            return path.file_size();
        }
        return 0;
    }

    void checkProgress(size_t received, size_t total) {
        printProgressBar(received, total);
    }

    void printProgressBar(size_t received, size_t total) {
        const int bar_width = 50; // 进度条宽度

        // 计算进度百分比
        double progress = static_cast<double>(received) / (total ? (double) total : 1.0);

        // 计算已完成的进度条宽度
        int completed_width = static_cast<int>(bar_width * progress);

        std::cout << "[";
        for (int i = 0; i < bar_width; ++i) {
            if (i < completed_width) {
                std::cout << "=";
            } else if (i == completed_width) {
                std::cout << ">";
            } else {
                std::cout << " ";
            }
        }

        std::cout << "] " << std::fixed << std::setprecision(2) << std::setw(6)
                  << (progress * 100.0) << "% - [" << std::setw(5) << received / 1000
                  << "/" << std::setw(5) << total / 1000 << " kB" << "]" << "\r";

        std::cout.flush();
    }

private:
    std::string m_Host;
    std::string m_AppName;
};
