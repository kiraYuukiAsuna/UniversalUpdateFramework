cmake_minimum_required(VERSION 3.26)
project(UniversalUpdateFrameworkClient)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
)

set(ProjectHeader
    UpdateCore.h

)

set(ProjectSource
        main.cpp
        UpdateCore.cpp

)

add_executable(UniversalUpdateFrameworkClient ${ProjectHeader} ${ProjectSource})
