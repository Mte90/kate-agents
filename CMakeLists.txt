cmake_minimum_required(VERSION 3.25)

project(kateagentplugin VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Concurrent Sql)
find_package(ECM ${KF5_DEP_VERSION} QUIET REQUIRED NO_MODULE)

list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMSetupVersion)
include(FeatureSummary)

set(QT_MAJOR_VERSION "6")
set(QT_MIN_VERSION "6.5.0")
set(KF6_DEP_VERSION "6.0.0")
find_package(KF6 ${KF6_DEP_VERSION}
    REQUIRED COMPONENTS
        CoreAddons
        I18n
        ConfigWidgets
        TextEditor
        XmlGui
)

set(KSYNTAXHIGHLIGHTING_KF6_DIR "/usr/lib/x86_64-linux-gnu/cmake/KF6SyntaxHighlighting")
find_package(KF6SyntaxHighlighting CONFIG PATHS ${KSYNTAXHIGHLIGHTING_KF6_DIR} NO_DEFAULT_PATH)

option(BUILD_TESTS "Build tests" ON)

set(PROJECT_SOURCES
    src/kateagentplugin.cpp
    src/kateagentplugin.h
    src/kateagentplugin.json
    src/agentloop.h
    src/agentloop.cpp
    src/llmprovider.h
    src/openaiprovider.h
    src/openaiprovider.cpp
    src/permissionmanager.h
    src/permissionmanager.cpp
    src/configmanager.h
    src/configmanager.cpp
    src/editorcontext.h
    src/editorcontext.cpp
    src/toolregistry.h
    src/toolregistry.cpp
    src/ghosttextprovider.h
    src/ghosttextprovider.cpp
    src/threadstorage.h
    src/threadstorage.cpp
    src/contextmenhander.h
    src/contextmenhander.cpp
    src/threadjson.h
    src/threadjson.cpp
    src/tools/readfiletool.h
    src/tools/greptool.h
    src/tools/terminaltool.h
    src/tools/editfiletool.h
    src/tools/websearchtool.h
    src/tools/urlfetchtool.h
    src/tools/diagnosticstool.h
    src/tools/findpathtool.h
    src/tools/listdirectorytool.h
    src/tools/createdirectorytool.h
    src/ui/agentconfigpage.h
    src/ui/agentconfigpage.cpp
    src/ui/agentpanel.h
    src/ui/agentpanel.cpp
    src/ui/filementionpopup.h
    src/ui/filementionpopup.cpp
    src/ui/inputbar.h
    src/ui/inputbar.cpp
    src/ui/threadview.h
    src/ui/threadview.cpp
    src/ui/diffpreviewdialog.h
    src/ui/diffpreviewdialog.cpp
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

kcoreaddons_add_plugin(kateagentplugin INSTALL_NAMESPACE "kf6/ktexteditor")
target_include_directories(kateagentplugin PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_definitions(kateagentplugin PRIVATE TRANSLATION_DOMAIN="kateagentplugin")
target_link_libraries(kateagentplugin PRIVATE
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
target_sources(kateagentplugin PRIVATE ${PROJECT_SOURCES})

if(KF6SyntaxHighlighting_FOUND)
    target_link_libraries(kateagentplugin PRIVATE KF6::SyntaxHighlighting)
    message(STATUS "KSyntaxHighlighting: found")
else()
    message(WARNING "KSyntaxHighlighting not found - markdown highlighting disabled")
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
