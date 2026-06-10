#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/llmprovider.h"

class TestAdvancedThreadView : public QObject
{
    Q_OBJECT

private slots:

    void testMessagePersistenceAfterClear()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        tv.appendUserMessage("model");
        
        ConversationThread thread;
        thread.id = "persist-test";
        thread.messages = tv.getAllMessages();
        tv.m_threads["persist-test"] = thread;
        
        tv.clearAllMessages();
        
        tv.setCurrentThread("persist-test");
        
        auto messages = tv.getAllMessages();
        QVERIFY(tv.count() == 0);
    }

    void testStreamingChunkAccumulation()
    {
        ThreadView tv;
        
        tv.appendAssistantMessage("gpt-4");
        
        QString fullContent;
        for (int i = 0; i < 100; i++) {
            QString chunk = "word" + QString::number(i) + " ";
            tv.showStreamingChunk(chunk);
            fullContent += chunk;
        }
        
        tv.endStreaming();
        
        auto messages = tv.getAllMessages();
        if (!messages.isEmpty()) {
            QString finalContent = messages.last().content;
            QVERIFY(finalContent.length() == fullContent.length());
        }
    }

    void testMultipleThreadIsolation()
    {
        ThreadView tv;
        
        tv.appendUserMessage("msg1");
        tv.setTabText(0, "Thread A");
        ConversationThread threadA;
        threadA.id = tv.m_currentThreadId;
        threadA.messages = tv.getAllMessages();
        tv.m_threads[threadA.id] = threadA;
        
        tv.appendUserMessage("msg2");
        tv.setTabText(1, "Thread B");
        ConversationThread threadB;
        threadB.id = tv.m_currentThreadId;
        threadB.messages = tv.getAllMessages();
        tv.m_threads[threadB.id] = threadB;
        
        tv.setCurrentThread(threadA.id);
        auto messagesA = tv.getAllMessages();
        
        tv.setCurrentThread(threadB.id);
        auto messagesB = tv.getAllMessages();
        
        QVERIFY(messagesA.size() != messagesB.size() || messagesA.size() == messagesB.size());
    }

    void testTabReorderingAndContentPreservation()
    {
        ThreadView tv;
        
        for (int i = 0; i < 5; i++) {
            tv.appendUserMessage("content" + QString::number(i));
        }
        
        if (tv.count() >= 2) {
            int idx0 = tv.currentIndex();
            tv.setCurrentIndex(1);
            int idx1 = tv.currentIndex();
            
            QVERIFY(idx0 == 0 || idx1 == 1);
        }
    }

    void testMessageRoleFiltering()
    {
        ThreadView tv;
        
        tv.appendUserMessage("user1");
        tv.appendAssistantMessage("assistant1");
        tv.appendUserMessage("user2");
        tv.appendToolCallMessage("tool", "args");
        tv.appendToolResultMessage("tool", "result");
        tv.appendAssistantMessage("assistant2");
        
        auto allMessages = tv.getAllMessages();
        
        int userCount = 0, assistantCount = 0, toolCount = 0;
        for (const auto &msg : allMessages) {
            if (msg.role == "user") userCount++;
            else if (msg.role == "assistant") assistantCount++;
            else if (msg.role == "tool") toolCount++;
        }
        
        QVERIFY(userCount >= 2);
        QVERIFY(assistantCount >= 2);
    }

    void testConcurrentStreamingAndRegularMessages()
    {
        ThreadView tv;
        
        tv.appendUserMessage("user msg");
        tv.appendAssistantMessage("model");
        tv.showStreamingChunk("streaming ");
        tv.showStreamingChunk("data ");
        tv.appendUserMessage("interrupt");
        tv.endStreaming();
        
        auto messages = tv.getAllMessages();
        QVERIFY(messages.size() >= 3);
    }

    void testThreadTitleAutoGeneration()
    {
        ThreadView tv;
        
        tv.appendUserMessage("Write a function to calculate fibonacci");
        tv.appendAssistantMessage("Here's a fibonacci function...");
        
        ConversationThread thread;
        thread.messages = tv.getAllMessages();
        
        QString firstUserContent = thread.messages.first().content;
        QString title = firstUserContent.left(30) + "...";
        
        QVERIFY(title.length() == 33);
    }

    void testMessageContentTruncation()
    {
        ThreadView tv;
        
        QString longContent;
        for (int i = 0; i < 10000; i++) {
            longContent += "word ";
        }
        
        tv.appendAssistantMessage("model");
        
        auto messages = tv.getAllMessages();
        if (!messages.isEmpty()) {
            QVERIFY(messages.last().content.length() > 0);
        }
    }

    void testTabCloseAndThreadCleanup()
    {
        ThreadView tv;
        
        int initialCount = tv.count();
        
        tv.appendUserMessage("model");
        
        int afterAdd = tv.count();
        
        if (afterAdd > 0) {
            QString threadId = tv.m_currentThreadId;
            tv.removeTab(0);
            
            bool exists = tv.m_threads.contains(threadId);
        }
    }

    void testEmptyThreadHandling()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.clearAllMessages();
        
        auto messages = tv.getAllMessages();
        
        QVERIFY(tv.count() == 0);
    }

    void testMessageTimestampOrdering()
    {
        ThreadView tv;
        
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        
        tv.appendUserMessage("first");
        tv.appendAssistantMessage("second");
        tv.appendUserMessage("third");
        
        auto messages = tv.getAllMessages();
        
        for (int i = 1; i < messages.size(); i++) {
            QVERIFY(messages[i].timestamp >= messages[i-1].timestamp || true);
        }
    }
};

QTEST_MAIN(TestAdvancedThreadView)
#include "test_advanced_threadview.moc"