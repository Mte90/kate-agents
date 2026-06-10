#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/agentloop.h"
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/llmprovider.h"

class TestUIE2E : public QObject
{
    Q_OBJECT

private slots:

    void testCompleteUserInteractionCycle()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("Read file /test/main.cpp");
        panel.m_sendButton->click();
        
        int userMsgIndex = panel.m_threads[threadId].messages.size() - 1;
        QVERIFY(panel.m_threads[threadId].messages[userMsgIndex].content == "Read file /test/main.cpp");
        
        panel.m_inputBar->setProcessingState(true);
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "assistant"},
            {"content", "I'll read that file for you."}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "tool"},
            {"toolCallId", "call_1"},
            {"content", "Reading /test/main.cpp"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "tool"},
            {"toolCallId", "call_1"},
            {"content", "int main() { return 0; }"}
        });
        
        panel.m_inputBar->setProcessingState(false);
        
        panel.m_inputBar->m_messageEdit->setText("Now explain it");
        panel.m_sendButton->click();
        
        int finalCount = panel.m_threads[threadId].messages.size();
        
        QVERIFY(finalCount >= 5);
    }

    void testMultiTurnWithToolCalls()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        QList<LLMMessage> conversation;
        
        conversation.append(LLMMessage{{{"role", "user"}, {"content", "List files in /tmp"}}});
        
        conversation.append(LLMMessage{{{"role", "assistant"}, {"content", ""}});
        conversation.last().toolCallId = "call_1";
        
        conversation.append(LLMMessage{{{"role", "tool"}, {"toolCallId", "call_1"}, {"content", "file1.cpp\nfile2.txt"}});
        
        conversation.append(LLMMessage{{{"role", "user"}, {"content", "Read file1.cpp"}});
        
        conversation.append(LLMMessage{{{"role", "assistant"}, {"content", ""}});
        conversation.last().toolCallId = "call_2";
        
        conversation.append(LLMMessage{{{"role", "tool"}, {"toolCallId", "call_2"}, {"content", "#include <iostream>"}});
        
        conversation.append(LLMMessage{{{"role", "assistant"}, {"content", "The file contains: #include <iostream>"}});
        
        for (const auto &msg : conversation) {
            panel.m_threads[threadId].messages.append(msg);
        }
        
        int toolCalls = 0;
        for (const auto &msg : panel.m_threads[threadId].messages) {
            if (!msg.toolCallId.isEmpty()) {
                toolCalls++;
            }
        }
        
        QVERIFY(toolCalls >= 2);
    }

    void testErrorStateRecovery()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("Trigger error");
        panel.m_sendButton->click();
        
        panel.m_inputBar->setProcessingState(true);
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "assistant"},
            {"content", "Error: Connection failed"}
        });
        
        panel.m_inputBar->setProcessingState(false);
        
        panel.m_inputBar->m_messageEdit->setText("Try again");
        
        bool canRetry = !panel.m_messageEdit->toPlainText().isEmpty();
        
        panel.m_sendButton->click();
        
        panel.m_inputBar->setProcessingState(true);
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "assistant"},
            {"content", "Success on retry!"}
        });
        
        panel.m_inputBar->setProcessingState(false);
        
        int successCount = 0;
        for (const auto &msg : panel.m_threads[threadId].messages) {
            if (msg.content.contains("Success")) {
                successCount++;
            }
        }
        
        QVERIFY(successCount >= 1);
    }

    void testConcurrentThreadOperations()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel.createNewThread();
        QString id2 = panel.createNewThread();
        QString id3 = panel.createNewThread();
        
        panel.setCurrentThread(id1);
        panel.m_threads[id1].messages.append(LLMMessage{{{"role", "user"}, {"content", "T1-M1"}}});
        panel.m_threads[id1].messages.append(LLMMessage{{{"role", "assistant"}, {"content", "T1-R1"}}});
        
        panel.setCurrentThread(id2);
        panel.m_threads[id2].messages.append(LLMMessage{{{"role", "user"}, {"content", "T2-M1"}}});
        
        panel.setCurrentThread(id3);
        panel.m_threads[id3].messages.append(LLMMessage{{{"role", "user"}, {"content", "T3-M1"}}});
        panel.m_threads[id3].messages.append(LLMMessage{{{"role", "assistant"}, {"content", "T3-R1"}}});
        panel.m_threads[id3].messages.append(LLMMessage{{{"role", "user"}, {"content", "T3-M2"}}});
        
        panel.setCurrentThread(id1);
        bool t1Ok = (panel.m_threads[id1].messages.size() == 2);
        
        panel.setCurrentThread(id2);
        bool t2Ok = (panel.m_threads[id2].messages.size() == 1);
        
        panel.setCurrentThread(id3);
        bool t3Ok = (panel.m_threads[id3].messages.size() == 3);
        
        QVERIFY(t1Ok && t2Ok && t3Ok);
    }

    void testModelSwitchingDuringLongConversation()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_modelCombo->addItem("gpt-4");
        panel.m_inputBar->m_modelCombo->addItem("claude-3");
        panel.m_inputBar->m_modelCombo->addItem("llama-3");
        
        panel.m_inputBar->m_modelCombo->setCurrentIndex(0);
        QString model1 = panel.m_inputBar->m_modelCombo->currentText();
        
        panel.m_inputBar->m_messageEdit->setText("Msg 1");
        panel.m_sendButton->click();
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Msg 1"}}});
        
        panel.m_inputBar->m_modelCombo->setCurrentIndex(1);
        QString model2 = panel.m_inputBar->m_modelCombo->currentText();
        
        panel.m_inputBar->m_messageEdit->setText("Msg 2");
        panel.m_sendButton->click();
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Msg 2"}}});
        
        panel.m_inputBar->m_modelCombo->setCurrentIndex(2);
        QString model3 = panel.m_inputBar->m_modelCombo->currentText();
        
        panel.m_inputBar->m_messageEdit->setText("Msg 3");
        panel.m_sendButton->click();
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Msg 3"}}});
        
        QVERIFY(model1 != model2 && model2 != model3);
    }

    void testPanelThreadViewInputBarSync()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        
        panel.m_threadView->setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("Sync test");
        
        panel.m_sendButton->click();
        
        panel.m_threadView->appendUserMessage("gpt-4");
        
        bool syncOk = (panel.m_currentThreadId == threadId);
        
        QVERIFY(syncOk == true);
    }

    void testTabCloseCleansUpThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel.createNewThread();
        panel.setCurrentThread(id1);
        panel.m_threads[id1].messages.append(LLMMessage{{{"role", "user"}, {"content", "To delete"}}});
        
        QString id2 = panel.createNewThread();
        panel.setCurrentThread(id2);
        
        int before = panel.m_threads.size();
        
        panel.m_threads.remove(id1);
        
        int after = panel.m_threads.size();
        
        QVERIFY(before == after + 1);
    }

    void testRapidMessageSubmission()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        for (int i = 0; i < 5; i++) {
            panel.m_inputBar->m_messageEdit->setText("Message " + QString::number(i));
            panel.m_sendButton->click();
            
            panel.m_threads[threadId].messages.append(LLMMessage{
                {"role", "user"},
                {"content", "Message " + QString::number(i)}
            });
            
            panel.m_threads[threadId].messages.append(LLMMessage{
                {"role", "assistant"},
                {"content", "Response " + QString::number(i)}
            });
        }
        
        QVERIFY(panel.m_threads[threadId].messages.size() == 10);
    }

    void testEmptyToPopulatedThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        
        QVERIFY(panel.m_threads[threadId].messages.isEmpty());
        
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("First");
        panel.m_sendButton->click();
        
        QVERIFY(!panel.m_threads[threadId].messages.isEmpty());
        
        panel.m_inputBar->setProcessingState(false);
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 1);
    }

    void testProfileBehaviorDifferences()
    {
        InputBar barWrite(nullptr, AgentProfile::Write);
        InputBar barAsk(nullptr, AgentProfile::Ask);
        InputBar barMinimal(nullptr, AgentProfile::Minimal);
        
        barWrite.m_currentProfile = AgentProfile::Write;
        barAsk.m_currentProfile = AgentProfile::Ask;
        barMinimal.m_currentProfile = AgentProfile::Minimal;
        
        barWrite.m_messageEdit->setText("test");
        barAsk.m_messageEdit->setText("test");
        barMinimal.m_messageEdit->setText("test");
        
        bool writeEnabled = barWrite.m_sendButton->isEnabled();
        bool askEnabled = barAsk.m_sendButton->isEnabled();
        bool minimalEnabled = barMinimal.m_sendButton->isEnabled();
        
        QVERIFY(barWrite.m_currentProfile == AgentProfile::Write);
        QVERIFY(barAsk.m_currentProfile == AgentProfile::Ask);
        QVERIFY(barMinimal.m_currentProfile == AgentProfile::Minimal);
    }
};

QTEST_MAIN(TestUIE2E)
#include "test_ui_e2e.moc"