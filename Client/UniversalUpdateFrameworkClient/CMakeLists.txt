cmake_minimum_required(VERSION 3.26)
project(UniversalUpdateFrameworkClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
        ../../Common/md5
        ../../Common/util
        ../../Common/magic_enum
)

set(ProjectHeader
        ../../Common/md5/md5.h
        ../../Common/util/util.hpp

        Network/ApiRequest.h
        UpdateCore/AppManifest.h
        UpdateCore/AppVersion.h
        UpdateCore/DifferencePackageManifest.h
        UpdateCore/FullPackageManifest.h
        UpdateCore/Version.h
        UpdateLogic/DifferencePackageUpdate.h
        UpdateLogic/FullpackageUpdate.h
        TypeDefinition.h
        ErrorCode.hpp
        UpdateLogic/verifyAndRePatch.h
        UpdateLogic/MultiVersionDifferencePackageUpdate.h

)

set(ProjectSource
        main.cpp
        ../../Common/md5/md5.cpp

        Network/ApiRequest.cpp
        UpdateCore/AppManifest.cpp
        UpdateCore/AppVersion.cpp
        UpdateCore/DifferencePackageManifest.cpp
        UpdateCore/FullPackageManifest.cpp
        UpdateCore/Version.cpp
        UpdateLogic/DifferencePackageUpdate.cpp
        UpdateLogic/FullpackageUpdate.cpp
        UpdateLogic/verifyAndRePatch.cpp
        UpdateLogic/MultiVersionDifferencePackageUpdate.cpp

)

add_executable(UniversalUpdateFrameworkClient ${ProjectHeader} ${ProjectSource})
