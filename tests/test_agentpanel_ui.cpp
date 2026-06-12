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

    void testTabHashConsistency()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        // Create several threads
        QString id1 = panel.createNewThread();
        QString id2 = panel.createNewThread();
        QString id3 = panel.createNewThread();
        
        // Verify hash is populated
        QVERIFY(panel.m_tabHash.contains(id1));
        QVERIFY(panel.m_tabHash.contains(id2));
        QVERIFY(panel.m_tabHash.contains(id3));
        
        // Verify correct indices (0, 1, 2)
        QCOMPARE(panel.m_tabHash.value(id1), 0);
        QCOMPARE(panel.m_tabHash.value(id2), 1);
        QCOMPARE(panel.m_tabHash.value(id3), 2);
        
        // Close middle tab and verify re-indexing
        panel.closeChatTab(1); // Close id2
        
        QVERIFY(!panel.m_tabHash.contains(id2));
        QVERIFY(panel.m_tabHash.contains(id1));
        QVERIFY(panel.m_tabHash.contains(id3));
        
        // id3 should now be at index 1
        QCOMPARE(panel.m_tabHash.value(id3), 1);
    }
};

QTEST_MAIN(TestAgentPanelUI)
#include "test_agentpanel_ui.moc"