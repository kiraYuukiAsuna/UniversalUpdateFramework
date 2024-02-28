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
)

set(ProjectHeader
        ../../Common/md5/md5.h
        ../../Common/util/util.hpp

        Network/ApiRequest.hpp

        UpdateCore/AppManifest.hpp
        UpdateCore/AppVersion.hpp
        UpdateCore/DifferencePackageManifest.hpp
        UpdateCore/FullPackageManifest.hpp

        UpdateLogic/FullPackageUpdate.hpp
        UpdateLogic/DifferencePackageUpdate.hpp
        UpdateLogic/VerifyAndRePatch.hpp
        UpdateLogic/MultiVersionDifferencePackageUpdate.hpp

        UpdateCore/TypeDefinition.hpp
        UpdateCore/ErrorCode.hpp
)

set(ProjectSource
        main.cpp

        ../../Common/md5/md5.cpp

)

add_executable(UniversalUpdateFrameworkClient ${ProjectHeader} ${ProjectSource})
