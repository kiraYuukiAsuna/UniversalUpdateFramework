#pragma once

#include <vector>
#include <string>
#include <ranges>
#include <fstream>
#include <filesystem>

namespace util {

    inline std::vector<std::string> string_split(const std::string &s, char delim = ' ') {
        std::vector<std::string> tokens;
        for (auto token: s | std::views::split(delim)) {
            tokens.emplace_back(token.begin(), token.end());
        }
        return tokens;
    }

    inline bool saveToFile(const std::string &content, std::filesystem::path path) {
        std::ofstream outfile(path);
        if (!outfile.is_open()) {
            return false;
        }
        outfile << content;
        outfile.close();
        return true;
    }

}
