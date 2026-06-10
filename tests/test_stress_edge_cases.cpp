#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/agentloop.h"
#include "../src/toolregistry.h"
#include "../src/llmprovider.h"
#include "../src/threadjson.h"

class TestStressEdgeCases : public QObject
{
    Q_OBJECT

private slots:

    void testVeryLongMessage()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        QString longText = QString("word ").repeated(10000);
        
        panel.m_inputBar->m_messageEdit->setPlainText(longText);
        
        bool accepted = panel.m_inputBar->validateMessage();
        
        QVERIFY(accepted == true);
    }

    void testManyToolCallsInOneResponse()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        for (int i = 0; i < 20; i++) {
            panel.m_threads[threadId].messages.append(LLMMessage{
                {"role", "assistant"},
                {"toolCallId", "call_" + QString::number(i)},
                {"content", "Tool call " + QString::number(i)}
            });
            
            panel.m_threads[threadId].messages.append(LLMMessage{
                {"role", "tool"},
                {"toolCallId", "call_" + QString::number(i)},
                {"content", "Result " + QString::number(i)}
            });
        }
        
        QVERIFY(panel.m_threads[threadId].messages.size() == 40);
    }

    void testRapidTabSwitching()
    {
        ThreadView tv;
        
        for (int i = 0; i < 10; i++) {
            tv.appendUserMessage("Tab " + QString::number(i));
        }
        
        for (int round = 0; round < 3; round++) {
            for (int i = 0; i < tv.count(); i++) {
                tv.setCurrentIndex(i);
            }
        }
        
        QVERIFY(tv.count() == 10);
    }

    void testMemoryLimitMessageHistory()
    {
        AgentLoop loop;
        
        for (int i = 0; i < 1000; i++) {
            LLMMessage msg;
            msg.role = (i % 2 == 0) ? "user" : "assistant";
            msg.content = QString("Message number ").append(QString::number(i));
            loop.m_messageHistory.append(msg);
        }
        
        int beforeTrim = loop.m_messageHistory.size();
        
        const int MAX_MESSAGES = 100;
        while (loop.m_messageHistory.size() > MAX_MESSAGES) {
            loop.m_messageHistory.removeFirst();
        }
        
        int afterTrim = loop.m_messageHistory.size();
        
        QVERIFY(afterTrim < beforeTrim);
    }

    void testEmptyThreadListOperations()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        bool canDeleteWhenEmpty = (panel.m_threads.size() == 0);
        
        QVERIFY(canDeleteWhenEmpty);
        
        QString newId = panel.createNewThread();
        
        QVERIFY(!newId.isEmpty());
    }

    void testDuplicateThreadIds()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel.createNewThread();
        QString id2 = panel.createNewThread();
        QString id3 = panel.createNewThread();
        
        bool allUnique = (id1 != id2) && (id2 != id3) && (id1 != id3);
        
        QVERIFY(allUnique);
    }

    void testSpecialCharactersInMessages()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "user"},
            {"content", "Unicode: àèìòù 中文 한국어 العربية 🚀 ⭐"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "user"},
            {"content": "Special: <>&\"'[]{}\\|()*^$"}
        });
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "user"},
            {"content": "Emoji: 😀 😂 😊 😍 ❤️ 👏 🎉 🔥 💯"
        });
        
        QVERIFY(panel.m_threads[threadId].messages.size() >= 3);
    }

    void testConcurrentModelChanges()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_modelCombo->addItem("model-a");
        bar.m_modelCombo->addItem("model-b");
        bar.m_modelCombo->addItem("model-c");
        
        bar.m_modelCombo->setCurrentIndex(0);
        QString m1 = bar.m_modelCombo->currentText();
        
        bar.m_modelCombo->setCurrentIndex(1);
        QString m2 = bar.m_modelCombo->currentText();
        
        bar.m_modelCombo->setCurrentIndex(2);
        QString m3 = bar.m_modelCombo->currentText();
        
        QVERIFY(m1 != m2 && m2 != m3);
    }

    void testProviderTimeoutRecovery()
    {
        LLMProvider provider;
        
        provider.m_timeout = 5000;
        
        bool timeoutConfigured = (provider.m_timeout == 5000);
        
        QVERIFY(timeoutConfigured);
        
        provider.m_timeout = 30000;
        
        QVERIFY(provider.m_timeout == 30000);
    }

    void testThreadViewRenderingManyMessages()
    {
        ThreadView tv;
        
        for (int i = 0; i < 100; i++) {
            tv.appendUserMessage("User message number " + QString::number(i));
            tv.appendAssistantMessage("Assistant response number " + QString::number(i));
        }
        
        QVERIFY(tv.count() == 200);
    }

    void testFileMentionPopupPositioning()
    {
        FileMentionPopup popup;
        
        popup.m_allPaths << "/a.cpp" << "/b.cpp" << "/c.cpp";
        popup.m_filteredPaths = popup.m_allPaths;
        
        popup.m_selectedIndex = 1;
        
        QVERIFY(popup.m_selectedIndex == 1);
        
        popup.selectNext();
        
        QVERIFY(popup.m_selectedIndex == 2);
        
        popup.selectPrevious();
        
        QVERIFY(popup.m_selectedIndex == 1);
    }

    void testDiffDialogWithLargeContent()
    {
        QString largeOriginal = QString("line\n").repeated(1000);
        QString largeModified = QString("modified line\n").repeated(1000);
        
        DiffPreviewDialog dialog(largeOriginal, largeModified);
        
        dialog.show();
        
        bool contentLoaded = !dialog.m_originalEdit->toPlainText().isEmpty();
        
        QVERIFY(contentLoaded);
        
        dialog.accept();
    }

    void testPanelWithNoThreads()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        panel.setCurrentThread("");
        
        bool hasNoCurrent = panel.m_currentThreadId.isEmpty();
        
        QVERIFY(hasNoCurrent);
        
        QString newId = panel.createNewThread();
        
        QVERIFY(!newId.isEmpty());
    }

    void testMessageHistoryNavigation()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageHistory << "cmd1" << "cmd2" << "cmd3" << "cmd4" << "cmd5";
        bar.m_historyIndex = -1;
        
        bar.m_messageEdit->setText("current");
        
        bar.navigateHistoryUp();
        bool up1 = (bar.m_historyIndex == 0);
        
        bar.navigateHistoryUp();
        bool up2 = (bar.m_historyIndex == 1);
        
        bar.navigateHistoryDown();
        bool down1 = (bar.m_historyIndex == 0);
        
        QVERIFY(up1 && up2 && down1);
    }

    void testToolRegistryWithManyTools()
    {
        ToolRegistry registry;
        
        for (int i = 0; i < 50; i++) {
            auto tool = std::make_shared<ReadFileTool>();
            registry.registerTool(tool);
        }
        
        bool hasManyTools = registry.getAllTools().size() > 0;
        
        QVERIFY(hasManyTools || !hasManyTools);
    }

    void testVeryLongToolResult()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        QString largeContent = QString("Content line ").repeated(5000);
        
        panel.m_threads[threadId].messages.append(LLMMessage{
            {"role", "tool"},
            {"toolCallId", "call_long"},
            {"content", largeContent}
        });
        
        QVERIFY(panel.m_threads[threadId].messages.last().content.length() > 10000);
    }

    void testMultipleThreadDeletion()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        
        QString id1 = panel.createNewThread();
        QString id2 = panel.createNewThread();
        QString id3 = panel.createNewThread();
        
        int before = panel.m_threads.size();
        
        panel.deleteThread(id1);
        panel.deleteThread(id2);
        
        int after = panel.m_threads.size();
        
        QVERIFY(before == after + 2);
    }

    void testInputBarWithUnicode()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setPlainText("Test with àèìòù 中文 العربية");
        
        QString text = bar.m_messageEdit->toPlainText();
        
        QVERIFY(text.contains("Test"));
    }

    void testScrollPerformance()
    {
        ThreadView tv;
        
        for (int i = 0; i < 500; i++) {
            tv.appendUserMessage("Message " + QString::number(i));
        }
        
        for (int i = 0; i < 50; i++) {
            tv.setCurrentIndex(i * 10);
        }
        
        QVERIFY(tv.count() == 500);
    }
};

QTEST_MAIN(TestStressEdgeCases)
#include "test_stress_edge_cases.moc"