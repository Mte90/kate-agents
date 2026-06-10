#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/llmprovider.h"

class TestThreadViewDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testAppendUserMessageWithModel()
    {
        ThreadView tv;
        tv.appendUserMessage("gpt-4");
        QVERIFY(tv.count() >= 1);
    }

    void testAppendAssistantMessageWithModel()
    {
        ThreadView tv;
        tv.appendAssistantMessage("claude-3");
        QVERIFY(tv.count() >= 1);
    }

    void testShowStreamingChunkMultiple()
    {
        ThreadView tv;
        tv.appendAssistantMessage("model");
        tv.showStreamingChunk("Hello");
        tv.showStreamingChunk(" ");
        tv.showStreamingChunk("World");
    }

    void testEndStreamingTwice()
    {
        ThreadView tv;
        tv.appendAssistantMessage("model");
        tv.endStreaming();
        tv.endStreaming();
    }

    void testClearAllMessagesMultipleTimes()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        tv.clearAllMessages();
        tv.clearAllMessages();
        QVERIFY(tv.count() == 0);
    }

    void testSetCurrentThreadDifferentIds()
    {
        ThreadView tv;
        tv.setCurrentThread("thread-1");
        tv.setCurrentThread("thread-2");
        tv.setCurrentThread("thread-3");
        QVERIFY(tv.m_currentThreadId == "thread-3");
    }

    void testGetAllMessagesAfterClear()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.clearAllMessages();
        auto msgs = tv.getAllMessages();
        QVERIFY(msgs.isEmpty());
    }

    void testGetAllMessagesWithMultipleMessages()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        tv.appendUserMessage("model");
        auto msgs = tv.getAllMessages();
        QVERIFY(msgs.size() >= 3);
    }

    void testStreamingWithEmptyChunk()
    {
        ThreadView tv;
        tv.appendAssistantMessage("model");
        tv.showStreamingChunk("");
        tv.showStreamingChunk("text");
    }

    void testMultipleThreads()
    {
        ThreadView tv;
        tv.setCurrentThread("t1");
        tv.appendUserMessage("model");
        tv.setCurrentThread("t2");
        tv.appendUserMessage("model");
    }
};

QTEST_MAIN(TestThreadViewDetailed)
#include "test_threadview_detailed.moc"