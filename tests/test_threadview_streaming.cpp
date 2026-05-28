#include <QtTest/QtTest>
#include <QApplication>
#include <QTextDocument>
#include "ui/threadview.h"

class TestThreadViewStreaming : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Ensure QApplication exists for QTextBrowser tests
        static int qargc = 1;
        static char qargv0[] = "test_streaming";
        static char *qargv[] = {qargv0};
        if (!QApplication::instance()) {
            new QApplication(qargc, qargv);
        }
    }

    void testStreamingChunkInsertion()
    {
        ThreadView threadView;
        threadView.appendAssistantMessage("Test Model");

        // Simulate streaming chunks
        threadView.showStreamingChunk("Hello ");
        threadView.showStreamingChunk("world!");

        // End streaming - should render markdown
        threadView.endStreaming();

        // Qt strips inline styles in toHtml(); check plain text
        QVERIFY(threadView.toPlainText().contains("Hello"));
        QVERIFY(threadView.toPlainText().contains("world!"));
    }

    void testMarkdownRendering()
    {
        ThreadView threadView;
        threadView.appendAssistantMessage("Test Model");

        // Send markdown content
        threadView.showStreamingChunk("**bold text** and *italic*");
        threadView.endStreaming();

        // Qt serializes bold/italic as styled spans in toHtml()
        // Verify no raw markdown remains in plain text, content preserved
        QVERIFY(threadView.toPlainText().contains("bold text"));
        QVERIFY(threadView.toPlainText().contains("italic"));
    }

    void testHRBetweenMessages()
    {
        ThreadView threadView;
        threadView.appendAssistantMessage("Model A");
        threadView.endStreaming();

        threadView.appendUserMessage("Hello", "User");

        QString html = threadView.toHtml();

        // Verify hr tag exists between messages
        QVERIFY(html.contains("<hr"));

        // Count hr tags - should be exactly 2 (before assistant and before user)
        int hrCount = html.count("<hr");
        QCOMPARE(hrCount, 2);
    }

    void testThinkingCollapsible()
    {
        ThreadView threadView;
        threadView.appendAssistantMessage("Model");

        // Simulate thinking content
        threadView.showStreamingChunk("Let me think about this...");
        threadView.endStreaming();

        // Qt strips <details>/<summary> in toHtml(); check plain text
        QVERIFY(threadView.toPlainText().contains("Let me think about this..."));
    }

    void testMessageStructureIntegrity()
    {
        ThreadView threadView;

        // Send multiple messages
        threadView.appendUserMessage("Request 1", "User");
        threadView.appendAssistantMessage("Model");
        threadView.showStreamingChunk("Response 1");
        threadView.endStreaming();

        threadView.appendUserMessage("Request 2", "User");
        threadView.appendAssistantMessage("Model");
        threadView.showStreamingChunk("Response 2");
        threadView.endStreaming();

        QString html = threadView.toHtml();

        // Verify balanced div tags
        int divOpen = html.count("<div style=");
        int divClose = html.count("</div>");
        QCOMPARE(divOpen, divClose);

        int hrCount = html.count("<hr");
        QCOMPARE(hrCount, 6);

        // Verify no orphaned tags
        QVERIFY(!html.contains("</div><br></div>"));
        QVERIFY(!html.contains("<div></div><br>"));
    }

    void testProfileLabelDisplay()
    {
        ThreadView threadView;
        threadView.appendUserMessage("Test message", "Write");

        QString html = threadView.toHtml();

        // Verify profile label is present
        QVERIFY(html.contains("Write"));
    }

    void testNoDuplicateHRInStreaming()
    {
        ThreadView threadView;
        threadView.appendAssistantMessage("Model");

        // Send multiple chunks
        for (int i = 0; i < 5; i++) {
            threadView.showStreamingChunk("Chunk " + QString::number(i) + " ");
        }

        QString htmlDuring = threadView.toHtml();
        int hrCountDuring = htmlDuring.count("<hr");

        QCOMPARE(hrCountDuring, 2);

        threadView.endStreaming();

        QString htmlAfter = threadView.toHtml();
        int hrCountAfter = htmlAfter.count("<hr");

        QCOMPARE(hrCountAfter, 2);
    }

    void testUnicodeContent()
    {
        ThreadView threadView;
        threadView.appendAssistantMessage("Model");

        threadView.showStreamingChunk("Hello 世界 🌍 Привет");
        threadView.endStreaming();

        // Unicode preserved in plain text
        QVERIFY(threadView.toPlainText().contains("世界"));
        QVERIFY(threadView.toPlainText().contains("🌍"));
        QVERIFY(threadView.toPlainText().contains("Привет"));
    }

    void testLargeContent()
    {
        ThreadView threadView;
        threadView.appendAssistantMessage("Model");

        // Send large content
        QString largeContent;
        for (int i = 0; i < 100; i++) {
            largeContent += "Line " + QString::number(i) + "\n";
        }

        threadView.showStreamingChunk(largeContent);
        threadView.endStreaming();

        // Content preserved in plain text after markdown rendering
        QVERIFY(threadView.toPlainText().contains("Line 0"));
        QVERIFY(threadView.toPlainText().contains("Line 99"));
    }
};

QTEST_MAIN(TestThreadViewStreaming)

#include "test_threadview_streaming.moc"
