#pragma once

#include <cstdint>
#include "magic_enum.hpp"

enum class ErrorCode : int32_t {
    NoError = 0,
    HttpRequestError,
    HttpResponseError,
    RemoveOldVersionDirFailed,
    CopyNewVersionDirFailed,
    CreateAppDirFailed,
    DeleteDownloadDirFailed,
    DeleteFileFailed,
    DownloadFileFailed,
    AccessFileFailed,
    CopyFileFailed,
    OpenConfigFileFailed,
    OpenFileFailed,
    CreateDirFailed
};
