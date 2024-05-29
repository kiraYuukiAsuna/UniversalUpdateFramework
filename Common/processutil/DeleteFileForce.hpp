#pragma once

#include <filesystem>
#include <string>

class ProcessUtil
{
public:
    // it works when you files or subfolders in this folder are being used by another application.
    static void TerminateProcessByFilePath(const std::string& filePath);
    static void TerminateProcessByFilePath(const std::filesystem::path& filePath);

    // forceCloseUsingOpennedFileApplication: use it only when you want to delete a file that is being used by another application,
    // and also this operation is time consuming, so use it only when you are sure that the file is being used by another application.
    // it works when you delete a folder and files or subfolders in this folder are being used by another application.
    static bool DeleteFileRecursive(const std::string& filePath, bool forceCloseUsingOpennedFileApplication=false);
    static bool DeleteFileRecursive(const std::filesystem::path& filePath, bool forceCloseUsingOpennedFileApplication=false);

};
