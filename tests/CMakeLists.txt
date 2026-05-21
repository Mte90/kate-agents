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