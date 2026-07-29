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
    Qt6::Concurrent
)
add_test(NAME test_chatuiflow COMMAND test_chatuiflow)

# ThreadView streaming test
add_executable(test_threadview_streaming test_threadview_streaming.cpp
    ../src/ui/threadview.cpp
    ../src/ui/syntaxhighlighter.cpp
)
set_target_properties(test_threadview_streaming PROPERTIES AUTOMOC ON)
target_link_libraries(test_threadview_streaming
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    KF6::I18n
)
target_include_directories(test_threadview_streaming PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test_threadview_streaming COMMAND test_threadview_streaming)

# ThreadView UI test
add_executable(test_threadview_ui test_threadview_ui.cpp
    ../src/ui/threadview.cpp
    ../src/ui/syntaxhighlighter.cpp
)
set_target_properties(test_threadview_ui PROPERTIES AUTOMOC ON)
target_link_libraries(test_threadview_ui
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    KF6::I18n
)
target_include_directories(test_threadview_ui PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test_threadview_ui COMMAND test_threadview_ui)

# SyntaxHighlighter test
add_executable(test_syntaxhighlighter test_syntaxhighlighter.cpp
    ../src/ui/syntaxhighlighter.cpp
)
set_target_properties(test_syntaxhighlighter PROPERTIES AUTOMOC ON)
target_include_directories(test_syntaxhighlighter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_syntaxhighlighter
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
)
add_test(NAME test_syntaxhighlighter COMMAND test_syntaxhighlighter)

# Retry and Abort test
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
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_retry_abort COMMAND test_retry_abort)

# EditorContext advanced test
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

