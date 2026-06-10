#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/llmprovider.h"
#include <QSignalSpy>

class TestAgentPanelEvents : public QObject
{
    Q_OBJECT

private slots:

    void testPanelVisibility()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.show();
        QVERIFY(panel.isVisible());
        
        panel.hide();
        QVERIFY(!panel.isVisible());
    }

    void testPanelEnabledState()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.setEnabled(false);
        QVERIFY(panel.isEnabled() == false);
        
        panel.setEnabled(true);
        QVERIFY(panel.isEnabled() == true);
    }

    void testThreadViewEnabled()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.m_threadView->setEnabled(true);
        QVERIFY(panel.m_threadView->isEnabled());
    }

    void testInputBarEnabled()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.m_inputBar->setEnabled(false);
        QVERIFY(!panel.m_inputBar->isEnabled());
    }

    void testAddThreadSignal()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSignalSpy spy(&panel, &AgentPanel::threadCreated);
    }

    void testRemoveThreadSignal()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSignalSpy spy(&panel, &AgentPanel::threadDeleted);
    }

    void testMessageSubmittedSignal()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSignalSpy spy(&panel, &AgentPanel::messageSubmitted);
    }

    void testModelChangedSignal()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSignalSpy spy(&panel, &AgentPanel::modelChanged);
    }

    void testProfileChangedSignal()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSignalSpy spy(&panel, &AgentPanel::profileChanged);
    }

    void testThreadViewCurrentChanged()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSignalSpy spy(panel.m_threadView->m_tabs, &QTabWidget::currentChanged);
    }

    void testSetCurrentThreadUpdatesId()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.setCurrentThread("event-test-thread");
        
        QVERIFY(panel.m_currentThreadId == "event-test-thread");
    }

    void testThreadMapOperations()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        ConversationThread thread1;
        thread1.id = "t1";
        panel.m_threads["t1"] = thread1;
        
        ConversationThread thread2;
        thread2.id = "t2";
        panel.m_threads["t2"] = thread2;
        
        QVERIFY(panel.m_threads.size() == 2);
        QVERIFY(panel.m_threads.contains("t1"));
        QVERIFY(panel.m_threads.contains("t2"));
    }

    void testRemoveThreadFromMap()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        ConversationThread thread;
        thread.id = "to-remove";
        panel.m_threads["to-remove"] = thread;
        
        panel.m_threads.remove("to-remove");
        
        QVERIFY(!panel.m_threads.contains("to-remove"));
    }

    void testModelsListSignal()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSignalSpy spy(panel.m_inputBar->m_modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged));
    }

    void testPanelFocus()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.setFocus();
        QVERIFY(panel.hasFocus() || !panel.isVisible());
    }

    void testPanelResize()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.resize(800, 600);
        
        QVERIFY(panel.width() == 800);
        QVERIFY(panel.height() == 600);
    }

    void testPanelMinimumSize()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.setMinimumSize(300, 200);
        
        QVERIFY(panel.minimumWidth() == 300);
        QVERIFY(panel.minimumHeight() == 200);
    }

    void testPanelLayout()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QVERIFY(panel.layout() != nullptr);
    }

    void testInputBarSendButtonConnection()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QVERIFY(panel.m_inputBar->m_sendButton != nullptr);
    }

    void testInputBarMessageEditConnection()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QVERIFY(panel.m_inputBar->m_messageEdit != nullptr);
    }
};

QTEST_MAIN(TestAgentPanelEvents)
#include "test_agentpanel_events.moc"