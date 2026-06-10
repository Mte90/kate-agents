#include <QtTest/QtTest>
#include "../src/llmprovider.h"

class TestLLMDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testLLMMessageDefault()
    {
        LLMMessage msg;
        QVERIFY(msg.role.isEmpty());
        QVERIFY(msg.content.isEmpty());
    }

    void testLLMMessageFull()
    {
        LLMMessage msg;
        msg.role = "user";
        msg.content = "Hello world";
        msg.toolCallId = "call_123";
        QVERIFY(msg.role == "user");
        QVERIFY(msg.content == "Hello world");
        QVERIFY(msg.toolCallId == "call_123");
    }

    void testLLMMessageToJson()
    {
        LLMMessage msg;
        msg.role = "user";
        msg.content = "test";
        QJsonObject json = msg.toJson();
        QVERIFY(json["role"] == "user");
        QVERIFY(json["content"] == "test");
    }

    void testLLMMessageFromJson()
    {
        QJsonObject json{{"role", "assistant"}, {"content", "response"}};
        LLMMessage msg = LLMMessage::fromJson(json);
        QVERIFY(msg.role == "assistant");
        QVERIFY(msg.content == "response");
    }

    void testToolDefinition()
    {
        ToolDefinition td;
        td.type = "function";
        td.function.name = "test_func";
        td.function.description = "Test function";
        QVERIFY(td.type == "function");
        QVERIFY(td.function.name == "test_func");
    }

    void testToolCall()
    {
        ToolCall tc;
        tc.id = "call_1";
        tc.name = "my_tool";
        tc.arguments = QJsonObject{{"arg", "value"}};
        QVERIFY(tc.id == "call_1");
        QVERIFY(tc.name == "my_tool");
    }

    void testLLMResponse()
    {
        LLMResponse resp;
        resp.content = "AI response";
        resp.finishReason = "stop";
        QVERIFY(resp.content == "AI response");
        QVERIFY(resp.finishReason == "stop");
    }

    void testConversationThread()
    {
        ConversationThread thread;
        thread.id = "thread-1";
        thread.title = "Test";
        thread.messages.append(LLMMessage{{{"role", "user"}, {"content", "Hi"}}});
        QVERIFY(thread.id == "thread-1");
        QVERIFY(thread.messages.size() == 1);
    }

    void testConversationThreadTitle()
    {
        ConversationThread thread;
        thread.title = "My Thread";
        QVERIFY(thread.title == "My Thread");
    }

    void testConversationThreadMessages()
    {
        ConversationThread thread;
        thread.messages.append(LLMMessage{{{"role", "user"}, {"content", "msg1"}}});
        thread.messages.append(LLMMessage{{{"role", "assistant"}, {"content", "msg2"}}});
        QVERIFY(thread.messages.size() == 2);
    }

    void testAgentProfile()
    {
        QVERIFY(AgentProfile::Write != AgentProfile::Ask);
        QVERIFY(AgentProfile::Ask != AgentProfile::Minimal);
        QVERIFY(AgentProfile::Minimal != AgentProfile::Write);
    }

    void testToolDefinitionParameters()
    {
        ToolDefinition td;
        td.function.parameters = "{}";
        QVERIFY(!td.function.parameters.isEmpty());
    }

    void testToolDefinitionToJson()
    {
        ToolDefinition td;
        td.type = "function";
        td.function.name = "test";
        QJsonObject json = td.toJson();
        QVERIFY(json["type"] == "function");
    }

    void testToolCallToJson()
    {
        ToolCall tc;
        tc.id = "call_1";
        tc.name = "tool1";
        QJsonObject json = tc.toJson();
        QVERIFY(json["id"] == "call_1");
        QVERIFY(json["name"] == "tool1");
    }

    void testLLMResponseTokens()
    {
        LLMResponse resp;
        resp.promptTokens = 100;
        resp.completionTokens = 50;
        QVERIFY(resp.promptTokens == 100);
        QVERIFY(resp.completionTokens == 50);
    }
};

QTEST_MAIN(TestLLMDetailed)
#include "test_llm_detailed.moc"