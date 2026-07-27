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

# ThreadView test (streaming, markdown, messaggi)
# ThreadView test - REMOVED (file deleted)
# add_executable(test_threadview test_threadview.cpp
#     ../src/ui/threadview.cpp
#     ../src/ui/syntaxhighlighter.cpp
# )
# set_target_properties(test_threadview PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadview
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     KF6::I18n
# )
# target_include_directories(test_threadview PRIVATE ${CMAKE_SOURCE_DIR}/src)
# add_test(NAME test_threadview COMMAND test_threadview)

# Streaming tests (new)
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

# UI tests (new)
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

# === New comprehensive tests ===

# SyntaxHighlighter direct tests
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
    Qt6::Concurrent
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

# PermissionManager tests
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

# ConfigManager advanced tests
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

# ThreadJson advanced tests
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

# Streaming chunk tests (pipe separator removal)
add_executable(test_streaming_chunk test_streaming_chunk.cpp)
target_link_libraries(test_streaming_chunk
    Qt6::Test
    Qt6::Core
)
add_test(NAME test_streaming_chunk COMMAND test_streaming_chunk)
# test_agentloop_detailed
# add_executable(test_agentloop_detailed test_agentloop_detailed.cpp
# ../src/agentloop.cpp
# ../src/openaiprovider.cpp
# ../src/configmanager.cpp
# ../src/toolregistry.cpp
# ../src/permissionmanager.cpp
# ../src/threadstorage.cpp
# ../src/threadjson.cpp
# ../src/auditlogger.cpp
# ../src/editorcontext.cpp
# )
# set_target_properties(test_agentloop_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_agentloop_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Network
# Qt6::Concurrent
# KF6::I18n
# )
# add_test(NAME test_agentloop_detailed COMMAND test_agentloop_detailed)

# test_agentloop_flow
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

# test_agentloop_utils
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

# test_agentpanel_events - REMOVED (file deleted)
# add_executable(test_agentpanel_events test_agentpanel_events.cpp)
# set_target_properties(test_agentpanel_events PROPERTIES AUTOMOC ON)
# target_link_libraries(test_agentpanel_events
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_agentpanel_events COMMAND test_agentpanel_events)

# test_agentpanel_ui - REMOVED (file deleted)
# add_executable(test_agentpanel_ui test_agentpanel_ui.cpp)
# set_target_properties(test_agentpanel_ui PROPERTIES AUTOMOC ON)
# target_link_libraries(test_agentpanel_ui
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_agentpanel_ui COMMAND test_agentpanel_ui)

# test_agentpanel_ui_detailed
# add_executable(test_agentpanel_ui_detailed test_agentpanel_ui_detailed.cpp)
# set_target_properties(test_agentpanel_ui_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_agentpanel_ui_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_agentpanel_ui_detailed COMMAND test_agentpanel_ui_detailed)

# test_basics
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

# test_concurrency
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

# test_config_integration
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

# test_configmanager_detailed
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

# test_configmanager_utils
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

# test_contextmenu_events
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

# test_contextmenuhandler
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

# test_crash_scenarios - DISABLED: vtable linking issues with mock classes
# add_executable(test_crash_scenarios test_crash_scenarios.cpp)
# set_target_properties(test_crash_scenarios PROPERTIES AUTOMOC ON)
# target_link_libraries(test_crash_scenarios
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_crash_scenarios COMMAND test_crash_scenarios)

# test_diffpreviewdialog
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

# test_diffpreviewdialog_events - DISABLED: tests private implementation details
# add_executable(test_diffpreviewdialog_events test_diffpreviewdialog_events.cpp)
# set_target_properties(test_diffpreviewdialog_events PROPERTIES AUTOMOC ON)
# target_link_libraries(test_diffpreviewdialog_events
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_diffpreviewdialog_events COMMAND test_diffpreviewdialog_events)

# test_diffpreviewdialog_ui_detailed
# add_executable(test_diffpreviewdialog_ui_detailed test_diffpreviewdialog_ui_detailed.cpp)
# set_target_properties(test_diffpreviewdialog_ui_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_diffpreviewdialog_ui_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_diffpreviewdialog_ui_detailed COMMAND test_diffpreviewdialog_ui_detailed)

# test_editorcontext_detailed - DISABLED: tests private implementation details
# add_executable(test_editorcontext_detailed test_editorcontext_detailed.cpp)
# set_target_properties(test_editorcontext_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_editorcontext_detailed
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_editorcontext_detailed COMMAND test_editorcontext_detailed)

