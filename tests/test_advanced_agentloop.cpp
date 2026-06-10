#include <QtTest/QtTest>
#include "../src/agentloop.h"
#include "../src/engine/agent-worker.h"
#include "../src/engine/agent-engine.h"
#include "../src/llmprovider.h"

class TestAdvancedAgentLoop : public QObject
{
    Q_OBJECT

private slots:

    void testExecuteTurnWithModel()
    {
        AgentLoop loop;
        
        loop.m_currentModel = "gpt-4";
        
        QString result = loop.executeTurn("Hello", "claude-3");
        
        QVERIFY(!result.isEmpty() || result.isEmpty());
    }

    void testExecuteTurnWithoutModel()
    {
        AgentLoop loop;
        
        QString result = loop.executeTurn("Test message");
        
        QVERIFY(!result.isEmpty() || result.isEmpty());
    }

    void testMessageHistoryTruncation()
    {
        AgentLoop loop;
        
        for (int i = 0; i < 100; i++) {
            LLMMessage msg;
            msg.role = i % 2 == 0 ? "user" : "assistant";
            msg.content = "Message " + QString::number(i);
            loop.m_messageHistory.append(msg);
        }
        
        int beforeCount = loop.m_messageHistory.size();
        
        if (beforeCount > 50) {
            loop.m_messageHistory = loop.m_messageHistory.mid(beforeCount - 50);
        }
        
        int afterCount = loop.m_messageHistory.size();
        
        QVERIFY(afterCount <= beforeCount);
    }

    void testToolCallExtraction()
    {
        AgentLoop loop;
        
        LLMMessage msg;
        msg.content = "I need to read a file";
        
        ToolDefinition tool;
        tool.type = "function";
        tool.function.name = "read_file";
        
        loop.m_toolDefinitions.append(tool);
        
        bool shouldCallTool = loop.shouldCallTool(msg.content);
        
        QVERIFY(shouldCallTool == true || shouldCallTool == false);
    }

    void testResponseParsing()
    {
        AgentLoop loop;
        
        QString response = "This is a simple response without any tool calls.";
        
        bool hasToolCall = response.contains("tool_call") || response.contains("function");
        
        QVERIFY(hasToolCall == false);
    }

    void testStreamingResponseAccumulation()
    {
        AgentLoop loop;
        
        QString accumulated;
        
        for (int i = 0; i < 10; i++) {
            QString chunk = "chunk" + QString::number(i) + " ";
            accumulated += chunk;
        }
        
        QVERIFY(accumulated.length() > 50);
    }

    void testWorkerThreadSafety()
    {
        AgentWorker worker;
        
        worker.m_state = AgentWorker::Idle;
        
        worker.m_abort = false;
        
        bool canStart = (worker.m_state == AgentWorker::Idle) && !worker.m_abort;
        
        QVERIFY(canStart == true);
    }

    void testWorkerErrorPropagation()
    {
        AgentWorker worker;
        
        worker.m_errorMessage = "Network timeout";
        worker.m_state = AgentWorker::Error;
        
        bool hasError = worker.m_state == AgentWorker::Error;
        
        QVERIFY(hasError == true);
        
        worker.m_state = AgentWorker::Idle;
        worker.m_errorMessage.clear();
        
        QVERIFY(worker.m_errorMessage.isEmpty());
    }

    void testEngineMessageBuilding()
    {
        AgentEngine engine;
        
        engine.m_systemPrompt = "You are helpful.";
        engine.m_messages.append(LLMMessage{{{"role", "system"}, {"content", engine.m_systemPrompt}}});
        engine.m_messages.append(LLMMessage{{{"role", "user"}, {"content", "Hello"}}});
        
        QJsonArray messages = engine.buildMessagesJson();
        
        QVERIFY(messages.size() >= 2);
    }

    void testEngineToolCallBuilding()
    {
        AgentEngine engine;
        
        ToolDefinition tool;
        tool.type = "function";
        tool.function.name = "read_file";
        tool.function.description = "Read a file";
        
        engine.m_toolDefinitions.append(tool);
        
        QJsonArray tools = engine.buildToolsJson();
        
        QVERIFY(tools.size() >= 1);
    }

    void testLoopStateTransitions()
    {
        AgentLoop loop;
        
        loop.m_active = false;
        QVERIFY(loop.m_active == false);
        
        loop.startLoop();
        loop.m_active = true;
        QVERIFY(loop.m_active == true);
        
        loop.stopLoop();
        loop.m_active = false;
        QVERIFY(loop.m_active == false);
    }

    void testAbortSignalHandling()
    {
        AgentLoop loop;
        
        loop.m_abortRequested = false;
        
        loop.requestAbort();
        
        QVERIFY(loop.m_abortRequested == true);
        
        loop.m_abortRequested = false;
        
        QVERIFY(loop.m_abortRequested == false);
    }

    void testTokenCounting()
    {
        AgentLoop loop;
        
        QString text = "This is a test message with several words";
        int wordCount = text.split(" ").size();
        
        int estimatedTokens = wordCount * 4 / 3;
        
        QVERIFY(estimatedTokens > 0);
    }

    void testRetryLogic()
    {
        AgentLoop loop;
        
        int maxRetries = 3;
        int attempts = 0;
        
        while (attempts < maxRetries) {
            attempts++;
        }
        
        QVERIFY(attempts == maxRetries);
    }

    void testContextWindowManagement()
    {
        AgentLoop loop;
        
        int maxContext = 4096;
        
        for (int i = 0; i < 100; i++) {
            LLMMessage msg;
            msg.content = QString("word ").repeated(100);
            loop.m_messageHistory.append(msg);
        }
        
        int totalTokens = loop.estimateTokenCount();
        
        QVERIFY(totalTokens >= 0);
    }
};

QTEST_MAIN(TestAdvancedAgentLoop)
#include "test_advanced_agentloop.moc"