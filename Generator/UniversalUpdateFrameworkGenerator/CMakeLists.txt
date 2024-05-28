cmake_minimum_required(VERSION 3.26)
project(UniversalUpdateFrameworkGenerator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
        ../../Common/md5
)

set(ProjectHeader
        ../../Common/md5/md5.h

)

set(ProjectSource
        main.cpp
        ../../Common/md5/md5.cpp

)

add_executable(UniversalUpdateFrameworkGenerator ${ProjectHeader} ${ProjectSource})