# test_filementionpopup_events - DISABLED: tests private implementation details
# add_executable(test_filementionpopup_events test_filementionpopup_events.cpp)
# set_target_properties(test_filementionpopup_events PROPERTIES AUTOMOC ON)
# target_link_libraries(test_filementionpopup_events
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_filementionpopup_events COMMAND test_filementionpopup_events)

# test_filementionpopup_ui_detailed
# add_executable(test_filementionpopup_ui_detailed test_filementionpopup_ui_detailed.cpp)
# set_target_properties(test_filementionpopup_ui_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_filementionpopup_ui_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_filementionpopup_ui_detailed COMMAND test_filementionpopup_ui_detailed)

# test_filementionpopup_utils - DISABLED: tests private implementation details
# add_executable(test_filementionpopup_utils test_filementionpopup_utils.cpp)
# set_target_properties(test_filementionpopup_utils PROPERTIES AUTOMOC ON)
# target_link_libraries(test_filementionpopup_utils
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_filementionpopup_utils COMMAND test_filementionpopup_utils)

# test_fuzzy_picker - DISABLED: missing ui/filementionpopup.h header
# add_executable(test_fuzzy_picker test_fuzzy_picker.cpp)
# set_target_properties(test_fuzzy_picker PROPERTIES AUTOMOC ON)
# target_link_libraries(test_fuzzy_picker
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_fuzzy_picker COMMAND test_fuzzy_picker)

# test_fuzzy_picker_unit - DISABLED: missing ui/filementionpopup.h header
# add_executable(test_fuzzy_picker_unit test_fuzzy_picker_unit.cpp)
# set_target_properties(test_fuzzy_picker_unit PROPERTIES AUTOMOC ON)
# target_link_libraries(test_fuzzy_picker_unit
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_fuzzy_picker_unit COMMAND test_fuzzy_picker_unit)

# test_grep_tool
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

# test_inputbar - DISABLED: accesses private class members, cannot be fixed without rewriting
# add_executable(test_inputbar test_inputbar.cpp)
# set_target_properties(test_inputbar PROPERTIES AUTOMOC ON)
# target_link_libraries(test_inputbar
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_inputbar COMMAND test_inputbar)

# test_inputbar_detailed
# add_executable(test_inputbar_detailed test_inputbar_detailed.cpp)
# set_target_properties(test_inputbar_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_inputbar_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_inputbar_detailed COMMAND test_inputbar_detailed)

# test_inputbar_events - DISABLED: accesses private class members, cannot be fixed without rewriting
# add_executable(test_inputbar_events test_inputbar_events.cpp)
# set_target_properties(test_inputbar_events PROPERTIES AUTOMOC ON)
# target_link_libraries(test_inputbar_events
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_inputbar_events COMMAND test_inputbar_events)

# test_inputbar_ui_detailed
# add_executable(test_inputbar_ui_detailed test_inputbar_ui_detailed.cpp)
# set_target_properties(test_inputbar_ui_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_inputbar_ui_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_inputbar_ui_detailed COMMAND test_inputbar_ui_detailed)

# test_llm_data
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

# test_llm_detailed
# add_executable(test_llm_detailed test_llm_detailed.cpp)
# set_target_properties(test_llm_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_llm_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_llm_detailed COMMAND test_llm_detailed)

# test_llm_provider
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

# test_message_deletion
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

# test_more_tools
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

# test_multi_component_flows - DISABLED: references non-existent ../src/engine/agent-worker.h
# add_executable(test_multi_component_flows test_multi_component_flows.cpp)
# set_target_properties(test_multi_component_flows PROPERTIES AUTOMOC ON)
# target_link_libraries(test_multi_component_flows
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_multi_component_flows COMMAND test_multi_component_flows)

# test_openai_provider_detailed
# add_executable(test_openai_provider_detailed test_openai_provider_detailed.cpp)
# set_target_properties(test_openai_provider_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_openai_provider_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_openai_provider_detailed COMMAND test_openai_provider_detailed)

# test_openaiprovider - DISABLED: accesses private members (m_baseUrl, m_apiKey)
# add_executable(test_openaiprovider test_openaiprovider.cpp)
# set_target_properties(test_openaiprovider PROPERTIES AUTOMOC ON)
# target_link_libraries(test_openaiprovider
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_openaiprovider COMMAND test_openaiprovider)

# test_panel_persistence
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

# test_permissionmanager_detailed
# add_executable(test_permissionmanager_detailed test_permissionmanager_detailed.cpp)
# set_target_properties(test_permissionmanager_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_permissionmanager_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_permissionmanager_detailed COMMAND test_permissionmanager_detailed)

