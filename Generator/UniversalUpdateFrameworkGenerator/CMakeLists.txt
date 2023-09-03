cmake_minimum_required(VERSION 3.26)
project(UniversalUpdateFrameworkGenerator)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
)

set(ProjectHeader
    md5.h

)

set(ProjectSource
        main.cpp
        md5.cpp

)

add_executable(UniversalUpdateFrameworkGenerator ${ProjectHeader} ${ProjectSource})
