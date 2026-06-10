#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/llmprovider.h"

class TestThreadViewUIDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testTabsCreation()
    {
        ThreadView tv;
        QVERIFY(tv.m_tabs != nullptr);
    }

    void testTabCountStartsZero()
    {
        ThreadView tv;
        QVERIFY(tv.count() == 0);
    }

    void testAppendUserMessageIncreasesTabCount()
    {
        ThreadView tv;
        int before = tv.count();
        tv.appendUserMessage("model");
        QVERIFY(tv.count() > before);
    }

    void testAppendAssistantMessageIncreasesTabCount()
    {
        ThreadView tv;
        int before = tv.count();
        tv.appendAssistantMessage("model");
        QVERIFY(tv.count() > before);
    }

    void testMultipleMessagesTabCount()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        QVERIFY(tv.count() >= 4);
    }

    void testClearAllResetsTabCount()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        tv.clearAllMessages();
        QVERIFY(tv.count() == 0);
    }

    void testCurrentThreadIdInitiallyEmpty()
    {
        ThreadView tv;
        QVERIFY(tv.m_currentThreadId.isEmpty());
    }

    void testSetCurrentThreadStoresId()
    {
        ThreadView tv;
        tv.setCurrentThread("my-thread-123");
        QVERIFY(tv.m_currentThreadId == "my-thread-123");
    }

    void testSetCurrentThreadTwice()
    {
        ThreadView tv;
        tv.setCurrentThread("thread-1");
        tv.setCurrentThread("thread-2");
        QVERIFY(tv.m_currentThreadId == "thread-2");
    }

    void testGetAllMessagesEmptyInitially()
    {
        ThreadView tv;
        auto msgs = tv.getAllMessages();
        QVERIFY(msgs.isEmpty());
    }

    void testGetAllMessagesAfterAppend()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        auto msgs = tv.getAllMessages();
        QVERIFY(!msgs.isEmpty());
    }

    void testGetAllMessagesCount()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        tv.appendUserMessage("model");
        auto msgs = tv.getAllMessages();
        QVERIFY(msgs.size() >= 3);
    }

    void testStreamingWithCurrentThread()
    {
        ThreadView tv;
        tv.setCurrentThread("streaming-thread");
        tv.appendAssistantMessage("model");
        tv.showStreamingChunk("chunk1");
        tv.showStreamingChunk("chunk2");
    }

    void testEndStreamingTwice()
    {
        ThreadView tv;
        tv.appendAssistantMessage("model");
        tv.endStreaming();
        tv.endStreaming();
    }

    void testAppendAfterClear()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.clearAllMessages();
        tv.appendUserMessage("model");
        QVERIFY(tv.count() >= 1);
    }

    void testTabBarPolicy()
    {
        ThreadView tv;
        QVERIFY(tv.m_tabs->tabBar() != nullptr);
    }

    void testAllMessagesInitial()
    {
        ThreadView tv;
        QVERIFY(tv.m_allMessages.isEmpty());
    }

    void testAddMessageToAllMessages()
    {
        ThreadView tv;
        LLMMessage msg;
        msg.role = "user";
        msg.content = "test";
        tv.m_allMessages.append(msg);
        QVERIFY(tv.m_allMessages.size() == 1);
    }
};

QTEST_MAIN(TestThreadViewUIDetailed)
#include "test_threadview_ui_detailed.moc"