#pragma once

#include <vector>
#include <string>
#include <ranges>

namespace util {

    inline std::vector<std::string> string_split(const std::string &s, char delim = ' ') {
        std::vector<std::string> tokens;
        for (auto token: s | std::views::split(delim)) {
            tokens.emplace_back(token.begin(), token.end());
        }
        return tokens;
    }

}
