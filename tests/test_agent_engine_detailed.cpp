#include <QtTest/QtTest>
#include "../src/engine/agent-engine.h"
#include "../src/llmprovider.h"

class TestAgentEngineDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        AgentEngine engine;
        QVERIFY(true);
    }

    void testProviderName()
    {
        AgentEngine engine;
        QVERIFY(!engine.name().isEmpty());
    }

    void testSetBaseUrl()
    {
        AgentEngine engine;
        engine.setBaseUrl("http://localhost:8080");
        QVERIFY(engine.m_baseUrl == "http://localhost:8080");
    }

    void testSetApiKey()
    {
        AgentEngine engine;
        engine.setApiKey("test-key-123");
        QVERIFY(engine.m_apiKey == "test-key-123");
    }

    void testSetModel()
    {
        AgentEngine engine;
        engine.setModel("custom-model");
        QVERIFY(engine.m_model == "custom-model");
    }

    void testIsAvailableFalseByDefault()
    {
        AgentEngine engine;
        QVERIFY(engine.isAvailable() == false);
    }

    void testTemperature()
    {
        AgentEngine engine;
        engine.setTemperature(0.7);
        QVERIFY(engine.m_temperature == 0.7);
    }

    void testMaxTokens()
    {
        AgentEngine engine;
        engine.setMaxTokens(2000);
        QVERIFY(engine.m_maxTokens == 2000);
    }

    void testSystemPrompt()
    {
        AgentEngine engine;
        engine.setSystemPrompt("You are a helpful assistant");
        QVERIFY(engine.m_systemPrompt == "You are a helpful assistant");
    }

    void testMessagesList()
    {
        AgentEngine engine;
        QVERIFY(engine.m_messages.isEmpty());
    }

    void testAddMessage()
    {
        AgentEngine engine;
        LLMMessage msg;
        msg.role = "user";
        msg.content = "Hello";
        engine.m_messages.append(msg);
        QVERIFY(engine.m_messages.size() == 1);
    }

    void testClearMessages()
    {
        AgentEngine engine;
        LLMMessage msg;
        engine.m_messages.append(msg);
        engine.m_messages.clear();
        QVERIFY(engine.m_messages.isEmpty());
    }

    void testToolDefinitions()
    {
        AgentEngine engine;
        QVERIFY(engine.m_toolDefinitions.isEmpty());
    }

    void testAddToolDefinition()
    {
        AgentEngine engine;
        ToolDefinition td;
        td.type = "function";
        td.function.name = "test";
        engine.m_toolDefinitions.append(td);
        QVERIFY(engine.m_toolDefinitions.size() == 1);
    }

    void testAbort()
    {
        AgentEngine engine;
        engine.abort();
    }

    void testNetworkManager()
    {
        AgentEngine engine;
        QVERIFY(engine.m_nam != nullptr);
    }
};

QTEST_MAIN(TestAgentEngineDetailed)
#include "test_agent_engine_detailed.moc"