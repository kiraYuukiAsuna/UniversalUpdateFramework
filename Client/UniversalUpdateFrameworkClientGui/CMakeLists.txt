cmake_minimum_required(VERSION 3.27)
project(UniversalUpdateFrameworkClientGui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

include_directories(
        ../../ThirdParty/magic_enum
        ../../ThirdParty/nlohmann_json
        ../../ThirdParty/httplib
        ../../ThirdParty/cxxopts
        ../../Common/util
        ../../Common/md5
        ../UniversalUpdateFrameworkClient
        ../../Generator/UniversalUpdateFrameworkGenerator/src
        ../../Common/processutil
        ../../ThirdParty/cinatra/include
        ../../ThirdParty/spdlog/include
        ../../ThirdParty/backward
        ../../Common/log
)

link_directories(
        .

)

set(QRESOURCE
        Resource/resources.qrc
)

file(GLOB_RECURSE UpdateCore_SRC_LIST
        "${PROJECT_SOURCE_DIR}/../UniversalUpdateFrameworkClient/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/../UniversalUpdateFrameworkClient/src/*.h"
        "${PROJECT_SOURCE_DIR}/../UniversalUpdateFrameworkClient/src/*.hpp"
)

file(GLOB_RECURSE SRC_LIST
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.ui"
        "${PROJECT_SOURCE_DIR}/src/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.hpp"
)

set(PROJECT_HEADERS

)

set(PROJECT_SOURCES
        main.cpp
        ../../Common/processutil/DeleteFileForce.cpp
        ../../Common/md5/md5.cpp

        ../../Common/log/Log.cpp
        ../../ThirdParty/backward/backward.cpp
)

if (WIN32)
    set(PROJECT_SOURCES
            ${PROJECT_SOURCES}
            app_win32.rc
            ../../Common/processutil/Windows/NtdllBase.cpp
            ../../Common/processutil/Windows/NtdllExtensions.cpp
            ../../Common/processutil/Windows/FileLocksmith.cpp
    )
elseif (UNIX)

elseif (APPLE)

endif ()

add_executable(${PROJECT_NAME} ${QRESOURCE} ${PROJECT_HEADERS} ${PROJECT_SOURCES} ${SRC_LIST} ${UpdateCore_SRC_LIST})

target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
#            "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
elseif (UNIX)

elseif (APPLE)

endif ()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
