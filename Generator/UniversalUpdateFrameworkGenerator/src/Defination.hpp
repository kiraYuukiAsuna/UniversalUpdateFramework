#pragma once

#include <string>

struct PackageBuildInfo {
    std::string PackageMode;
    std::string oldPath;
    std::string newPath;
    std::string appname;
    std::string channel;
    std::string platform;
    std::string appversion;
    std::string appbeforeversion;
};
