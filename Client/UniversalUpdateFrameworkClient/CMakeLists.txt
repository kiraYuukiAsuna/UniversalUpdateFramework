cmake_minimum_required(VERSION 3.26)
project(UniversalUpdateFrameworkClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
        ../../ThirdParty/magic_enum
        ../../Common/md5
        ../../Common/util
        ../../ThirdParty/asio-1.30.2/include
        ../../Generator/UniversalUpdateFrameworkGenerator/src
        ../../Common/processutil
)

set(ProjectHeader
        ../../Common/md5/md5.h
        ../../Common/util/util.hpp

        ../../Generator/UniversalUpdateFrameworkGenerator/src/AppVersion.hpp
        ../../Generator/UniversalUpdateFrameworkGenerator/src/AppManifest.hpp
        ../../Generator/UniversalUpdateFrameworkGenerator/src/FullPackageManifest.hpp
        ../../Generator/UniversalUpdateFrameworkGenerator/src/DifferencePackageManifest.hpp

        Network/ApiRequest.hpp

        UpdateLogic/FullPackageUpdate.hpp
        UpdateLogic/DifferencePackageUpdate.hpp
        UpdateLogic/VerifyAndRePatch.hpp
        UpdateLogic/MultiVersionDifferencePackageUpdate.hpp

        UpdateCore/TypeDefinition.hpp
        UpdateCore/ErrorCode.hpp
)

set(ProjectSource
        main.cpp
        ../../Common/processutil/DeleteFileForce.cpp
        ../../Common/processutil/Windows/NtdllBase.cpp
        ../../Common/processutil/Windows/NtdllExtensions.cpp
        ../../Common/processutil/Windows/FileLocksmith.cpp

        ../../Common/md5/md5.cpp

)

add_executable(UniversalUpdateFrameworkClient ${ProjectHeader} ${ProjectSource})
