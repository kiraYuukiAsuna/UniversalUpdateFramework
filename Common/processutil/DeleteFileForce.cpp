#include "DeleteFileForce.hpp"

#include <filesystem>
#include <iostream>
#include <filesystem>

#ifdef _WIN32
#include "Windows/FileLocksmith.h"

std::wstring Str2Wstr(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

#elif defined __linux__

#elif defined __APPLE__

#endif

void ProcessUtil::TerminateProcessByFilePath(const std::string& filePath)
{
#ifdef _WIN32
    std::vector<std::wstring> path = {Str2Wstr(filePath)};
    auto process = find_processes_recursive(path);
    for (auto &p : process) {
        std::wcout << L"Process name: " << p.name << std::endl;
        std::wcout << L"Process id: " << p.pid << std::endl;
        std::wcout << L"Process user: " << p.user << std::endl;
        for (auto &f : p.files) {
            std::wcout << L"File: " << f << std::endl;
        }

        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)p.pid);
        if (hProcess != nullptr) {
            TerminateProcess(hProcess, 9);
            CloseHandle(hProcess);
            std::wcout << L"Process terminated" << std::endl;
        }
    }
#elif defined __linux__

#elif defined __APPLE__

#endif
}

bool ProcessUtil::DeleteFileRecursiveForce(const std::string& filePath) {
    if (std::filesystem::exists(std::filesystem::path(filePath))) {
        TerminateProcessByFilePath(filePath);
        std::error_code ec;
        std::filesystem::remove_all(filePath, ec);
        return !ec;
    }
    return true;
}

bool ProcessUtil::DeleteFileRecursiveForce(const std::filesystem::path& filePath) {
    return DeleteFileRecursiveForce(filePath.string());
}
