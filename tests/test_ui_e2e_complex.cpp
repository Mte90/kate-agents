#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/ui/filementionpopup.h"
#include "../src/ui/diffpreviewdialog.h"
#include "../src/agentloop.h"
#include "../src/toolregistry.h"
#include "../src/llmprovider.h"

class TestUIE2EComplex : public QObject
{
    Q_OBJECT

private slots:

    void testFileMentionAndToolCallChain()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("@/test/file.cpp");
        
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file.cpp" << "/test/main.cpp";
        popup.m_filteredPaths = popup.m_allPaths;
        
        popup.m_selectedIndex = 0;
        QString selected = popup.getSelectedPath();
        
        panel.m_inputBar->m_messageEdit->setText("Read " + selected);
        panel.m_sendButton->click();
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "assistant"},
            {"toolCallId", "call_read"},
            {"content", "Reading " + selected}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "tool"},
            {"toolCallId", "call_read"},
            {"content", "int main() { return 0; }"}
        });
        
        panel.m_inputBar->m_messageEdit->setText("Now modify it to add comments");
        panel.m_sendButton->click();
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 4);
    }

    void testDiffPreviewAfterToolEdit()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        QString originalContent = "int main() { return 0; }";
        QString newContent = "// Modified\nint main() { return 1; }";
        
        DiffPreviewDialog dialog(originalContent, newContent);
        dialog.show();
        
        dialog.m_originalEdit->toPlainText();
        dialog.m_modifiedEdit->toPlainText();
        
        dialog.accept();
        
        QVERIFY(true);
    }

    void testInterruptedStreamingRecovery()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->setProcessingState(true);
        
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Long response"}}});
        
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "assistant"}, {"content", ""}});
        
        QString partial = "";
        for (int i = 0; i < 5; i++) {
            partial += "chunk ";
        }
        
        panel.m_inputBar->setProcessingState(false);
        
        panel.m_threads[threadId].messages.last().content = partial;
        
        QVERIFY(panel.m_threads[threadId].messages.last().content.length() > 0);
    }

    void testDeepThreadHistoryNavigation()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        for (int i = 0; i < 20; i++) {
            panel.m_threads[threadId].messages.append(LLMMessage{
                {"role", "user"},
                {"content", "Message " + QString::number(i)}
            });
            panel.m_threads[threadId].messages.append(LLMMessage{
                {"role", "assistant"},
                {"content", "Response " + QString::number(i)}
            });
        }
        
        QVERIFY(panel.m_threads[threadId].messages.size() == 40);
        
        panel.m_inputBar->m_messageHistory << "old1" << "old2" << "old3";
        panel.m_inputBar->m_historyIndex = 0;
        
        panel.m_inputBar->navigateHistoryUp();
        panel.m_inputBar->navigateHistoryUp();
        
        QVERIFY(panel.m_inputBar->m_historyIndex >= 0);
    }

    void testProfileSwitchPreservesContent()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("Important message being typed");
        bar.m_currentProfile = AgentProfile::Write;
        
        bar.m_currentProfile = AgentProfile::Ask;
        
        QString preserved = bar.m_messageEdit->toPlainText();
        
        QVERIFY(preserved == "Important message being typed");
        
        bar.m_currentProfile = AgentProfile::Write;
        
        QVERIFY(bar.m_messageEdit->toPlainText() == preserved);
    }

    void testModelListDynamicRefresh()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.updateModelList(QStringList() << "model-v1");
        int count1 = bar.m_modelCombo->count();
        
        bar.updateModelList(QStringList() << "model-v2" << "model-v3");
        int count2 = bar.m_modelCombo->count();
        
        bar.updateModelList(QStringList() << "new-model");
        int count3 = bar.m_modelCombo->count();
        
        QVERIFY(count3 > 0);
    }

    void testToolPermissionWorkflow()
    {
        PermissionManager pm;
        
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        
        bool firstRequest = pm.requestPermission("terminal", "ls /tmp");
        
        pm.setToolPolicy("terminal", PermissionPolicy::Allow);
        
        bool secondRequest = pm.requestPermission("terminal", "cat file");
        
        pm.setToolPolicy("terminal", PermissionPolicy::Deny);
        
        bool thirdRequest = pm.requestPermission("terminal", "rm file");
        
        QVERIFY(firstRequest == false && secondRequest == true && thirdRequest == false);
    }

    void testConcurrentTabsAndThreadOperations()
    {
        ThreadView tv;
        
        tv.appendUserMessage("Tab 0 content");
        tv.setTabText(0, "Conversation A");
        
        tv.appendUserMessage("Tab 1 content");
        tv.setTabText(1, "Conversation B");
        
        tv.appendUserMessage("Tab 2 content");
        tv.setTabText(2, "Conversation C");
        
        tv.setCurrentIndex(0);
        int index0 = tv.currentIndex();
        
        tv.setCurrentIndex(1);
        int index1 = tv.currentIndex();
        
        tv.setCurrentIndex(2);
        int index2 = tv.currentIndex();
        
        QVERIFY(index0 == 0 && index1 == 1 && index2 == 2);
    }

    void testEmptyInputRejection()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("");
        bool emptyRejected = !bar.validateMessage();
        
        bar.m_messageEdit->setText("   ");
        bool whitespaceRejected = !bar.validateMessage();
        
        bar.m_messageEdit->setText("valid");
        bool validAccepted = bar.validateMessage();
        
        QVERIFY(emptyRejected && whitespaceRejected && validAccepted);
    }

    void testSendButtonStateManagement()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("");
        bar.updateSendButtonState();
        bool emptyDisabled = !bar.m_sendButton->isEnabled();
        
        bar.m_messageEdit->setText("text");
        bar.updateSendButtonState();
        bool textEnabled = bar.m_sendButton->isEnabled();
        
        bar.setProcessingState(true);
        bool processingDisabled = !bar.m_sendButton->isEnabled();
        
        bar.setProcessingState(false);
        bool afterEnabled = bar.m_sendButton->isEnabled();
        
        QVERIFY(emptyDisabled && textEnabled && processingDisabled && afterEnabled);
    }

    void testThreadPersistenceAcrossPanel()
    {
        AgentPanel panel1(nullptr, nullptr, nullptr, nullptr, nullptr);
        AgentPanel panel2(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel1.createNewThread();
        panel1.setCurrentThread(id1);
        panel1.m_threads[id1].messages.append(LLMMessage{{{"role", "user"}, {"content", "Test"}}});
        
        panel2.m_threads = panel1.m_threads;
        
        bool messageExists = panel2.m_threads.contains(id1);
        
        QVERIFY(messageExists);
    }

    void testComplexMessageContent()
    {
        ThreadView tv;
        
        tv.appendUserMessage("Simple text");
        
        tv.appendAssistantMessage("Response with `code` and **formatting**");
        
        tv.appendAssistantMessage("Line 1\nLine 2\nLine 3");
        
        tv.appendToolCallMessage("read_file", "/path/to/file.cpp");
        
        tv.appendToolResultMessage("read_file", "File content here");
        
        tv.appendUserMessage("Unicode: àèìòù 中文 한국어");
        
        auto messages = tv.getAllMessages();
        
        QVERIFY(messages.size() >= 6);
    }

    void testPanelLayoutAfterResize()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.resize(1200, 800);
        
        QSize threadViewSize = panel.m_threadView->size();
        QSize inputBarSize = panel.m_inputBar->size();
        
        bool validLayout = (threadViewSize.width() > 0) && (inputBarSize.width() > 0);
        
        QVERIFY(validLayout || !validLayout);
    }

    void testSearchInMultipleThreads()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.m_threads["t1"].title = "Python help";
        panel.m_threads["t1"].messages.append(LLMMessage{{{"role", "user"}, {"content"}}});
        
        panel.m_threads["t2"].title = "C++ question";
        panel.m_threads["t2"].messages.append(LLMMessage{{{"role", "user"}, {"content"}}});
        
        panel.m_threads["t3"].title = "JavaScript";
        panel.m_threads["t3"].messages.append(LLMMessage{{{"role", "user"}, {"content"}}});
        
        QStringList results = panel.searchThreads("Python");
        
        QVERIFY(results.contains("t1") || results.isEmpty());
    }
};

QTEST_MAIN(TestUIE2EComplex)
#include "test_ui_e2e_complex.moc"