# test_plugin_config
add_executable(test_plugin_config test_plugin_config.cpp ../src/configmanager.cpp ../src/permissionmanager.cpp ../src/auditlogger.cpp)
set_target_properties(test_plugin_config PROPERTIES AUTOMOC ON)
target_include_directories(test_plugin_config PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(test_plugin_config
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
    KF6::I18n
    KF6::ConfigCore
)
add_test(NAME test_plugin_config COMMAND test_plugin_config)

# test_readfile_tool
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

# test_sandbox_security
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

# test_storage_integration - DISABLED: uses non-existent LLMMessage::Role enum
# add_executable(test_storage_integration test_storage_integration.cpp)
# set_target_properties(test_storage_integration PROPERTIES AUTOMOC ON)
# target_include_directories(test_storage_integration PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
# target_link_libraries(test_storage_integration
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_storage_integration COMMAND test_storage_integration)
add_test(NAME test_storage_integration COMMAND test_storage_integration)

# test_streaming_ui
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

# test_stress_edge_cases - DISABLED: invalid C++ syntax (JSON-style initializers) + private member access
# add_executable(test_stress_edge_cases test_stress_edge_cases.cpp)
# set_target_properties(test_stress_edge_cases PROPERTIES AUTOMOC ON)
# target_link_libraries(test_stress_edge_cases
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_stress_edge_cases COMMAND test_stress_edge_cases)

# test_syntaxhighlighter_events - DISABLED: uses undeclared QTextEdit, calls non-existent rehighlight() (method is highlight())
# add_executable(test_syntaxhighlighter_events test_syntaxhighlighter_events.cpp)
# set_target_properties(test_syntaxhighlighter_events PROPERTIES AUTOMOC ON)
# target_link_libraries(test_syntaxhighlighter_events
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_syntaxhighlighter_events COMMAND test_syntaxhighlighter_events)

# test_terminal_websearch_tools
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

# test_thread_json_corruption - DISABLED: uses removed loadAllThreads() API + deprecated qrand()
# add_executable(test_thread_json_corruption test_thread_json_corruption.cpp)
# set_target_properties(test_thread_json_corruption PROPERTIES AUTOMOC ON)
# target_link_libraries(test_thread_json_corruption
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_thread_json_corruption COMMAND test_thread_json_corruption)

# test_thread_json_persistence - DISABLED: uses removed loadAllThreads() API
# add_executable(test_thread_json_persistence test_thread_json_persistence.cpp)
# set_target_properties(test_thread_json_persistence PROPERTIES AUTOMOC ON)
# target_link_libraries(test_thread_json_persistence
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_thread_json_persistence COMMAND test_thread_json_persistence)
# test_thread_persistence - DISABLED: uses non-existent ThreadJson class + broken JSON API
# add_executable(test_thread_persistence test_thread_persistence.cpp ../src/threadjson.cpp)
# set_target_properties(test_thread_persistence PROPERTIES AUTOMOC ON)
# target_include_directories(test_thread_persistence PRIVATE ${CMAKE_SOURCE_DIR}/src)
# target_link_libraries(test_thread_persistence
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_thread_persistence COMMAND test_thread_persistence)

# test_threadjson_detailed - DISABLED: accesses private members (s_currentProjectId)
# add_executable(test_threadjson_detailed test_threadjson_detailed.cpp)
# set_target_properties(test_threadjson_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadjson_detailed
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_threadjson_detailed COMMAND test_threadjson_detailed)

# test_threadjson_utils - DISABLED: accesses private members (getRepoName)
# add_executable(test_threadjson_utils test_threadjson_utils.cpp)
# set_target_properties(test_threadjson_utils PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadjson_utils
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_threadjson_utils COMMAND test_threadjson_utils)

# test_threadstorage - DISABLED: accesses private members (databasePath, m_currentProjectId)
# add_executable(test_threadstorage test_threadstorage.cpp)
# set_target_properties(test_threadstorage PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadstorage
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_threadstorage COMMAND test_threadstorage)

# test_threadstorage_detailed - DISABLED: accesses private members
# add_executable(test_threadstorage_detailed test_threadstorage_detailed.cpp)
# set_target_properties(test_threadstorage_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadstorage_detailed
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_threadstorage_detailed COMMAND test_threadstorage_detailed)

# test_threadview_detailed
# add_executable(test_threadview_detailed test_threadview_detailed.cpp)
# set_target_properties(test_threadview_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadview_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_threadview_detailed COMMAND test_threadview_detailed)

# test_threadview_events - DISABLED: accesses private members
# add_executable(test_threadview_events test_threadview_events.cpp)
# set_target_properties(test_threadview_events PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadview_events
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_threadview_events COMMAND test_threadview_events)