# AgentLoop test
add_executable(test_agentloop test_agentloop.cpp
    ../src/llmprovider.h
    ../src/agentloop.cpp
    ../src/openaiprovider.cpp
    ../src/configmanager.cpp
    ../src/toolregistry.cpp
    ../src/permissionmanager.cpp
    ../src/threadstorage.cpp
    ../src/threadjson.cpp
    ../src/editorcontext.cpp
    ../src/auditlogger.cpp
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

# PermissionManager test
add_executable(test_permissionmanager test_permissionmanager.cpp
    ../src/permissionmanager.cpp
)
set_target_properties(test_permissionmanager PROPERTIES AUTOMOC ON)
target_link_libraries(test_permissionmanager
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
)
add_test(NAME test_permissionmanager COMMAND test_permissionmanager)

# ConfigManager advanced test
add_executable(test_configmanager_advanced test_configmanager_advanced.cpp
    ../src/configmanager.cpp
)
set_target_properties(test_configmanager_advanced PROPERTIES AUTOMOC ON)
target_link_libraries(test_configmanager_advanced
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    KF6::TextEditor
    KF6::ConfigWidgets
    KF6::I18n
)
add_test(NAME test_configmanager_advanced COMMAND test_configmanager_advanced)

# ThreadJson advanced test
add_executable(test_threadjson_advanced test_threadjson_advanced.cpp
    ../src/threadjson.cpp
)
set_target_properties(test_threadjson_advanced PROPERTIES AUTOMOC ON)
target_link_libraries(test_threadjson_advanced
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
)
add_test(NAME test_threadjson_advanced COMMAND test_threadjson_advanced)

# Integration test
add_executable(test_integration test_integration.cpp
    ../src/llmprovider.h
    ../src/agentloop.cpp
    ../src/openaiprovider.cpp
    ../src/configmanager.cpp
    ../src/toolregistry.cpp
    ../src/permissionmanager.cpp
    ../src/threadstorage.cpp
    ../src/threadjson.cpp
    ../src/editorcontext.cpp
    ../src/auditlogger.cpp
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

# Streaming chunk test
add_executable(test_streaming_chunk test_streaming_chunk.cpp)
target_link_libraries(test_streaming_chunk
    Qt6::Test
    Qt6::Core
)
add_test(NAME test_streaming_chunk COMMAND test_streaming_chunk)

# AgentLoop flow test
add_executable(test_agentloop_flow test_agentloop_flow.cpp)
set_target_properties(test_agentloop_flow PROPERTIES AUTOMOC ON)
target_link_libraries(test_agentloop_flow
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_agentloop_flow COMMAND test_agentloop_flow)

# AgentLoop utils test
add_executable(test_agentloop_utils test_agentloop_utils.cpp)
set_target_properties(test_agentloop_utils PROPERTIES AUTOMOC ON)
target_link_libraries(test_agentloop_utils
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_agentloop_utils COMMAND test_agentloop_utils)

# Basics test
add_executable(test_basics test_basics.cpp)
set_target_properties(test_basics PROPERTIES AUTOMOC ON)
target_link_libraries(test_basics
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_basics COMMAND test_basics)

# Concurrency test
add_executable(test_concurrency test_concurrency.cpp)
set_target_properties(test_concurrency PROPERTIES AUTOMOC ON)
target_link_libraries(test_concurrency
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_concurrency COMMAND test_concurrency)

# Config integration test
add_executable(test_config_integration test_config_integration.cpp ../src/configmanager.cpp ../src/permissionmanager.cpp)
set_target_properties(test_config_integration PROPERTIES AUTOMOC ON)
target_link_libraries(test_config_integration
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::ConfigCore
)
add_test(NAME test_config_integration COMMAND test_config_integration)

# ConfigManager detailed test
add_executable(test_configmanager_detailed test_configmanager_detailed.cpp ../src/configmanager.cpp)
set_target_properties(test_configmanager_detailed PROPERTIES AUTOMOC ON)
target_link_libraries(test_configmanager_detailed
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::ConfigCore
)
add_test(NAME test_configmanager_detailed COMMAND test_configmanager_detailed)

# ConfigManager utils test
add_executable(test_configmanager_utils test_configmanager_utils.cpp ../src/configmanager.cpp)
set_target_properties(test_configmanager_utils PROPERTIES AUTOMOC ON)
target_link_libraries(test_configmanager_utils
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::ConfigCore
)
add_test(NAME test_configmanager_utils COMMAND test_configmanager_utils)

# ContextMenu events test
add_executable(test_contextmenu_events test_contextmenu_events.cpp)
set_target_properties(test_contextmenu_events PROPERTIES AUTOMOC ON)
target_link_libraries(test_contextmenu_events
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::TextEditor
)
add_test(NAME test_contextmenu_events COMMAND test_contextmenu_events)

# ContextMenuHandler test
add_executable(test_contextmenuhandler test_contextmenuhandler.cpp)
set_target_properties(test_contextmenuhandler PROPERTIES AUTOMOC ON)
target_link_libraries(test_contextmenuhandler
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::TextEditor
)
add_test(NAME test_contextmenuhandler COMMAND test_contextmenuhandler)

# DiffPreviewDialog test
add_executable(test_diffpreviewdialog test_diffpreviewdialog.cpp ../src/ui/diffpreviewdialog.cpp)
set_target_properties(test_diffpreviewdialog PROPERTIES AUTOMOC ON)
target_link_libraries(test_diffpreviewdialog
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_diffpreviewdialog COMMAND test_diffpreviewdialog)

# GrepTool test
add_executable(test_grep_tool test_grep_tool.cpp
    ../src/toolregistry.cpp
    ../src/auditlogger.cpp
    ../src/permissionmanager.cpp
    ../src/tools/greptool.h
)
set_target_properties(test_grep_tool PROPERTIES AUTOMOC ON)
target_link_libraries(test_grep_tool
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_grep_tool COMMAND test_grep_tool)

# LLM data test
add_executable(test_llm_data test_llm_data.cpp)
set_target_properties(test_llm_data PROPERTIES AUTOMOC ON)
target_link_libraries(test_llm_data
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_llm_data COMMAND test_llm_data)

# LLM provider test
add_executable(test_llm_provider test_llm_provider.cpp
    ../src/llmprovider.h
    ../src/openaiprovider.cpp
)
set_target_properties(test_llm_provider PROPERTIES AUTOMOC ON)
target_link_libraries(test_llm_provider
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
target_include_directories(test_llm_provider PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
add_test(NAME test_llm_provider COMMAND test_llm_provider)

# Message deletion test
add_executable(test_message_deletion test_message_deletion.cpp
    ../src/ui/threadview.cpp
    ../src/ui/syntaxhighlighter.cpp
)
set_target_properties(test_message_deletion PROPERTIES AUTOMOC ON)
target_link_libraries(test_message_deletion
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::TextEditor
)
target_include_directories(test_message_deletion PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
add_test(NAME test_message_deletion COMMAND test_message_deletion)

# More tools test
add_executable(test_more_tools test_more_tools.cpp
    ../src/toolregistry.cpp
    ../src/auditlogger.cpp
    ../src/permissionmanager.cpp
    ../src/ui/diffpreviewdialog.cpp
    ../src/tools/greptool.h
    ../src/tools/applydifftool.h
    ../src/tools/findpathtool.h
    ../src/tools/editfiletool.h
)
set_target_properties(test_more_tools PROPERTIES AUTOMOC ON)
target_link_libraries(test_more_tools
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::TextEditor
)
add_test(NAME test_more_tools COMMAND test_more_tools)

# Panel persistence test
add_executable(test_panel_persistence test_panel_persistence.cpp ../src/configmanager.cpp ../src/permissionmanager.cpp ../src/auditlogger.cpp)
set_target_properties(test_panel_persistence PROPERTIES AUTOMOC ON)
target_include_directories(test_panel_persistence PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(test_panel_persistence
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::ConfigCore
)
add_test(NAME test_panel_persistence COMMAND test_panel_persistence)

# ReadFile tool test
add_executable(test_readfile_tool test_readfile_tool.cpp ../src/toolregistry.cpp ../src/auditlogger.cpp ../src/permissionmanager.cpp ../src/tools/readfiletool.h)
set_target_properties(test_readfile_tool PROPERTIES AUTOMOC ON)
target_link_libraries(test_readfile_tool
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_readfile_tool COMMAND test_readfile_tool)

# Sandbox security test
add_executable(test_sandbox_security test_sandbox_security.cpp)
set_target_properties(test_sandbox_security PROPERTIES AUTOMOC ON)
target_link_libraries(test_sandbox_security
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_sandbox_security COMMAND test_sandbox_security)

# Streaming UI test
add_executable(test_streaming_ui test_streaming_ui.cpp)
set_target_properties(test_streaming_ui PROPERTIES AUTOMOC ON)
target_link_libraries(test_streaming_ui
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_streaming_ui COMMAND test_streaming_ui)

# Terminal and WebSearch tools test
add_executable(test_terminal_websearch_tools test_terminal_websearch_tools.cpp ../src/toolregistry.cpp ../src/auditlogger.cpp ../src/permissionmanager.cpp ../src/tools/terminaltool.h ../src/tools/websearchtool.h)
set_target_properties(test_terminal_websearch_tools PROPERTIES AUTOMOC ON)
target_link_libraries(test_terminal_websearch_tools
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_terminal_websearch_tools COMMAND test_terminal_websearch_tools)

# Tool integration test
add_executable(test_tool_integration test_tool_integration.cpp)
set_target_properties(test_tool_integration PROPERTIES AUTOMOC ON)
target_link_libraries(test_tool_integration
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_tool_integration COMMAND test_tool_integration)

# ToolRegistry utils test
add_executable(test_toolregistry_utils test_toolregistry_utils.cpp ../src/toolregistry.cpp ../src/auditlogger.cpp ../src/permissionmanager.cpp ../src/tools/readfiletool.h ../src/tools/greptool.h)
set_target_properties(test_toolregistry_utils PROPERTIES AUTOMOC ON)
target_include_directories(test_toolregistry_utils PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/tools)
target_link_libraries(test_toolregistry_utils
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_toolregistry_utils COMMAND test_toolregistry_utils)

# Tools test
add_executable(test_tools test_tools.cpp ../src/toolregistry.cpp ../src/auditlogger.cpp ../src/permissionmanager.cpp ../src/ui/diffpreviewdialog.cpp ../src/ui/diffpreviewdialog.h ../src/tools/editfiletool.h ../src/tools/findpathtool.h ../src/tools/readfiletool.h ../src/tools/greptool.h ../src/tools/listdirectorytool.h)
set_target_properties(test_tools PROPERTIES AUTOMOC ON)
target_include_directories(test_tools PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/tools)
target_link_libraries(test_tools
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
)
add_test(NAME test_tools COMMAND test_tools)

# URLFetch diagnostic tools test
add_executable(test_urlfetch_diagnostic_tools test_urlfetch_diagnostic_tools.cpp ../src/toolregistry.cpp ../src/auditlogger.cpp ../src/permissionmanager.cpp ../src/tools/listdirectorytool.h ../src/tools/urlfetchtool.h ../src/tools/diagnosticstool.h ../src/tools/createdirectorytool.h)
set_target_properties(test_urlfetch_diagnostic_tools PROPERTIES AUTOMOC ON)
target_include_directories(test_urlfetch_diagnostic_tools PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/tools)
target_link_libraries(test_urlfetch_diagnostic_tools
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::TextEditor
)
add_test(NAME test_urlfetch_diagnostic_tools COMMAND test_urlfetch_diagnostic_tools)