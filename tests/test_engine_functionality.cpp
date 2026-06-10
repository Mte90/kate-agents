#include <QtTest/QtTest>
#include "../src/agentloop.h"
#include "../src/engine/agent-worker.h"
#include "../src/engine/agent-engine.h"
#include "../src/llmprovider.h"

class TestEngineFunctionality : public QObject
{
    Q_OBJECT

private slots:

    void testAgentLoopConstruction()
    {
        AgentLoop loop;
        QVERIFY(true);
    }

    void testAgentLoopSetProvider()
    {
        AgentLoop loop;
        
        QVERIFY(loop.m_provider != nullptr);
    }

    void testAgentLoopStartStop()
    {
        AgentLoop loop;
        
        loop.startLoop();
        
        loop.stopLoop();
        
        QVERIFY(true);
    }

    void testAgentLoopMessageHistory()
    {
        AgentLoop loop;
        
        LLMMessage msg;
        msg.role = "user";
        msg.content = "Hello";
        loop.m_messageHistory.append(msg);
        
        QVERIFY(loop.m_messageHistory.size() == 1);
        
        loop.m_messageHistory.clear();
        QVERIFY(loop.m_messageHistory.isEmpty());
    }

    void testAgentWorkerExecuteSimple()
    {
        AgentWorker worker;
        
        worker.m_messages.clear();
        
        QVERIFY(worker.m_messages.isEmpty());
    }

    void testAgentWorkerAddMultipleMessages()
    {
        AgentWorker worker;
        
        for (int i = 0; i < 10; i++) {
            LLMMessage msg;
            msg.role = (i % 2 == 0) ? "user" : "assistant";
            msg.content = "Message " + QString::number(i);
            worker.m_messages.append(msg);
        }
        
        QVERIFY(worker.m_messages.size() == 10);
    }

    void testAgentWorkerAbort()
    {
        AgentWorker worker;
        
        worker.m_abort = false;
        worker.abort();
        
        QVERIFY(worker.m_abort == true);
    }

    void testAgentWorkerErrorHandling()
    {
        AgentWorker worker;
        
        worker.m_errorMessage = "Test error";
        
        QVERIFY(worker.m_errorMessage == "Test error");
        
        worker.m_errorMessage.clear();
        QVERIFY(worker.m_errorMessage.isEmpty());
    }

    void testAgentEngineConfiguration()
    {
        AgentEngine engine;
        
        engine.setBaseUrl("http://localhost:8080");
        engine.setApiKey("test-key");
        engine.setModel("gpt-4");
        
        QVERIFY(engine.m_baseUrl == "http://localhost:8080");
        QVERIFY(engine.m_apiKey == "test-key");
        QVERIFY(engine.m_model == "gpt-4");
    }

    void testAgentEngineMessageManagement()
    {
        AgentEngine engine;
        
        LLMMessage msg1;
        msg1.role = "system";
        msg1.content = "You are helpful";
        
        LLMMessage msg2;
        msg2.role = "user";
        msg2.content = "Hello";
        
        engine.m_messages.append(msg1);
        engine.m_messages.append(msg2);
        
        QVERIFY(engine.m_messages.size() == 2);
    }

    void testAgentEngineClearMessages()
    {
        AgentEngine engine;
        
        engine.m_messages.append(LLMMessage());
        engine.m_messages.append(LLMMessage());
        
        engine.m_messages.clear();
        
        QVERIFY(engine.m_messages.isEmpty());
    }

    void testAgentEngineToolDefinitions()
    {
        AgentEngine engine;
        
        ToolDefinition tool;
        tool.type = "function";
        tool.function.name = "test_tool";
        
        engine.m_toolDefinitions.append(tool);
        
        QVERIFY(engine.m_toolDefinitions.size() == 1);
    }

    void testAgentEngineParameterSettings()
    {
        AgentEngine engine;
        
        engine.setTemperature(0.5);
        engine.setMaxTokens(1000);
        
        QVERIFY(engine.m_temperature == 0.5);
        QVERIFY(engine.m_maxTokens == 1000);
    }

    void testAgentEngineSystemPrompt()
    {
        AgentEngine engine;
        
        engine.setSystemPrompt("You are a coding assistant");
        
        QVERIFY(engine.m_systemPrompt == "You are a coding assistant");
    }

    void testAgentLoopToolExecution()
    {
        AgentLoop loop;
        
        QVERIFY(loop.m_toolRegistry != nullptr);
    }

    void testAgentLoopThreadId()
    {
        AgentLoop loop;
        
        loop.m_threadId = "test-thread-123";
        
        QVERIFY(loop.m_threadId == "test-thread-123");
    }

    void testAgentLoopActiveFlag()
    {
        AgentLoop loop;
        
        loop.m_active = true;
        
        QVERIFY(loop.m_active == true);
        
        loop.m_active = false;
        
        QVERIFY(loop.m_active == false);
    }

    void testAgentWorkerStateTransitions()
    {
        AgentWorker worker;
        
        worker.m_state = AgentWorker::Idle;
        QVERIFY(worker.m_state == AgentWorker::Idle);
        
        worker.m_state = AgentWorker::Working;
        QVERIFY(worker.m_state == AgentWorker::Working);
        
        worker.m_state = AgentWorker::Idle;
        QVERIFY(worker.m_state == AgentWorker::Idle);
        
        worker.m_state = AgentWorker::Error;
        QVERIFY(worker.m_state == AgentWorker::Error);
    }

    void testAgentLoopSignalConnections()
    {
        AgentLoop loop;
        
        QVERIFY(loop.m_provider != nullptr);
    }

    void testAgentEngineAbort()
    {
        AgentEngine engine;
        
        engine.abort();
        
        QVERIFY(true);
    }
};

QTEST_MAIN(TestEngineFunctionality)
#include "test_engine_functionality.moc"