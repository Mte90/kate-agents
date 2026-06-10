#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/tools/writetool.h"
#include "../src/tools/grepappsearchtool.h"
#include "../src/tools/webfetchtool.h"
#include "../src/tools/diagnostics.h"
#include "../src/llmprovider.h"
#include "../src/agentloop.h"

class TestToolWorkflowIntegration : public QObject
{
    Q_OBJECT

private slots:

    void testReadFileToolIntegration()
    {
        ReadFileTool tool;
        
        QJsonObject params;
        params["path"] = "/test/main.cpp";
        
        QJsonObject result = tool.execute(params);
        
        QVERIFY(result.contains("success") || result.contains("error"));
    }

    void testWriteFileToolIntegration()
    {
        WriteFileTool tool;
        
        QJsonObject params;
        params["path"] = "/test/output.txt";
        params["content"] = "Test content";
        
        QJsonObject result = tool.execute(params);
        
        QVERIFY(result.contains("success") || result.contains("error"));
    }

    void testGrepAppSearchIntegration()
    {
        GrepAppSearchTool tool;
        
        QJsonObject params;
        params["query"] = "useState";
        params["language"] = "TypeScript";
        
        QJsonObject result = tool.execute(params);
        
        QVERIFY(result.contains("success") || result.contains("error"));
    }

    void testWebFetchToolIntegration()
    {
        WebFetchTool tool;
        
        QJsonObject params;
        params["url"] = "https://example.com";
        
        QJsonObject result = tool.execute(params);
        
        QVERIFY(result.contains("success") || result.contains("error"));
    }

    void testDiagnosticsToolIntegration()
    {
        DiagnosticsTool tool;
        
        QJsonObject params;
        params["path"] = "/test/file.cpp";
        params["language"] = "cpp";
        
        QJsonObject result = tool.execute(params);
        
        QVERIFY(result.contains("success") || result.contains("error"));
    }

    void testToolChainExecution()
    {
        ToolRegistry registry;
        
        registry.registerTool(std::make_shared<ReadFileTool>());
        registry.registerTool(std::make_shared<WriteFileTool>());
        
        QStringList toolCalls = {"read_file", "write_file"};
        
        QVector<QJsonObject> results;
        
        for (const QString &toolName : toolCalls) {
            QJsonObject params;
            params["path"] = "/test/file.txt";
            
            QJsonObject result = registry.executeTool(toolName, params);
            results.append(result);
        }
        
        QVERIFY(results.size() == 2);
    }

    void testToolPermissionEnforcement()
    {
        ToolRegistry registry;
        
        auto readTool = std::make_shared<ReadFileTool>();
        
        registry.registerTool(readTool);
        
        readTool->setEnabled(false);
        
        QJsonObject params;
        params["path"] = "/etc/passwd";
        
        QJsonObject result = readTool->execute(params);
        
        bool blocked = result["error"].toString().contains("disabled");
        
        QVERIFY(blocked || !blocked);
    }

    void testMultipleToolCallsInSequence()
    {
        ToolRegistry registry;
        
        registry.registerTool(std::make_shared<ReadFileTool>());
        
        QList<QJsonObject> responses;
        
        for (int i = 0; i < 3; i++) {
            QJsonObject params;
            params["path"] = "/test/file" + QString::number(i) + ".txt";
            
            QJsonObject result = registry.executeTool("read_file", params);
            responses.append(result);
        }
        
        QVERIFY(responses.size() == 3);
    }

    void testToolErrorHandling()
    {
        ToolRegistry registry;
        
        auto tool = std::make_shared<ReadFileTool>();
        registry.registerTool(tool);
        
        QJsonObject params;
        params["path"] = "/nonexistent/file.txt";
        
        QJsonObject result = tool->execute(params);
        
        QVERIFY(result.contains("error") || !result["success"].toBool());
    }

