#pragma once

enum class UpdateMode {
    Unknown,
    FullPackageUpdate,
    DifferencePackageUpdate,
    MultiVersionDifferencePackageUpdate,
    DifferenceUpdate
};

#ifdef WIN32
constexpr const char* hdiffzExecuable = "hdiffz.exe";
#elif defined __linux
constexpr const char* hdiffzExecuable = "./hdiffz";
#endif
