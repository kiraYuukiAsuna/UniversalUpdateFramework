cmake_minimum_required(VERSION 3.26)
project(UniversalUpdateFrameworkClient)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
        ../../Common/md5
        ../../Common/util
)

set(ProjectHeader
        UpdateCore.h
        ../../Common/md5/md5.h
        ../../Common/util/util.hpp

)

set(ProjectSource
        main.cpp
        UpdateCore.cpp
        ../../Common/md5/md5.cpp

)

add_executable(UniversalUpdateFrameworkClient ${ProjectHeader} ${ProjectSource})