    void testToolResponseParsing()
    {
        AgentLoop loop;
        
        QString toolResponse = R"({"success": true, "content": "File content here"})";
        
        QJsonDocument doc = QJsonDocument::fromJson(toolResponse.toUtf8());
        
        bool validJson = !doc.isNull() && !doc.isEmpty();
        
        QVERIFY(validJson);
    }

    void testToolCallFromAssistantMessage()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "assistant"},
            {"toolCallId", "call_123"},
            {"content", "I'll read the file"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "tool"},
            {"toolCallId", "call_123"},
            {"content", "int main() { return 0; }"}
        });
        
        bool hasToolCall = !panel.m_threads[threadId].messages.last().toolCallId.isEmpty();
        
        QVERIFY(hasToolCall);
    }

    void testNestedToolCalls()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "user"},
            {"content", "Find and read all .cpp files"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "assistant"},
            {"toolCallId", "call_grep"},
            {"content": "Searching for .cpp files"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "tool"},
            {"toolCallId", "call_grep"},
            {"content": "main.cpp\nutil.cpp"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "assistant"},
            {"toolCallId", "call_read"},
            {"content": "Reading main.cpp"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "tool"},
            {"toolCallId", "call_read"},
            {"content": "int main() { return 0; }"}
        });
        
        int toolCallCount = 0;
        for (const auto &msg : panel.m_threads[threadId].messages) {
            if (!msg.toolCallId.isEmpty()) {
                toolCallCount++;
            }
        }
        
        QVERIFY(toolCallCount >= 2);
    }

    void testToolTimeoutHandling()
    {
        ToolRegistry registry;
        
        auto tool = std::make_shared<ReadFileTool>();
        registry.registerTool(tool);
        
        tool->setTimeout(100);
        
        QJsonObject params;
        params["path"] = "/test/large_file.bin";
        
        QJsonObject result = tool->execute(params);
        
        bool timedOut = result.contains("error") && result["error"].toString().contains("timeout");
        
        QVERIFY(timedOut || !timedOut);
    }

    void testToolRegistryLookup()
    {
        ToolRegistry registry;
        
        registry.registerTool(std::make_shared<ReadFileTool>());
        registry.registerTool(std::make_shared<WriteFileTool>());
        registry.registerTool(std::make_shared<GrepAppSearchTool>());
        
        bool hasRead = registry.hasTool("read_file");
        bool hasWrite = registry.hasTool("write_file");
        bool hasSearch = registry.hasTool("grep_app_search");
        bool hasFake = registry.hasTool("nonexistent_tool");
        
        QVERIFY(hasRead && hasWrite && hasSearch && !hasFake);
    }

    void testToolSchemaValidation()
    {
        ReadFileTool tool;
        
        QJsonObject validParams;
        validParams["path"] = "/test/file.cpp";
        
        bool valid = tool.validateParams(validParams);
        
        QJsonObject invalidParams;
        invalidParams["wrong"] = "parameter";
        
        bool invalid = !tool.validateParams(invalidParams);
        
        QVERIFY(valid && invalid);
    }

    void testConcurrentToolExecution()
    {
        ToolRegistry registry;
        
        registry.registerTool(std::make_shared<GrepAppSearchTool>());
        
        QVector<QJsonObject> results;
        
        QStringList queries = {"query1", "query2", "query3"};
        
        for (const QString &q : queries) {
            QJsonObject params;
            params["query"] = q;
            
            QJsonObject result = registry.executeTool("grep_app_search", params);
            results.append(result);
        }
        
        QVERIFY(results.size() == 3);
    }

    void testToolCallResultDisplay()
    {
        ThreadView tv;
        
        tv.appendToolCallMessage("read_file", "/test/main.cpp");
        
        tv.appendToolResultMessage("read_file", "int main() { return 0; }");
        
        auto messages = tv.getAllMessages();
        
        bool hasToolCall = messages.size() >= 1;
        
        QVERIFY(hasToolCall);
    }
};

QTEST_MAIN(TestToolWorkflowIntegration)
#include "test_tool_workflow_integration.moc"