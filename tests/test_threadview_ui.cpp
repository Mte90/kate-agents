#include <QtTest/QtTest>
#include <QApplication>
#include <QTextDocument>
#include "ui/threadview.h"

class TestThreadViewUI : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
    }

    void testAppendAssistantMessageHasHR()
    {
        ThreadView threadView;
        threadView.show();

        threadView.appendAssistantMessage("Hello");
        QString html = threadView.toHtml();

        QVERIFY(html.contains("<hr"));
        QVERIFY(html.contains("Hello"));
    }

    void testAppendUserMessageHasHR()
    {
        ThreadView threadView;
        threadView.show();

        threadView.appendUserMessage("Hi", "Write");
        QString html = threadView.toHtml();

        QVERIFY(html.contains("<hr"));
        QVERIFY(html.contains("Hi"));
    }

    void testHROnlyBetweenMessages()
    {
        ThreadView threadView;
        threadView.show();

        threadView.appendAssistantMessage("First");
        threadView.appendAssistantMessage("Second");
        QString html = threadView.toHtml();

        int hrCount = 0;
        int pos = 0;
        while ((pos = html.indexOf("<hr", pos)) != -1) {
            hrCount++;
            pos++;
        }

        QCOMPARE(hrCount, 2);
    }

    void testThinkingCollapsible()
    {
        ThreadView threadView;
        threadView.show();

        QString thinking = "This is thinking content";
        QString response = "This is response";
        threadView.appendAssistantMessage(response, thinking);
        QString html = threadView.toHtml();

        QVERIFY(html.contains("<details>"));
        QVERIFY(html.contains("<summary>"));
        QVERIFY(html.contains("Thinking"));
    }

    void testStreamingChunkStructure()
    {
        ThreadView threadView;
        threadView.show();

        threadView.showStreamingChunk("Hello ");
        threadView.showStreamingChunk("World");
        threadView.endStreaming();

        QString html = threadView.toHtml();

        QVERIFY(html.contains("<div"));
        QVERIFY(html.contains("</div>"));
        QVERIFY(html.contains("Hello World"));
    }

    void testStreamingMarkdownRendering()
    {
        ThreadView threadView;
        threadView.show();

        threadView.showStreamingChunk("**bold** and *italic*");
        threadView.endStreaming();

        QString html = threadView.toHtml();

        QVERIFY(html.contains("<strong>bold</strong>") || html.contains("<b>bold</b>"));
        QVERIFY(html.contains("<em>italic</em>") || html.contains("<i>italic</i>"));
    }

    void testStreamingNoExtraHR()
    {
        ThreadView threadView;
        threadView.show();

        threadView.showStreamingChunk("Line 1");
        threadView.showStreamingChunk("Line 2");
        threadView.endStreaming();

        QString html = threadView.toHtml();

        int hrCount = 0;
        int pos = 0;
        while ((pos = html.indexOf("<hr", pos)) != -1) {
            hrCount++;
            pos++;
        }

        QCOMPARE(hrCount, 1);
    }

    void testProfileLabelInUserMessage()
    {
        ThreadView threadView;
        threadView.show();

        threadView.appendUserMessage("Test message", "Write");
        QString html = threadView.toHtml();

        QVERIFY(html.contains("Write"));
    }
};

QTEST_MAIN(TestThreadViewUI)

#include "test_threadview_ui.moc"
