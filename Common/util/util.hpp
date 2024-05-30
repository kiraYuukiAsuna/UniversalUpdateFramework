#pragma once

#include <vector>
#include <string>
#include <ranges>
#include <fstream>
#include <filesystem>

namespace util {
    inline std::vector<std::string> string_split(const std::string&s, char delim = ' ') {
        std::vector<std::string> tokens;
        for (auto token: s | std::views::split(delim)) {
            tokens.emplace_back(token.begin(), token.end());
        }
        return tokens;
    }

    inline std::string subreplace(std::string resource_str, std::string sub_str, std::string new_str) {
        std::string::size_type pos = 0;
        while ((pos = resource_str.find(sub_str)) != std::string::npos) //替换所有指定子串
        {
            resource_str.replace(pos, sub_str.length(), new_str);
        }
        return resource_str;
    }

    inline bool saveToFile(const std::string&content, std::filesystem::path path) {
        std::ofstream outfile(path);
        if (!outfile.is_open()) {
            return false;
        }
        outfile << content;
        outfile.close();
        return true;
    }

    // Function to convert uint64 timestamp to date string
    inline std::string timestampToString(uint64_t timestamp) {
        std::time_t timeT = timestamp;
        std::tm* timeStruct = std::localtime(&timeT);

        char buffer[128];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeStruct);

        return buffer;
    }

    // Function to convert date string to uint64 timestamp
    inline uint64_t stringToTimestamp(const std::string&time_str) {
        std::tm tm = {};
        std::stringstream ss(time_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        std::time_t time = std::mktime(&tm);
        if (time == -1) {
            return 0;
        }
        return static_cast<uint64_t>(time);
    }
}
