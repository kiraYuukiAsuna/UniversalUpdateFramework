#pragma once

#include <filesystem>
#include <string>

class ProcessUtil
{
public:
    static void TerminateProcessByFilePath(const std::string& filePath);
    static bool DeleteFileRecursiveForce(const std::string& filePath);
    static bool DeleteFileRecursiveForce(const std::filesystem::path& filePath);

};
