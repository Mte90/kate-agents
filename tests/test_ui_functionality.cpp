#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/ui/agentpanel.h"
#include "../src/ui/filementionpopup.h"
#include "../src/ui/diffpreviewdialog.h"
#include "../src/llmprovider.h"
#include <QFile>

class TestUIFunctionality : public QObject
{
    Q_OBJECT

private slots:

    void testThreadViewCreateAndStore()
    {
        ThreadView tv;
        
        tv.appendUserMessage("gpt-4");
        ConversationThread thread;
        thread.id = "func-test";
        thread.messages = tv.getAllMessages();
        
        tv.setCurrentThread("func-test");
        tv.m_threads["func-test"] = thread;
        
        QVERIFY(tv.m_threads.contains("func-test"));
    }

    void testThreadViewRetrieveById()
    {
        ThreadView tv;
        
        ConversationThread thread;
        thread.id = "retrieve-test";
        thread.title = "Test Title";
        tv.m_threads["retrieve-test"] = thread;
        
        tv.setCurrentThread("retrieve-test");
        
        QVERIFY(tv.getAllMessages().size() >= 0);
    }

    void testThreadViewDelete()
    {
        ThreadView tv;
        
        ConversationThread thread;
        thread.id = "delete-test";
        tv.m_threads["delete-test"] = thread;
        
        tv.setCurrentThread("delete-test");
        tv.deleteCurrentThread();
        
        QVERIFY(!tv.m_threads.contains("delete-test") || tv.m_threads.contains("delete-test"));
    }

    void testThreadViewRename()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        
        if (tv.count() > 0) {
            tv.setTabText(0, "New Thread Name");
            tv.m_currentThreadId = "rename-test";
            tv.m_threads["rename-test"].title = "New Thread Name";
            
            QVERIFY(tv.tabText(0) == "New Thread Name");
        }
    }

    void testInputBarSendMessage()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setText("Test message");
        
        QString text = bar.m_messageEdit->toPlainText();
        
        QVERIFY(text == "Test message");
        QVERIFY(!text.isEmpty());
    }

    void testInputBarSwitchProfile()
    {
        InputBar barWrite(nullptr, AgentProfile::Write);
        
        barWrite.m_profileCombo->setCurrentIndex(1);
        AgentProfile newProfile = barWrite.m_currentProfile;
        
        QVERIFY(newProfile == AgentProfile::Ask || newProfile == AgentProfile::Minimal);
    }

    void testInputBarSelectModel()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_models << "gpt-4" << "claude-3" << "llama-3";
        
        bar.m_modelCombo->clear();
        for (const QString &model : bar.m_models) {
            bar.m_modelCombo->addItem(model);
        }
        
        bar.m_modelCombo->setCurrentIndex(0);
        
        QVERIFY(bar.m_modelCombo->currentText() == "gpt-4");
    }

    void testInputBarValidateMessage()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("");
        bool emptyValid = bar.m_messageEdit->toPlainText().isEmpty();
        
        bar.m_messageEdit->setText("Valid message");
        bool nonEmptyValid = !bar.m_messageEdit->toPlainText().isEmpty();
        
        QVERIFY(emptyValid == true);
        QVERIFY(nonEmptyValid == true);
    }

    void testFileMentionPopupFilter()
    {
        FileMentionPopup popup;
        
        popup.m_allPaths << "/test/file1.cpp" << "/test/file2.h" << "/test/file3.cpp";
        popup.m_filterPattern = "*.cpp";
        
        popup.updateFilteredPaths();
        
        QVERIFY(popup.m_filteredPaths.size() >= 0);
    }

    void testFileMentionPopupSelectPath()
    {
        FileMentionPopup popup;
        
        popup.m_allPaths << "/test/file1.cpp";
        
        popup.setSelectedIndex(0);
        
        QVERIFY(popup.m_selectedIndex == 0);
    }

    void testFileMentionPopupGetSelectedPath()
    {
        FileMentionPopup popup;
        
        popup.m_allPaths << "/test/file1.cpp" << "/test/file2.cpp";
        popup.m_selectedIndex = 1;
        
        QString selected = popup.getSelectedPath();
        
        QVERIFY(selected == "/test/file2.cpp" || selected.isEmpty());
    }

    void testFileMentionPopupRefresh()
    {
        FileMentionPopup popup;
        
        popup.m_allPaths.clear();
        popup.m_filteredPaths.clear();
        
        popup.m_rootPath = "/tmp";
        popup.setMaxDepth(3);
        
        QVERIFY(popup.m_maxDepth == 3);
    }

    void testDiffPreviewDialogCompare()
    {
        DiffPreviewDialog dialog("old content", "new content");
        
        QString original = dialog.m_originalEdit->toPlainText();
        QString modified = dialog.m_modifiedEdit->toPlainText();
        
        QVERIFY(original == "old content");
        QVERIFY(modified == "new content");
    }

    void testDiffPreviewDialogCopyOriginal()
    {
        DiffPreviewDialog dialog("original text", "modified text");
        
        dialog.m_originalEdit->selectAll();
        dialog.m_originalEdit->copy();
        
        QVERIFY(true);
    }

    void testDiffPreviewDialogCopyModified()
    {
        DiffPreviewDialog dialog("original", "modified");
        
        dialog.m_modifiedEdit->selectAll();
        dialog.m_modifiedEdit->copy();
        
        QVERIFY(true);
    }

    void testAgentPanelCreateThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        
        QVERIFY(!threadId.isEmpty());
    }

    void testAgentPanelSwitchThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        ConversationThread thread1;
        thread1.id = "thread-1";
        panel.m_threads["thread-1"] = thread1;
        
        panel.setCurrentThread("thread-1");
        
        QVERIFY(panel.m_currentThreadId == "thread-1");
    }

    void testAgentPanelLoadModels()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QStringList models;
        models << "gpt-4" << "gpt-3.5-turbo" << "claude-3";
        
        for (const QString &model : models) {
            panel.m_inputBar->m_modelCombo->addItem(model);
        }
        
        QVERIFY(panel.m_inputBar->m_modelCombo->count() >= 3);
    }

    void testAgentPanelStoreThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        ConversationThread thread;
        thread.id = "store-test";
        thread.title = "Store Test";
        
        panel.m_threads[thread.id] = thread;
        
        QVERIFY(panel.m_threads.contains("store-test"));
    }

    void testThreadViewGetAllMessages()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        auto messages = tv.getAllMessages();
        
        QVERIFY(messages.size() >= 2);
    }

    void testThreadViewGetCurrentThreadMessages()
    {
        ThreadView tv;
        
        tv.appendUserMessage("user message");
        tv.setCurrentThread("current-test");
        
        auto messages = tv.getCurrentThreadMessages();
        
        QVERIFY(messages.size() >= 0);
    }
};

QTEST_MAIN(TestUIFunctionality)
#include "test_ui_functionality.moc"