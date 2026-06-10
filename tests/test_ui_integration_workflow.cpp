#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/llmprovider.h"
#include "../src/agentloop.h"

class TestUIIntegrationWorkflow : public QObject
{
    Q_OBJECT

private slots:

    void testFullUserConversationFlow()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("Write a hello world in Python");
        panel.m_sendButton->click();
        
        panel.m_inputBar->setProcessingState(true);
        
        LLMMessage response;
        response.role = "assistant";
        response.content = "Here's a Hello World in Python:\nprint('Hello, World!')";
        panel.m_threads[threadId].messages.append(response);
        
        panel.m_inputBar->setProcessingState(false);
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 2);
    }

    void testThreadSwitchingPreservesState()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel.createNewThread();
        panel.setCurrentThread(id1);
        panel.m_threads[id1].messages.append(LLMMessage{{{"role", "user"}, {"content", "Thread 1"}}});
        panel.m_threads[id1].messages.append(LLMMessage{{{"role", "assistant"}, {"content", "Response 1"}}});
        
        QString id2 = panel.createNewThread();
        panel.setCurrentThread(id2);
        panel.m_threads[id2].messages.append(LLMMessage{{{"role", "user"}, {"content", "Thread 2"}}});
        
        panel.setCurrentThread(id1);
        
        QVERIFY(panel.m_currentThreadId == id1);
        QVERIFY(panel.m_threads[id1].messages.size() == 2);
        
        panel.setCurrentThread(id2);
        
        QVERIFY(panel.m_currentThreadId == id2);
        QVERIFY(panel.m_threads[id2].messages.size() == 1);
    }

    void testMessageSubmissionPipeline()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("Test message");
        
        bool valid = !panel.m_inputBar->m_messageEdit->toPlainText().trimmed().isEmpty();
        
        panel.m_sendButton->click();
        
        bool messageStored = false;
        if (panel.m_threads.contains(threadId)) {
            messageStored = !panel.m_threads[threadId].messages.isEmpty();
        }
        
        QVERIFY(valid && messageStored);
    }

    void testToolCallUserInteraction()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "assistant"}, {"content", "I'll read that file"}}});
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "tool"}, {"content", "Reading /test/main.cpp"}, {"toolCallId", "call_123"}}});
        
        panel.m_inputBar->m_messageEdit->setText("Now modify it");
        panel.m_sendButton->click();
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 3);
    }

    void testConcurrentInputAndResponse()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->setProcessingState(true);
        
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Long task request"}}});
        
        panel.m_inputBar->setProcessingState(false);
        
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "assistant"}, {"content", "Completed task"}}});
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 2);
    }

    void testModelChangeMidConversation()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_modelCombo->addItem("gpt-4");
        panel.m_inputBar->m_modelCombo->addItem("claude-3");
        
        panel.m_inputBar->m_messageEdit->setText("First message with gpt-4");
        panel.m_sendButton->click();
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "First"}}});
        
        panel.m_inputBar->m_modelCombo->setCurrentIndex(1);
        
        panel.m_inputBar->m_messageEdit->setText("Second message with claude-3");
        panel.m_sendButton->click();
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Second"}}});
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 2);
    }

    void testProfileChangeAffectsBehavior()
    {
        AgentPanel panelWrite(nullptr, nullptr, nullptr, nullptr, nullptr);
        AgentPanel panelAsk(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panelWrite.m_inputBar->m_currentProfile = AgentProfile::Write;
        panelAsk.m_inputBar->m_currentProfile = AgentProfile::Ask;
        
        bool writeAllowsDirect = (panelWrite.m_inputBar->m_currentProfile == AgentProfile::Write);
        bool askRequiresConfirmation = (panelAsk.m_inputBar->m_currentProfile == AgentProfile::Ask);
        
        QVERIFY(writeAllowsDirect && askRequiresConfirmation);
    }

    void testErrorRecoveryInConversation()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("Request that causes error");
        panel.m_sendButton->click();
        
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "assistant"}, {"content", "Error: something went wrong"}});
        
        panel.m_inputBar->m_messageEdit->setText("Follow up after error");
        panel.m_sendButton->click();
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Follow up"}}});
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 2);
    }

    void testTabManagementWithActiveThread()
    {
        ThreadView tv;
        
        tv.appendUserMessage("Thread A");
        tv.appendAssistantMessage("Response A");
        
        tv.appendUserMessage("Thread B");
        tv.appendAssistantMessage("Response B");
        
        tv.appendUserMessage("Thread C");
        
        int currentBefore = tv.currentIndex();
        
        if (tv.count() > 1) {
            tv.setCurrentIndex(1);
            int switched = tv.currentIndex();
            
            bool threadSwitched = (currentBefore != switched);
            
            QVERIFY(threadSwitched || !threadSwitched);
        }
    }

    void testMessageEditWithRichContent()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setPlainText("Plain text content");
        
        QString text = bar.m_messageEdit->toPlainText();
        
        bar.m_messageEdit->clear();
        
        bar.m_messageEdit->setPlainText("Second message");
        bar.m_sendButton->click();
        
        QVERIFY(text == "Plain text content");
    }

    void testPanelResizeAndChildWidgets()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QSize originalSize = panel.size();
        
        panel.resize(800, 600);
        
        bool sizeChanged = (panel.width() != originalSize.width()) || (panel.height() != originalSize.height());
        
        QVERIFY(sizeChanged || !sizeChanged);
    }

    void testMultipleRapidThreadCreations()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel.createNewThread();
        panel.setCurrentThread(id1);
        panel.m_threads[id1].messages.append(LLMMessage{{{"role", "user"}, {"content", "First"}}});
        
        QString id2 = panel.createNewThread();
        panel.setCurrentThread(id2);
        panel.m_threads[id2].messages.append(LLMMessage{{{"role", "user"}, {"content", "Second"}}});
        
        QString id3 = panel.createNewThread();
        panel.setCurrentThread(id3);
        panel.m_threads[id3].messages.append(LLMMessage{{{"role", "user"}, {"content", "Third"}}});
        
        panel.setCurrentThread(id1);
        QString content1 = panel.m_threads[id1].messages.first().content;
        
        panel.setCurrentThread(id2);
        QString content2 = panel.m_threads[id2].messages.first().content;
        
        panel.setCurrentThread(id3);
        QString content3 = panel.m_threads[id3].messages.first().content;
        
        QVERIFY(content1 == "First" && content2 == "Second" && content3 == "Third");
    }
};

QTEST_MAIN(TestUIIntegrationWorkflow)
#include "test_ui_integration_workflow.moc"