# test_threadview_ui_detailed - DISABLED: accesses private members (m_allMessages)
# add_executable(test_threadview_ui_detailed test_threadview_ui_detailed.cpp)
# set_target_properties(test_threadview_ui_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_threadview_ui_detailed
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_threadview_ui_detailed COMMAND test_threadview_ui_detailed)

# test_tool_integration
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
# test_tool_permissions - DISABLED: uses non-existent PermissionManager API (allowedTools, setToolPermissions, removeTool)
# add_executable(test_tool_permissions test_tool_permissions.cpp ../src/permissionmanager.cpp ../src/toolregistry.cpp ../src/auditlogger.cpp ../src/tools/readfiletool.h ../src/tools/editfiletool.h)
# set_target_properties(test_tool_permissions PROPERTIES AUTOMOC ON)
# target_include_directories(test_tool_permissions PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/tools)
# target_link_libraries(test_tool_permissions
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_tool_permissions COMMAND test_tool_permissions)

# test_tool_workflow_integration - DISABLED: includes non-existent writetool.h
# add_executable(test_tool_workflow_integration test_tool_workflow_integration.cpp)
# set_target_properties(test_tool_workflow_integration PROPERTIES AUTOMOC ON)
# target_link_libraries(test_tool_workflow_integration
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_tool_workflow_integration COMMAND test_tool_workflow_integration)

# test_toolregistry_detailed
# add_executable(test_toolregistry_detailed test_toolregistry_detailed.cpp)
# set_target_properties(test_toolregistry_detailed PROPERTIES AUTOMOC ON)
# target_link_libraries(test_toolregistry_detailed
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_toolregistry_detailed COMMAND test_toolregistry_detailed)

# test_toolregistry_utils
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

# test_tools
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

# test_ui_e2e_complex
# add_executable(test_ui_e2e_complex test_ui_e2e_complex.cpp)
# set_target_properties(test_ui_e2e_complex PROPERTIES AUTOMOC ON)
# target_link_libraries(test_ui_e2e_complex
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_ui_e2e_complex COMMAND test_ui_e2e_complex)

# test_ui_events - DISABLED: uses non-existent ThreadView::currentIndex() method
# add_executable(test_ui_events test_ui_events.cpp)
# set_target_properties(test_ui_events PROPERTIES AUTOMOC ON)
# target_link_libraries(test_ui_events
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_ui_events COMMAND test_ui_events)

# test_ui_functionality - DISABLED: accesses private m_threads, uses non-existent setCurrentThread/getCurrentThreadMessages
# add_executable(test_ui_functionality test_ui_functionality.cpp)
# set_target_properties(test_ui_functionality PROPERTIES AUTOMOC ON)
# target_link_libraries(test_ui_functionality
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_ui_functionality COMMAND test_ui_functionality)

# test_ui_functionality_extended - DISABLED: accesses private members
# add_executable(test_ui_functionality_extended test_ui_functionality_extended.cpp)
# set_target_properties(test_ui_functionality_extended PROPERTIES AUTOMOC ON)
# target_link_libraries(test_ui_functionality_extended
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_ui_functionality_extended COMMAND test_ui_functionality_extended)

# test_ui_integration - DISABLED: accesses private m_currentThreadId, uses non-existent count() method
# add_executable(test_ui_integration test_ui_integration.cpp)
# set_target_properties(test_ui_integration PROPERTIES AUTOMOC ON)
# target_link_libraries(test_ui_integration
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_ui_integration COMMAND test_ui_integration)

# test_ui_integration_workflow
# add_executable(test_ui_integration_workflow test_ui_integration_workflow.cpp)
# set_target_properties(test_ui_integration_workflow PROPERTIES AUTOMOC ON)
# target_link_libraries(test_ui_integration_workflow
# Qt6::Test
# Qt6::Core
# Qt6::Widgets
# Qt6::Concurrent
# Qt6::Network
# KF6::I18n
# )
# add_test(NAME test_ui_integration_workflow COMMAND test_ui_integration_workflow)

# test_ui_interactions - DISABLED: calls non-existent ThreadView::parseMarkdownStatic()
# add_executable(test_ui_interactions test_ui_interactions.cpp)
# set_target_properties(test_ui_interactions PROPERTIES AUTOMOC ON)
# target_link_libraries(test_ui_interactions
#     Qt6::Test
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Concurrent
#     Qt6::Network
#     KF6::I18n
# )
# add_test(NAME test_ui_interactions COMMAND test_ui_interactions)

# test_urlfetch_diagnostic_tools
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
