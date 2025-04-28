cmake_minimum_required(VERSION 3.26)
project(UniversalUpdateFrameworkClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ProjectHeader
        ../../Common/md5/md5.h
        ../../Common/util/util.hpp
        ../../Common/processutil/DeleteFileForce.hpp

        ../../Generator/UniversalUpdateFrameworkGenerator/src/TypeDefination/AppVersion.hpp
        ../../Generator/UniversalUpdateFrameworkGenerator/src/TypeDefination/AppManifest.hpp
        ../../Generator/UniversalUpdateFrameworkGenerator/src/TypeDefination/FullPackageManifest.hpp
        ../../Generator/UniversalUpdateFrameworkGenerator/src/TypeDefination/DifferencePackageManifest.hpp

        Network/ApiRequest.hpp

        UpdateLogic/FullPackageUpdate.hpp
        UpdateLogic/DifferencePackageUpdate.hpp
        UpdateLogic/VerifyRePatchSetPermission.hpp
        UpdateLogic/MultiVersionDifferencePackageUpdate.hpp

        UpdateCore/TypeDefinition.hpp
        UpdateCore/ErrorCode.hpp

        ../../Common/log/Log.cpp
        ../../ThirdParty/backward/backward.cpp
)

set(ProjectSource
        main.cpp
        ../../Common/processutil/DeleteFileForce.cpp
        ../../Common/md5/md5.cpp
)

if (WIN32)
    set(ProjectSource
            ${ProjectSource}
            ../../Common/processutil/Windows/NtdllBase.cpp
            ../../Common/processutil/Windows/NtdllExtensions.cpp
            ../../Common/processutil/Windows/FileLocksmith.cpp
    )
elseif (UNIX)

elseif (APPLE)

endif ()

add_executable(UniversalUpdateFrameworkClient ${ProjectHeader} ${ProjectSource})

target_include_directories(UniversalUpdateFrameworkClient
        PRIVATE
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
        ../../ThirdParty/magic_enum
        ../../Common/md5
        ../../Common/util
        ../../ThirdParty/asio-1.30.2/include
        ../../Generator/UniversalUpdateFrameworkGenerator/src
        ../../Common/processutil
        ../../ThirdParty/cinatra/include
        ../../ThirdParty/spdlog/include
        ../../ThirdParty/backward
        ../../Common/log
        ../../ThirdParty/openssl/include
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_directories(UniversalUpdateFrameworkClient
                PRIVATE
                ../../ThirdParty/openssl/lib/debug
                )
        target_link_libraries(UniversalUpdateFrameworkClient
                PRIVATE
                libssl
                libcrypto
                )
else()
        target_link_directories(UniversalUpdateFrameworkClient
                PRIVATE
                ../../ThirdParty/openssl/lib/release
                )
        target_link_libraries(UniversalUpdateFrameworkClient
                PRIVATE
                libssl
                libcrypto
                )
endif()

target_compile_definitions(UniversalUpdateFrameworkClient
        PRIVATE
        CINATRA_ENABLE_SSL
)

target_compile_options(UniversalUpdateFrameworkClient
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${GNU_BIG_OBJ_FLAG_ENABLE}>>:-Wa,-mbig-obj>)
