#pragma once

#include <string>
#include <filesystem>

class FilePermission {
public:
    FilePermission(const std::filesystem::path& filePath) : filePath(filePath) {}

    auto ReadPermission() {
        std::filesystem::perms p = std::filesystem::status(filePath).permissions();

        auto permission = static_cast<uint16_t>(p);

        return permission;
    }

    bool WritePermission(uint16_t permission) {
        std::filesystem::perms p = static_cast<std::filesystem::perms>(permission);

        std::error_code ec;
        std::filesystem::permissions(filePath, p, ec);

        return !ec;
    }

private:
    std::filesystem::path filePath;

};