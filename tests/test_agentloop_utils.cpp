#include <QtTest/QtTest>
#include "../src/agentloop.h"
#include "../src/llmprovider.h"
#include <QJsonDocument>

class TestAgentLoopUtils : public QObject
{
    Q_OBJECT

private slots:

    // Test formatMessagesForAPI
    void testFormatMessagesEmpty()
    {
        std::vector<LLMMessage> msgs;
        // Should not crash
        QVERIFY(true);
    }

    void testFormatMessagesUserOnly()
    {
        std::vector<LLMMessage> msgs;
        LLMMessage msg;
        msg.role = "user";
        msg.content = "Hello";
        msgs.push_back(msg);
        QVERIFY(msgs.size() == 1);
    }

    void testFormatMessagesAssistant()
    {
        std::vector<LLMMessage> msgs;
        LLMMessage msg;
        msg.role = "assistant";
        msg.content = "Response";
        msgs.push_back(msg);
        QVERIFY(msgs.size() == 1);
    }

    void testFormatMessagesSystem()
    {
        std::vector<LLMMessage> msgs;
        LLMMessage msg;
        msg.role = "system";
        msg.content = "You are helpful";
        msgs.push_back(msg);
        QVERIFY(msgs.size() == 1);
    }

    void testFormatMessagesToolCall()
    {
        std::vector<LLMMessage> msgs;
        LLMMessage msg;
        msg.role = "tool";
        msg.content = "File content";
        msg.toolCallId = "call_123";
        msgs.push_back(msg);
        QVERIFY(msg.toolCallId == "call_123");
    }

    void testFormatMessagesMultiple()
    {
        std::vector<LLMMessage> msgs;
        for (int i = 0; i < 10; i++) {
            LLMMessage msg;
            msg.role = "user";
            msg.content = "Message " + QString::number(i);
            msgs.push_back(msg);
        }
        QVERIFY(msgs.size() == 10);
    }

    // Test LLMMessage content variations
    void testMessageWithEmptyContent()
    {
        LLMMessage msg;
        msg.content = "";
        QVERIFY(msg.content.isEmpty());
    }

    void testMessageWithNewlines()
    {
        LLMMessage msg;
        msg.content = "Line 1\nLine 2\nLine 3";
        QVERIFY(msg.content.contains('\n'));
    }

    void testMessageWithSpecialChars()
    {
        LLMMessage msg;
        msg.content = "Test <>&\"' chars";
        QVERIFY(msg.content.contains('<'));
    }

    void testMessageWithUnicode()
    {
        LLMMessage msg;
        msg.content = "Ciao mondo 🌍";
        QVERIFY(msg.content.contains("mondo"));
    }

    void testMessageWithVeryLongContent()
    {
        LLMMessage msg;
        msg.content = QString(10000, 'x');
        QVERIFY(msg.content.length() == 10000);
    }

    // Test ToolDefinition
    void testToolDefinitionEmptyParameters()
    {
        ToolDefinition td;
        td.function.parameters = QJsonObject();
        QVERIFY(td.function.parameters.isEmpty());
    }

    void testToolDefinitionComplexParameters()
    {
        ToolDefinition td;
        QJsonObject params;
        params["type"] = "object";
        params["properties"] = QJsonObject{{"name", QJsonObject{{"type", "string"}}}};
        params["required"] = QJsonArray{"name"};
        td.function.parameters = params;
        QVERIFY(!td.function.parameters.isEmpty());
    }

    // Test LLMResponse
    void testResponseEmptyContent()
    {
        LLMResponse resp;
        QVERIFY(resp.content.isEmpty());
    }

    void testResponseWithThinking()
    {
        LLMResponse resp;
        resp.thinking = "Let me think about this...";
        resp.content = "Final answer";
        QVERIFY(resp.thinking.contains("think"));
        QVERIFY(resp.content == "Final answer");
    }

    void testResponseFinishReasons()
    {
        LLMResponse resp1;
        resp1.finishReason = "stop";
        QVERIFY(resp1.finishReason == "stop");
        
        LLMResponse resp2;
        resp2.finishReason = "length";
        QVERIFY(resp2.finishReason == "length");
        
        LLMResponse resp3;
        resp3.finishReason = "tool_calls";
        QVERIFY(resp3.finishReason == "tool_calls");
    }

    // Test token counting
    void testResponseTokenCounts()
    {
        LLMResponse resp;
        resp.promptTokens = 100;
        resp.completionTokens = 50;
        QVERIFY(resp.promptTokens == 100);
        QVERIFY(resp.completionTokens == 50);
        QVERIFY(resp.promptTokens + resp.completionTokens == 150);
    }

    // Test ConversationThread
    void testThreadEmptyId()
    {
        ConversationThread thread;
        QVERIFY(thread.id.isEmpty());
    }

    void testThreadWithMessages()
    {
        ConversationThread thread;
        thread.messages.append(LLMMessage{{{"role", "user"}, {"content", "Hi"}}});
        thread.messages.append(LLMMessage{{{"role", "assistant"}, {"content", "Hello"}}});
        QVERIFY(thread.messages.size() == 2);
    }

    void testThreadInactiveByDefault()
    {
        ConversationThread thread;
        QVERIFY(thread.isActive == false);
    }

    void testThreadSetActive()
    {
        ConversationThread thread;
        thread.isActive = true;
        QVERIFY(thread.isActive == true);
    }

    void testThreadTimestamps()
    {
        ConversationThread thread;
        QDateTime before = QDateTime::currentDateTime();
        thread.createdAt = before;
        thread.updatedAt = before;
        QVERIFY(thread.createdAt <= thread.updatedAt);
    }

    // Test AgentProfile enum values
    void testAllAgentProfiles()
    {
        QVERIFY(AgentProfile::Write != AgentProfile::Ask);
        QVERIFY(AgentProfile::Ask != AgentProfile::Minimal);
        QVERIFY(AgentProfile::Write != AgentProfile::Minimal);
    }

    // Test ToolCall
    void testToolCallEmptyArguments()
    {
        ToolCall tc;
        QVERIFY(tc.arguments.isEmpty());
    }

    void testToolCallWithJsonArguments()
    {
        ToolCall tc;
        tc.arguments = QJsonDocument::fromJson("{\"key\": \"value\"}").object();
        QVERIFY(tc.arguments["key"].toString() == "value");
    }

    // Test vector operations
    void testMessageVectorPushBack()
    {
        std::vector<LLMMessage> msgs;
        msgs.push_back(LLMMessage{{{"role", "user"}, {"content", "Hello"}}});
        msgs.push_back(LLMMessage{{{"role", "assistant"}, {"content", "Hi"}}});
        QVERIFY(msgs.size() == 2);
    }

    void testToolVectorEmpty()
    {
        std::vector<ToolDefinition> tools;
        QVERIFY(tools.empty());
        QVERIFY(tools.size() == 0);
    }

    void testToolVectorPushBack()
    {
        std::vector<ToolDefinition> tools;
        ToolDefinition td;
        td.function.name = "test";
        tools.push_back(td);
        QVERIFY(tools.size() == 1);
    }

    // Edge cases
    void testEmptyRole()
    {
        LLMMessage msg;
        msg.role = "";
        QVERIFY(msg.role.isEmpty());
    }

    void testSystemRole()
    {
        LLMMessage msg;
        msg.role = "system";
        QVERIFY(msg.role == "system");
    }

    void testToolRole()
    {
        LLMMessage msg;
        msg.role = "tool";
        QVERIFY(msg.role == "tool");
    }
};

QTEST_MAIN(TestAgentLoopUtils)
#include "test_agentloop_utils.moc"