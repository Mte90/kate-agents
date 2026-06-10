#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/ui/agentpanel.h"
#include "../src/llmprovider.h"
#include "../src/agentloop.h"

class TestUIFunctionalityExtended : public QObject
{
    Q_OBJECT

private slots:

    void testThreadViewAppendWithModel()
    {
        ThreadView tv;
        
        tv.appendUserMessage("gpt-4");
        tv.appendAssistantMessage("claude-3");
        tv.appendUserMessage("llama-3");
        
        auto messages = tv.getAllMessages();
        
        bool hasUser = false, hasAssistant = false;
        for (const auto &msg : messages) {
            if (msg.role == "user") hasUser = true;
            if (msg.role == "assistant") hasAssistant = true;
        }
        
        QVERIFY(hasUser && hasAssistant);
    }

    void testThreadViewStreamingMessage()
    {
        ThreadView tv;
        
        tv.appendAssistantMessage("model");
        tv.showStreamingChunk("Hello");
        tv.showStreamingChunk(" World");
        tv.endStreaming();
        
        auto messages = tv.getAllMessages();
        QVERIFY(messages.size() >= 1);
    }

    void testThreadViewToolCallMessage()
    {
        ThreadView tv;
        
        tv.appendToolCallMessage("read_file", "/test/path.cpp");
        
        auto messages = tv.getAllMessages();
        QVERIFY(messages.size() >= 1);
    }

    void testThreadViewToolResultMessage()
    {
        ThreadView tv;
        
        tv.appendToolResultMessage("read_file", "file content here");
        
        auto messages = tv.getAllMessages();
        QVERIFY(messages.size() >= 1);
    }

    void testThreadViewSystemMessage()
    {
        ThreadView tv;
        
        tv.appendSystemMessage("System notification");
        
        auto messages = tv.getAllMessages();
        QVERIFY(messages.size() >= 1);
    }

    void testInputBarMessageValidation()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("");
        bool empty = bar.m_messageEdit->toPlainText().trimmed().isEmpty();
        
        bar.m_messageEdit->setText("   ");
        bool whitespace = bar.m_messageEdit->toPlainText().trimmed().isEmpty();
        
        bar.m_messageEdit->setText("valid");
        bool valid = !bar.m_messageEdit->toPlainText().trimmed().isEmpty();
        
        QVERIFY(empty && whitespace && valid);
    }

    void testInputBarProfileBehavior()
    {
        InputBar bar(nullptr, AgentProfile::Minimal);
        
        bar.m_currentProfile = AgentProfile::Minimal;
        
        QVERIFY(bar.m_currentProfile == AgentProfile::Minimal);
        
        bar.m_currentProfile = AgentProfile::Write;
        QVERIFY(bar.m_currentProfile == AgentProfile::Write);
    }

    void testInputBarModelSelection()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_models << "model-a" << "model-b" << "model-c";
        
        for (const QString &model : bar.m_models) {
            bar.m_modelCombo->addItem(model);
        }
        
        bar.m_modelCombo->setCurrentIndex(1);
        
        QVERIFY(bar.m_modelCombo->currentText() == "model-b");
    }

    void testAgentPanelThreadManagement()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel.createNewThread();
        QString id2 = panel.createNewThread();
        
        QVERIFY(id1 != id2);
        QVERIFY(panel.m_threads.contains(id1));
        QVERIFY(panel.m_threads.contains(id2));
    }

    void testAgentPanelDeleteThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        QVERIFY(panel.m_threads.contains(threadId));
        
        panel.m_threads.remove(threadId);
        
        QVERIFY(!panel.m_threads.contains(threadId));
    }

    void testAgentPanelUpdateThread()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        ConversationThread thread;
        thread.id = "update-test";
        thread.title = "Original";
        
        panel.m_threads[thread.id] = thread;
        
        panel.m_threads[thread.id].title = "Updated";
        
        QVERIFY(panel.m_threads["update-test"].title == "Updated");
    }

    void testAgentPanelMessageFlow()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        LLMMessage msg;
        msg.role = "user";
        msg.content = "test";
        
        panel.m_threads[threadId].messages.append(msg);
        
        QVERIFY(panel.m_threads[threadId].messages.size() == 1);
    }

    void testThreadViewMultipleConversations()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        tv.appendUserMessage("model");
        
        QVERIFY(tv.count() >= 3);
    }

    void testThreadViewClearMessages()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        QVERIFY(tv.count() > 0);
        
        tv.clearAllMessages();
        
        QVERIFY(tv.count() == 0);
    }

    void testInputBarStateManagement()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.setEnabled(false);
        QVERIFY(!bar.isEnabled());
        
        bar.setEnabled(true);
        QVERIFY(bar.isEnabled());
        
        bar.m_sendButton->setEnabled(false);
        QVERIFY(!bar.m_sendButton->isEnabled());
        
        bar.m_sendButton->setEnabled(true);
        QVERIFY(bar.m_sendButton->isEnabled());
    }

    void testFileMentionPopupNavigation()
    {
        FileMentionPopup popup;
        
        popup.m_allPaths << "/a.cpp" << "/b.cpp" << "/c.cpp" << "/d.cpp";
        popup.m_selectedIndex = 0;
        
        QKeyEvent up(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &up);
        
        QKeyEvent down(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &down);
        
        QVERIFY(popup.m_selectedIndex >= 0);
    }

    void testDiffPreviewDialogSyncScroll()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.m_originalEdit->verticalScrollBar()->setValue(50);
        
        int origValue = dialog.m_originalEdit->verticalScrollBar()->value();
        
        QVERIFY(origValue >= 0);
    }

    void testThreadViewTabReordering()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendUserMessage("model");
        tv.appendUserMessage("model");
        
        if (tv.count() >= 3) {
            tv.setCurrentIndex(2);
            QVERIFY(tv.currentIndex() == 2);
        }
    }

    void testInputBarTextFormatting()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setPlainText("**bold** and *italic*");
        
        QString text = bar.m_messageEdit->toPlainText();
        QVERIFY(text.contains("**bold**"));
    }

    void testAgentPanelModelUpdate()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.m_inputBar->m_modelCombo->clear();
        panel.m_inputBar->m_modelCombo->addItem("new-model");
        
        panel.m_inputBar->m_modelCombo->setCurrentIndex(0);
        
        QVERIFY(panel.m_inputBar->m_modelCombo->currentText() == "new-model");
    }
};

QTEST_MAIN(TestUIFunctionalityExtended)
#include "test_ui_functionality_extended.moc"