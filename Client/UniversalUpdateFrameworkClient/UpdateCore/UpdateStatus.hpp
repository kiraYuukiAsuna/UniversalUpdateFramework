#pragma once

#include <string>

enum class UpdateStatus {
    None,

    DownloadAppVersionFile,
    DownloadAppManifestFile,
    DownloadFullPackageManifestFile,
    DownloadFullPackageFile,
    DownloadDifferencePackageManifestFile,
    DownloadDifferencePackageFile,

    FullPackageUpdateInstalling,
    DifferencePackageUpdateInstalling,
    DifferenceUpdateInstalling,
    MutilVersionUpdateInstalling,
    VerifyingUpdate,
    RepatchingUpdate,

    VerifyingFile,
    DownloadingFile,
    CopyingFile,
    DeletingFile,
    UpdatingFile,
    CreatingFile,

    Warning,

    Completed,
    Failed,
};

struct UpdateStatusInfo {
    UpdateStatus status;

    std::string CurrentFileName;
    float CurerentProgress;

    int DownloadTotalSize;
    int DownloadCurrentSize;
    float DownloadCurrentSpeed;

    std::string WarningMessage;
    std::string ErrorMessage;
};
