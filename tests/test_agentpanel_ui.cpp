#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/llmprovider.h"

class TestAgentPanelUI : public QObject
{
    Q_OBJECT

private slots:

    void testAgentPanelConstruction()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threadView != nullptr);
    }

    void testInputBarExists()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_inputBar != nullptr);
    }

    void testThreadViewExists()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threadView != nullptr);
    }

    void testEmptyThreadList()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threads.isEmpty());
    }

    void testCurrentThreadEmpty()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_currentThreadId.isEmpty());
    }

    void testSetCurrentThreadId()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        panel.m_currentThreadId = "test-123";
        QVERIFY(panel.m_currentThreadId == "test-123");
    }

    void testThreadMapEmptyByDefault()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_threads.isEmpty());
    }

    void testActiveThreadIdEmpty()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_activeThreadId.isEmpty());
    }

    void testModelsListEmptyInitially()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        QVERIFY(panel.m_models.isEmpty());
    }

    void testAddModel()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        panel.m_models.append("gpt-4");
        QVERIFY(panel.m_models.size() == 1);
    }

    void testMultipleModels()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        panel.m_models << "gpt-4" << "claude-3" << "llama-3";
        QVERIFY(panel.m_models.size() == 3);
    }
};

QTEST_MAIN(TestAgentPanelUI)
#include "test_agentpanel_ui.moc"