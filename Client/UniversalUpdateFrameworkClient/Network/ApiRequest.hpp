#pragma once

#include <string>
#include <format>
#include <future>
#include <iostream>
#include <filesystem>
#include "httplib.h"
#include "UpdateCore/TypeDefinition.hpp"
#include <cinatra.hpp>
#include <Log.h>

class ApiRequest {
public:
    ApiRequest(std::string host, std::string appname, std::string channel, std::string platform)
        : m_Host(host), m_AppName(appname), m_Channel(channel), m_Platform(platform) {
    }

    // async_simple::coro::Lazy<std::pair<ReturnWrapper, std::string>>
    // apiRequestDownload(std::string api, std::string localSaveFilePath, size_t &bytes_received,
    //                    size_t &bytes_total) {
    //     cinatra::coro_http_client client{};
    //     std::string uri = std::format("{}{}", m_Host, api);
    //
    //     std::error_code ec{};
    //     std::filesystem::remove(localSaveFilePath, ec);

    //     auto result = co_await client.async_download(uri, localSaveFilePath);
    //     if(!result.net_err && (result.status == 200 || result.status == 206)) {
    //         std::pair<ReturnWrapper, std::string> ret{{true}, localSaveFilePath};
    //         co_return ret;
    //     } else {
    //         std::pair<ReturnWrapper, std::string> ret{{false, ErrorCode::HttpResponseError,
    //                  "Http Response Error!" + result.net_err.message()}, ""};
    //         co_return ret;
    //     }
    // }

    void mergeFiles(const std::vector<std::string>&fileNames, const std::string&outputFileName) {
        std::ofstream outputFile(outputFileName, std::ios::binary);

        for (const auto&fileName: fileNames) {
            std::ifstream inputFile(fileName, std::ios::binary);
            outputFile << inputFile.rdbuf();
        }
    }

    async_simple::coro::Lazy<std::pair<ReturnWrapper, std::string>>
    apiRequestDownload(std::string api, std::string localSaveFilePath, size_t&bytes_received,
                       size_t&bytes_total) {
        cinatra::coro_http_client client{};
        std::string uri = std::format("{}{}", m_Host, api);

        std::error_code ec{};
        std::filesystem::remove(localSaveFilePath, ec);

        // Send a HEAD request
        auto head_result = co_await client.async_head(uri);
        if (head_result.net_err || head_result.status != 200) {
            SEELE_INFO_TAG(__func__, "{}", "Http Response Error!" + head_result.net_err.message());
            std::pair<ReturnWrapper, std::string> ret{
                {
                    false, ErrorCode::HttpResponseError,
                    "Http Response Error!" + head_result.net_err.message()
                },
                ""
            };
            co_return ret;
        }

        auto iter = std::find_if(head_result.resp_headers.begin(), head_result.resp_headers.end(),
                                 [](const cinatra::http_header&header) {
                                     return header.name == "Content-Length";
                                 });
        if (iter == head_result.resp_headers.end()) {
            SEELE_INFO_TAG(__func__, "{}", "Http Response Error! Content-Length not found!");

            std::pair<ReturnWrapper, std::string> ret{
                {
                    false, ErrorCode::HttpResponseError,
                    "Http Response Error! Content-Length not found!"
                },
                ""
            };
            co_return ret;
        }
        bytes_total = std::stoul(std::string(iter->value));

        std::vector<std::string> rangeFiles;

        // Download the file in chunks of 100MB
        const size_t chunk_size = 100 * 1024 * 1024; // 100MB
        for (size_t start = 0; start < bytes_total; start += chunk_size) {
            size_t end = std::min(start + chunk_size - 1, bytes_total - 1);
            SEELE_INFO_TAG(__func__, "{}",
                           "Download Range: " + std::to_string(start) + "-" + std::to_string(end) + "/" + std::to_string
                           (bytes_total));
            std::string rangeFilePath = localSaveFilePath + "_range=" + std::to_string(start) + "-" + std::to_string(end);
            auto result = co_await client.async_download(uri, rangeFilePath, std::to_string(start) + "-" + std::to_string(end));
            if (!result.net_err && result.status == 206) {
                bytes_received += end - start + 1;

                std::ofstream ofstream(rangeFilePath, std::ios::binary);
                if(!ofstream.is_open()) {
                    SEELE_INFO_TAG(__func__, "{}", "Open File Error! File:" + rangeFilePath);
                    std::pair<ReturnWrapper, std::string> ret{
                        {
                            false, ErrorCode::AccessFileFailed,
                            "Open File Error! File:" + rangeFilePath
                        },
                        ""
                    };
                    co_return ret;
                }

                ofstream << result.resp_body;
                ofstream.close();

                rangeFiles.push_back(rangeFilePath);
            }
            else {
                SEELE_INFO_TAG(__func__, "{}", "Http Response Error!" + result.net_err.message());
                std::pair<ReturnWrapper, std::string> ret{
                    {
                        false, ErrorCode::HttpResponseError,
                        "Http Response Error!" + result.net_err.message()
                    },
                    ""
                };
                co_return ret;
            }
        }

        mergeFiles(rangeFiles, localSaveFilePath);

        std::pair<ReturnWrapper, std::string> ret{{true}, localSaveFilePath};
        SEELE_INFO_TAG(__func__, "Download file success! File: {}", localSaveFilePath);
        co_return ret;
    }

