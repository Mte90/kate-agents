cmake_minimum_required(VERSION 3.16)
project(kateagentplugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_EXCLUDE_PATH "src/kateagentplugin.cpp")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Concurrent Sql)
find_package(KF6 REQUIRED COMPONENTS CoreAddons I18n Config ConfigWidgets TextEditor XmlGui)

find_package(KSyntaxHighlighting CONFIG)

include(ECMSetupVersion)
include(ECMInstallIcons)
include(GenerateExportHeader)

ecm_setup_version(PROJECT VARIABLE_PREFIX KATEAGENTPLUGIN
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kateagentplugin_version.h"
)

option(BUILD_TESTS "Build tests" ON)

set(KATEAGENTPLUGIN_LIB_SRCS
    src/kateagentplugin.cpp
    src/kateagentplugin.h
    src/kateagentplugin.json
    src/llmprovider.h
    src/openaiprovider.cpp
    src/openaiprovider.h
    src/toolregistry.cpp
    src/toolregistry.h
    src/agentloop.cpp
    src/agentloop.h
    src/permissionmanager.cpp
    src/permissionmanager.h
    src/configmanager.cpp
    src/configmanager.h
    src/ui/threadview.cpp
    src/ui/threadview.h
    src/ui/inputbar.cpp
    src/ui/inputbar.h
    src/ui/agentpanel.cpp
    src/ui/agentpanel.h
    src/ui/filementionpopup.cpp
    src/ui/filementionpopup.h
    src/ui/agentconfigpage.cpp
    src/ui/agentconfigpage.h
    src/tools/readfiletool.h
    src/tools/greptool.h
    src/tools/terminaltool.h
    src/tools/editfiletool.h
    src/threadstorage.cpp
    src/threadstorage.h
    src/ghosttextprovider.cpp
    src/ghosttextprovider.h
)

add_library(kateagentplugin MODULE ${KATEAGENTPLUGIN_LIB_SRCS})

target_include_directories(kateagentplugin PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(kateagentplugin
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        Qt6::Concurrent
        Qt6::Sql
        KF6::CoreAddons
        KF6::I18n
        KF6::ConfigWidgets
        KF6::TextEditor
        KF6::XmlGui
)

if(KSyntaxHighlighting_FOUND)
    target_link_libraries(kateagentplugin PUBLIC KSyntaxHighlighting::KSyntaxHighlighting)
else()
    message(WARNING "KSyntaxHighlighting not found - markdown highlighting disabled")
endif()

generate_export_header(kateagentplugin EXPORT_FILE_NAME kateagentplugin_export.h)

install(TARGETS kateagentplugin LIBRARY DESTINATION ${KDE_PLUGINS_INSTALL_DIR}/ktexteditor)
install(FILES src/kateagentplugin.json DESTINATION ${KDE_PLUGINS_INSTALL_DIR}/ktexteditor)
install(PROGRAMS scripts/install.sh scripts/uninstall.sh DESTINATION bin)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_custom_target(install-plugin
    COMMAND ${CMAKE_COMMAND} -E echo "Installing Kate Agent Plugin to KF6 location..."
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kateagentplugin> "/usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/"
    COMMENT "Installing Kate Agent Plugin to system plugins (needs root)"
)

message(STATUS "")
message(STATUS "Kate Agent Plugin Configuration (KF6)")
message(STATUS "  Qt6:          ${Qt6_VERSION}")
message(STATUS "  KF6:          ${KF6_VERSION}")
message(STATUS "  Tests:        ${BUILD_TESTS}")
message(STATUS "")