find_package(Qt6Test REQUIRED)
find_package(KF6TextEditor REQUIRED)

# Enable testing
enable_testing()

# ConfigManager test
add_executable(test_configmanager test_configmanager.cpp
    ../src/configmanager.cpp
)
target_link_libraries(test_configmanager
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    KF6::TextEditor
)
add_test(NAME test_configmanager COMMAND test_configmanager)

# EditorContext test
add_executable(test_editorcontext test_editorcontext.cpp
    ../src/editorcontext.cpp
)
target_link_libraries(test_editorcontext
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    KF6::TextEditor
)
add_test(NAME test_editorcontext COMMAND test_editorcontext)

# ThreadJsonStorage test
add_executable(test_threadjsonstorage test_threadjsonstorage.cpp
    ../src/threadjson.cpp
)
set_target_properties(test_threadjsonstorage PROPERTIES AUTOMOC ON)
target_link_libraries(test_threadjsonstorage
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
)
add_test(NAME test_threadjsonstorage COMMAND test_threadjsonstorage)

# Chat UI Flow test
add_executable(test_chatuiflow test_chatuiflow.cpp)
set_target_properties(test_chatuiflow PROPERTIES AUTOMOC ON)
target_link_libraries(test_chatuiflow
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
)
add_test(NAME test_chatuiflow COMMAND test_chatuiflow)

## ToolRegistry test (disabled - header-only tools)
#add_executable(test_toolregistry test_toolregistry.cpp
#    ../src/toolregistry.cpp
#)
#target_link_libraries(test_toolregistry
#    Qt6::Test
#    Qt6::Core
#    Qt6::Widgets
#    Qt6::Concurrent
#    KF6::TextEditor
#)
#add_test(NAME test_toolregistry COMMAND test_toolregistry)

## AgentProfile test (disabled - complex dependencies)
#add_executable(test_agentprofile test_agentprofile.cpp
#    ../src/agentloop.cpp
#    ../src/openaiprovider.cpp
#    ../src/configmanager.cpp
#)
#target_link_libraries(test_agentprofile
#    Qt6::Test
#    Qt6::Core
#    Qt6::Widgets
#    Qt6::Concurrent
#    KF6::TextEditor
#)
#add_test(NAME test_agentprofile COMMAND test_agentprofile)

## ThreadView test (disabled - private methods)
#add_executable(test_threadview test_threadview.cpp
#    ../src/ui/threadview.cpp
#)
#target_link_libraries(test_threadview
#    Qt6::Test
#    Qt6::Core
#    Qt6::Widgets
#    Qt6::Concurrent
#)
#add_test(NAME test_threadview COMMAND test_threadview)

# === New comprehensive tests ===

# Retry and Abort tests
add_executable(test_retry_abort test_retry_abort.cpp
    ../src/llmprovider.h
    ../src/openaiprovider.cpp
)
set_target_properties(test_retry_abort PROPERTIES AUTOMOC ON)
target_include_directories(test_retry_abort PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_retry_abort
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_retry_abort COMMAND test_retry_abort)

# EditorContext advanced tests
add_executable(test_editorcontext_advanced test_editorcontext_advanced.cpp
    ../src/editorcontext.cpp
)
target_link_libraries(test_editorcontext_advanced
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    KF6::TextEditor
)
add_test(NAME test_editorcontext_advanced COMMAND test_editorcontext_advanced)

# AgentLoop tests
add_executable(test_agentloop test_agentloop.cpp
    ../src/llmprovider.h
    ../src/agentloop.cpp
    ../src/openaiprovider.cpp
    ../src/configmanager.cpp
    ../src/toolregistry.cpp
    ../src/permissionmanager.cpp
    ../src/threadstorage.cpp
    ../src/threadjson.cpp
    ../src/ghosttextprovider.cpp
    ../src/editorcontext.cpp
)
set_target_properties(test_agentloop PROPERTIES AUTOMOC ON)
target_link_libraries(test_agentloop
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Concurrent
    Qt6::Sql
    KF6::TextEditor
    KF6::I18n
)
target_include_directories(test_agentloop PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test_agentloop COMMAND test_agentloop)

# PermissionManager tests
add_executable(test_permissionmanager test_permissionmanager.cpp
    ../src/permissionmanager.cpp
)
set_target_properties(test_permissionmanager PROPERTIES AUTOMOC ON)
target_link_libraries(test_permissionmanager
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
)
add_test(NAME test_permissionmanager COMMAND test_permissionmanager)

# ConfigManager advanced tests
add_executable(test_configmanager_advanced test_configmanager_advanced.cpp
    ../src/configmanager.cpp
)
set_target_properties(test_configmanager_advanced PROPERTIES AUTOMOC ON)
target_link_libraries(test_configmanager_advanced
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    KF6::TextEditor
    KF6::ConfigWidgets
    KF6::I18n
)
add_test(NAME test_configmanager_advanced COMMAND test_configmanager_advanced)

# ThreadJson advanced tests
add_executable(test_threadjson_advanced test_threadjson_advanced.cpp
    ../src/threadjson.cpp
)
set_target_properties(test_threadjson_advanced PROPERTIES AUTOMOC ON)
target_link_libraries(test_threadjson_advanced
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
)
add_test(NAME test_threadjson_advanced COMMAND test_threadjson_advanced)

# Integration tests
add_executable(test_integration test_integration.cpp
    ../src/llmprovider.h
    ../src/agentloop.cpp
    ../src/openaiprovider.cpp
    ../src/configmanager.cpp
    ../src/toolregistry.cpp
    ../src/permissionmanager.cpp
    ../src/threadstorage.cpp
    ../src/threadjson.cpp
    ../src/ghosttextprovider.cpp
    ../src/editorcontext.cpp
)
set_target_properties(test_integration PROPERTIES AUTOMOC ON)
target_link_libraries(test_integration
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Concurrent
    Qt6::Sql
    KF6::TextEditor
    KF6::I18n
)
target_include_directories(test_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test_integration COMMAND test_integration)