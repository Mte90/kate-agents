#include <QtTest/QtTest>
#include "../src/agentloop.h"
#include "../src/llmprovider.h"

class TestAgentLoopDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testProfileEnumValues()
    {
        QVERIFY(AgentProfile::Write != AgentProfile::Ask);
        QVERIFY(AgentProfile::Ask != AgentProfile::Minimal);
        QVERIFY(AgentProfile::Minimal != AgentProfile::Write);
    }

    void testSystemPromptWriteNotEmpty()
    {
        QString prompt = systemPromptForProfile(AgentProfile::Write);
        QVERIFY(!prompt.isEmpty());
        QVERIFY(prompt.length() > 50);
    }

    void testSystemPromptAskNotEmpty()
    {
        QString prompt = systemPromptForProfile(AgentProfile::Ask);
        QVERIFY(!prompt.isEmpty());
    }

    void testSystemPromptMinimalNotEmpty()
    {
        QString prompt = systemPromptForProfile(AgentProfile::Minimal);
        QVERIFY(!prompt.isEmpty());
    }

    void testSystemPromptWriteVsAsk()
    {
        QString write = systemPromptForProfile(AgentProfile::Write);
        QString ask = systemPromptForProfile(AgentProfile::Ask);
        QVERIFY(write != ask);
    }

    void testProfileToStringAll()
    {
        QVERIFY(profileToString(AgentProfile::Write) == "Write");
        QVERIFY(profileToString(AgentProfile::Ask) == "Ask");
        QVERIFY(profileToString(AgentProfile::Minimal) == "Minimal");
    }

    void testStringToProfileAll()
    {
        QVERIFY(stringToProfile("Write") == AgentProfile::Write);
        QVERIFY(stringToProfile("Ask") == AgentProfile::Ask);
        QVERIFY(stringToProfile("Minimal") == AgentProfile::Minimal);
    }

    void testMessageVectorOperations()
    {
        std::vector<LLMMessage> msgs;
        LLMMessage msg1;
        msg1.role = "user";
        msg1.content = "Hello";
        msgs.push_back(msg1);
        
        LLMMessage msg2;
        msg2.role = "assistant";
        msg2.content = "Hi there";
        msgs.push_back(msg2);
        
        QVERIFY(msgs.size() == 2);
        QVERIFY(msgs[0].role == "user");
        QVERIFY(msgs[1].role == "assistant");
    }

    void testToolDefinitionVector()
    {
        std::vector<ToolDefinition> tools;
        ToolDefinition td;
        td.type = "function";
        td.function.name = "test";
        tools.push_back(td);
        
        QVERIFY(tools.size() == 1);
        QVERIFY(tools[0].function.name == "test");
    }

    void testToolCallVectorOperations()
    {
        LLMResponse resp;
        ToolCall tc;
        tc.id = "call_1";
        tc.name = "tool1";
        resp.toolCalls.push_back(tc);
        
        QVERIFY(resp.toolCalls.size() == 1);
        QVERIFY(resp.toolCalls[0].id == "call_1");
    }

    void testLLMResponseTokens()
    {
        LLMResponse resp;
        resp.promptTokens = 100;
        resp.completionTokens = 200;
        resp.totalTokens();
        QVERIFY(resp.promptTokens == 100);
        QVERIFY(resp.completionTokens == 200);
    }

    void testConversationThreadMessages()
    {
        ConversationThread thread;
        thread.messages.append(LLMMessage{{{"role", "user"}, {"content", "Hi"}}});
        thread.messages.append(LLMMessage{{{"role", "assistant"}, {"content", "Hello"}}});
        thread.messages.append(LLMMessage{{{"role", "user"}, {"content", "How are you?"}}});
        
        QVERIFY(thread.messages.size() == 3);
        QVERIFY(thread.messages[0].role == "user");
        QVERIFY(thread.messages[1].role == "assistant");
    }
};

QTEST_MAIN(TestAgentLoopDetailed)
#include "test_agentloop_detailed.moc"