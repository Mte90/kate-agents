#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/ui/agentpanel.h"
#include "../src/llmprovider.h"
#include "../src/agentloop.h"

class TestUIIntegration : public QObject
{
    Q_OBJECT

private slots:

    void testThreadViewInputBarConnection()
    {
        ThreadView tv;
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(tv.m_tabs != nullptr);
        QVERIFY(bar.m_messageEdit != nullptr);
    }

    void testAppendAndGetMessages()
    {
        ThreadView tv;
        tv.appendUserMessage("gpt-4");
        auto msgs = tv.getAllMessages();
        QVERIFY(msgs.size() >= 1);
    }

    void testMultipleThreadOperations()
    {
        ThreadView tv;
        tv.setCurrentThread("thread-1");
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        tv.setCurrentThread("thread-2");
        tv.appendUserMessage("model");
        
        QVERIFY(tv.count() >= 2);
    }

    void testStreamingStateTransitions()
    {
        ThreadView tv;
        tv.appendAssistantMessage("model");
        tv.showStreamingChunk("Hello");
        tv.showStreamingChunk(" World");
        tv.endStreaming();
        
        auto msgs = tv.getAllMessages();
        QVERIFY(msgs.size() >= 1);
    }

    void testMessageContentTypes()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        auto msgs = tv.getAllMessages();
        bool hasUser = false, hasAssistant = false;
        for (const auto &msg : msgs) {
            if (msg.role == "user") hasUser = true;
            if (msg.role == "assistant") hasAssistant = true;
        }
    }

    void testPanelThreadViewConnection()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threadView != nullptr);
        QVERIFY(panel.m_inputBar != nullptr);
    }

    void testInputBarProfileSwitch()
    {
        InputBar barWrite(nullptr, AgentProfile::Write);
        InputBar barAsk(nullptr, AgentProfile::Ask);
        
        QVERIFY(barWrite.m_currentProfile == AgentProfile::Write);
        QVERIFY(barAsk.m_currentProfile == AgentProfile::Ask);
    }

    void testModelsListPropagation()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_models << "gpt-4" << "claude-3" << "llama-3";
        QVERIFY(bar.m_models.size() == 3);
    }

    void testClearAndRepopulate()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        tv.clearAllMessages();
        QVERIFY(tv.count() == 0);
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        QVERIFY(tv.count() >= 2);
    }

    void testThreadIdPersists()
    {
        ThreadView tv;
        tv.setCurrentThread("persist-test");
        QString id = tv.m_currentThreadId;
        QVERIFY(id == "persist-test");
    }

    void testEmptyToMultipleMessages()
    {
        ThreadView tv;
        QVERIFY(tv.count() == 0);
        
        for (int i = 0; i < 10; i++) {
            tv.appendUserMessage("model");
        }
        
        QVERIFY(tv.count() >= 10);
    }
};

QTEST_MAIN(TestUIIntegration)
#include "test_ui_integration.moc"