#include <QtTest/QtTest>
#include "../src/llmprovider.h"

class TestLLMData : public QObject
{
    Q_OBJECT

private slots:

    void testLLMMessageDefault()
    {
        LLMMessage msg;
        QVERIFY(msg.role.isEmpty());
        QVERIFY(msg.content.isEmpty());
        QVERIFY(msg.toolCallId.isEmpty());
    }

    void testLLMMessageAssignment()
    {
        LLMMessage msg;
        msg.role = "user";
        msg.content = "Hello";
        msg.toolCallId = "call_123";
        
        LLMMessage msg2 = msg;
        QVERIFY(msg2.role == "user");
        QVERIFY(msg2.content == "Hello");
        QVERIFY(msg2.toolCallId == "call_123");
    }

    void testLLMMessageThinking()
    {
        LLMMessage msg;
        msg.thinking = "Reasoning about the answer...";
        QVERIFY(msg.thinking == "Reasoning about the answer...");
    }

    void testToolCall()
    {
        ToolCall tc;
        tc.id = "call_1";
        tc.name = "read_file";
        tc.arguments["path"] = "/test.cpp";
        
        QVERIFY(tc.id == "call_1");
        QVERIFY(tc.name == "read_file");
        QVERIFY(tc.arguments["path"].toString() == "/test.cpp");
    }

    void testToolDefinition()
    {
        ToolDefinition td;
        td.type = "function";
        td.function.name = "grep";
        td.function.description = "Search files";
        td.function.parameters = QJsonObject{{"type", "object"}};
        
        QVERIFY(td.type == "function");
        QVERIFY(td.function.name == "grep");
    }

    void testLLMResponse()
    {
        LLMResponse resp;
        resp.content = "Test response";
        resp.thinking = "Reasoning";
        resp.finishReason = "stop";
        
        QVERIFY(resp.content == "Test response");
        QVERIFY(resp.finishReason == "stop");
        QVERIFY(resp.promptTokens == 0);
        QVERIFY(resp.completionTokens == 0);
    }

    void testLLMResponseWithToolCalls()
    {
        LLMResponse resp;
        ToolCall tc;
        tc.name = "test";
        resp.toolCalls.push_back(tc);
        
        QVERIFY(resp.toolCalls.size() == 1);
    }

    void testConversationThread()
    {
        ConversationThread thread;
        thread.id = "thread_1";
        thread.title = "Test Thread";
        thread.currentModel = "gpt-4";
        thread.isActive = true;
        
        QVERIFY(thread.id == "thread_1");
        QVERIFY(thread.title == "Test Thread");
        QVERIFY(thread.isActive == true);
    }

    void testConversationThreadMessages()
    {
        ConversationThread thread;
        LLMMessage msg;
        msg.role = "user";
        msg.content = "Hello";
        thread.messages.append(msg);
        
        QVERIFY(thread.messages.size() == 1);
        QVERIFY(thread.messages[0].role == "user");
    }

    void testConversationThreadTimestamps()
    {
        ConversationThread thread;
        QDateTime now = QDateTime::currentDateTime();
        thread.createdAt = now;
        thread.updatedAt = now;
        
        QVERIFY(thread.createdAt == now);
        QVERIFY(thread.updatedAt == now);
    }

    void testQ_DECLARE_TYPEINFO()
    {
        static_assert(QTypeInfo<LLMMessage>::isRelocatable, "LLMMessage should be relocatable");
    }
};

QTEST_MAIN(TestLLMData)
#include "test_llm_data.moc"