    async_simple::coro::Lazy<std::pair<ReturnWrapper, std::string>> httpRequestGet(std::string api) {
        cinatra::coro_http_client client{};
        std::string url = std::format("{}{}", m_Host, api);
        auto result = co_await client.async_get(url);

        if (!result.net_err && result.status == 200) {
            std::string content = std::string{result.resp_body};
            SEELE_INFO_TAG(__func__, "Response content: {}", content);

            std::pair<ReturnWrapper, std::string> ret{{true}, content};
            co_return ret;
        }
        else {
            SEELE_INFO_TAG(__func__, "Http Request Error! Error: {}", result.net_err.message());
            std::pair<ReturnWrapper, std::string> ret{
                {
                    false, ErrorCode::HttpRequestError,
                    "Http Request Error!" + result.net_err.message()
                },
                ""
            };
            co_return ret;
        }
    }

    auto GetCurrentAppVersion() {
        std::string url = std::format("/api/v1/GetCurrentAppVersion?appname={}&channel={}&platform={}", m_AppName,
                                      m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto GetAppVersion(std::string version) {
        std::string url = std::format("/api/v1/GetAppVersion?appname={}&appversion={}&channel={}&platform={}",
                                      m_AppName, version, m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto GetAppVersionList() {
        std::string url = std::format("/api/v1/GetAppVersionList?appname={}&channel={}&platform={}", m_AppName,
                                      m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto GetCurrentAppManifest() {
        std::string url = std::format("/api/v1/GetCurrentAppManifest?appname={}&channel={}&platform={}", m_AppName,
                                      m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto GetAppManifest(std::string version) {
        std::string url = std::format("/api/v1/GetAppManifest?appname={}&appversion={}&channel={}&platform={}",
                                      m_AppName, version, m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto GetCurrentAppFullPackageManifest() {
        std::string url = std::format("/api/v1/GetCurrentAppFullPackage?appname={}&channel={}&platform={}", m_AppName,
                                      m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto GetAppFullPackageManifest(std::string version) {
        std::string url = std::format("/api/v1/GetAppFullPackage?appname={}&appversion={}&channel={}&platform={}",
                                      m_AppName, version, m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto DownloadCurrentFullPackage(std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadCurrentFullPackage?appname={}&channel={}&platform={}", m_AppName,
                                      m_Channel, m_Platform);
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

    auto DownloadFullPackage(std::string version, std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadFullPackage?appname={}&appversion={}&channel={}&platform={}",
                                      m_AppName, version, m_Channel, m_Platform);
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

    auto
    DownloadFileFromFullPackage(std::string version, std::string md5, std::string localSaveFilePath) {
        std::string url = std::format(
            "/api/v1/DownloadFileFromFullPackage?appname={}&appversion={}&md5={}&channel={}&platform={}", m_AppName,
            version, md5, m_Channel, m_Platform);
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

    auto GetCurrentAppDifferencePackageManifest() {
        std::string url = std::format("/api/v1/GetCurrentAppDifferencePackage?appname={}&channel={}&platform={}",
                                      m_AppName, m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto GetAppDifferencePackageManifest(std::string version) {
        std::string url = std::format("/api/v1/GetAppDifferencePackage?appname={}&appversion={}&channel={}&platform={}",
                                      m_AppName, version, m_Channel, m_Platform);
        return httpRequestGet(url);
    }

    auto DownloadCurrentDifferencePackage(std::string localSaveFilePath) {
        std::string url = std::format("/api/v1/DownloadCurrentDifferencePackage?appname={}&channel={}&platform={}",
                                      m_AppName, m_Channel, m_Platform);
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

    auto
    DownloadDifferencePackage(std::string version, std::string localSaveFilePath) {
        std::string url = std::format(
            "/api/v1/DownloadDifferencePackage?appname={}&appversion={}&channel={}&platform={}", m_AppName, version,
            m_Channel, m_Platform);
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
    size_t getFileSize(const std::string&filePath) {
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
        double progress = static_cast<double>(received) / (total ? (double)total : 1.0);

        // 计算已完成的进度条宽度
        int completed_width = static_cast<int>(bar_width * progress);

        std::cout << "[";
        for (int i = 0; i < bar_width; ++i) {
            if (i < completed_width) {
                std::cout << "=";
            }
            else if (i == completed_width) {
                std::cout << ">";
            }
            else {
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
    std::string m_Channel;
    std::string m_Platform;
};
