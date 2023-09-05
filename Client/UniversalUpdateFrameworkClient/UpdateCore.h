#pragma once

#include <iostream>
#include <format>
#include "httplib.h"
#include "json.hpp"


class UpdateCore{
public:
    void getCurrentVersion(){
        std::string host = "http://api.kirayuukiasuna.cloud";
        std::string appname = "GameApp1";
        std::string requestUrl = std::format("/api/v1/GetCurrentAppVersion?appname={}", appname);

        httplib::Client client(host);
        httplib::Result result = client.Get(requestUrl);
        if (result && result->status == 200) {
            std::string content = result->body;
            std::cout << "Response content: " << content << std::endl;
        } else {
            std::cout << "Request failed with status code: " << (result ? result->status : -1) << std::endl;
        }
    }

private:

    void download(){
        std::string server = "localhost";
        int port = 5200;
        std::string filePath = "/WeatherForecast/download?filePath=Sanctum.7z";
        std::string localFilePath = "test.zip";

        httplib::Client client(server, port);
        httplib::Headers headers;
        if (std::ifstream(localFilePath)) {
            std::ifstream file(localFilePath, std::ios::binary | std::ios::ate);
            auto fileSize = file.tellg();
            headers.emplace("Range", "bytes=" + std::to_string(fileSize) + "-");
        }

        auto res = client.Get(filePath.c_str(), headers);
        if (res && res->status == 206) {
            std::ofstream file(localFilePath, std::ios::binary | std::ios::app);
            file.write(res->body.data(), res->body.size());
        } else if (res && res->status == 200) {
            std::ofstream file(localFilePath, std::ios::binary | std::ios::trunc);
            file.write(res->body.data(), res->body.size());
        }

    }
};

