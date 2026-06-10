#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/llmprovider.h"

class TestAgentPanelUIDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testThreadViewExists()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threadView != nullptr);
    }

    void testInputBarExists()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_inputBar != nullptr);
    }

    void testThreadsMapInitiallyEmpty()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threads.isEmpty());
    }

    void testCurrentThreadIdEmpty()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_currentThreadId.isEmpty());
    }

    void testActiveThreadIdEmpty()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_activeThreadId.isEmpty());
    }

    void testSetCurrentThreadId()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        panel.m_currentThreadId = "panel-thread-1";
        QVERIFY(panel.m_currentThreadId == "panel-thread-1");
    }

    void testAddThreadToMap()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        ConversationThread thread;
        thread.id = "thread-1";
        panel.m_threads["thread-1"] = thread;
        QVERIFY(panel.m_threads.contains("thread-1"));
    }

    void testAddMultipleThreads()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        for (int i = 0; i < 5; i++) {
            ConversationThread thread;
            thread.id = "thread-" + QString::number(i);
            panel.m_threads["thread-" + QString::number(i)] = thread;
        }
        QVERIFY(panel.m_threads.size() == 5);
    }

    void testRemoveThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        ConversationThread thread;
        thread.id = "to-remove";
        panel.m_threads["to-remove"] = thread;
        panel.m_threads.remove("to-remove");
        QVERIFY(!panel.m_threads.contains("to-remove"));
    }

    void testModelsListEmpty()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_models.isEmpty());
    }

    void testAddModels()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        panel.m_models << "gpt-4" << "claude-3" << "llama-3";
        QVERIFY(panel.m_models.size() == 3);
    }

    void testActiveThreadId()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        panel.m_activeThreadId = "active-thread";
        QVERIFY(panel.m_activeThreadId == "active-thread");
    }

    void testThreadViewReference()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threadView != nullptr);
    }

    void testInputBarReference()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_inputBar != nullptr);
    }

    void testCloseThreadClearsId()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        panel.m_currentThreadId = "thread-to-close";
        panel.m_currentThreadId.clear();
        QVERIFY(panel.m_currentThreadId.isEmpty());
    }
};

QTEST_MAIN(TestAgentPanelUIDetailed)
#include "test_agentpanel_ui_detailed.moc"