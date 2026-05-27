#include <QTest>
#include <QApplication>
#include <QTextDocument>
#include <QDebug>

#include "ui/threadview.h"

class TestThreadView : public QObject
{
    Q_OBJECT

private slots:
    void testStreamingAndMarkdown();
    void testAppendUserMessage();
    void testAppendAssistantMessage();
    void testAppendAssistantMessageWithThinking();
    void testMessageOrderWithHR();
    void testStreamingChunkCursorPosition();
    void testHrSeparatorColor();
    void testCodeBlockCopyButton();
};

void TestThreadView::testStreamingAndMarkdown()
{
    ThreadView view;
    view.show();

    view.showStreamingChunk("**bold** text");
    view.showStreamingChunk(" and `code`");
    view.endStreaming();

    QString html = view.toHtml();

    QVERIFY2(html.contains("<strong>bold</strong>"),
             "Markdown bold should be rendered as <strong>");
    QVERIFY2(html.contains("<code>code</code>"),
             "Markdown inline code should be rendered as <code>");
    QVERIFY2(!html.contains("**bold**"),
             "Raw markdown syntax **bold** should not remain");
    QVERIFY2(!html.contains("`code`"),
             "Raw markdown syntax `code` should not remain");

    int openDivCount = html.count("<div ");
    int closeDivCount = html.count("</div>");
    QCOMPARE(openDivCount, closeDivCount);

    QVERIFY2(html.contains("white-space: pre-wrap"),
             "Streaming div should preserve white-space: pre-wrap");
}

void TestThreadView::testAppendUserMessage()
{
    ThreadView view;
    view.show();

    view.appendUserMessage("Hello!", "write");

    QString html = view.toHtml();

    QVERIFY2(html.contains("write"),
             "Profile label 'write' should appear in user message");
    QVERIFY2(html.contains("Hello!"),
             "User message content should appear");
    QVERIFY2(html.contains("class=\"user-message\""),
             "User message should have user-message class");
}

void TestThreadView::testAppendAssistantMessage()
{
    ThreadView view;
    view.show();

    view.appendAssistantMessage("Hi there!");

    QString html = view.toHtml();

    QVERIFY2(html.contains("<hr"),
             "Assistant message should be preceded by <hr>");
    QVERIFY2(html.contains("Hi there!"),
             "Assistant message content should appear");
    QVERIFY2(html.contains("class=\"assistant-message\""),
             "Assistant message should have assistant-message class");
}

void TestThreadView::testAppendAssistantMessageWithThinking()
{
    ThreadView view;
    view.show();

    view.appendAssistantMessage("Final answer", "Step-by-step reasoning");

    QString html = view.toHtml();

    QVERIFY2(html.contains("<details"),
             "Thinking should render inside <details>");
    QVERIFY2(html.contains("🤔 Thinking"),
             "Thinking summary should show 🤔 Thinking");
    QVERIFY2(html.contains("Step-by-step reasoning"),
             "Thinking content should appear");
    QVERIFY2(html.contains("Final answer"),
             "Assistant message content should appear after thinking");
}

void TestThreadView::testMessageOrderWithHR()
{
    ThreadView view;
    view.show();

    view.appendUserMessage("User msg 1");
    view.appendAssistantMessage("Assistant msg 1");
    view.appendUserMessage("User msg 2");
    view.appendAssistantMessage("Assistant msg 2");

    QString html = view.toHtml();

    int hrCount = html.count("<hr");
    QVERIFY2(hrCount == 2,
             qPrintable(QString("Should have 2 <hr> separators for 2 assistant messages, got %1").arg(hrCount)));

    int firstUser = html.indexOf("User msg 1");
    int firstHR = html.indexOf("<hr");
    int firstAssistant = html.indexOf("Assistant msg 1");
    int secondUser = html.indexOf("User msg 2");
    int secondAssistant = html.indexOf("Assistant msg 2");

    QVERIFY2(firstUser < firstHR,
             "User message should appear before first <hr>");
    QVERIFY2(firstHR < firstAssistant,
             "First <hr> should appear before first assistant message");
    QVERIFY2(firstAssistant < secondUser,
             "First assistant should appear before second user message");
    QVERIFY2(secondUser < secondAssistant,
             "Second user should appear before second assistant message");
}

void TestThreadView::testStreamingChunkCursorPosition()
{
    ThreadView view;
    view.show();

    view.showStreamingChunk("Line 1\nLine 2");
    view.endStreaming();

    QString html = view.toHtml();

    int openDiv = html.indexOf("<div");
    int closeDiv = html.indexOf("</div>");
    QVERIFY2(openDiv < closeDiv, "Opening <div> should come before closing </div>");

    int line1Pos = html.indexOf("Line 1");
    int line2Pos = html.indexOf("Line 2");
    QVERIFY2(line1Pos > openDiv && line1Pos < closeDiv,
             "Line 1 should be inside <div> element");
    QVERIFY2(line2Pos > openDiv && line2Pos < closeDiv,
             "Line 2 should be inside <div> element");
}

void TestThreadView::testHrSeparatorColor()
{
    ThreadView view;
    view.show();

    view.appendUserMessage("User");
    view.appendAssistantMessage("Assistant");

    QString html = view.toHtml();

    QVERIFY2(html.contains("border-top"), "HR should have border-top style");
    QVERIFY2(!html.contains("border: none; border-top: 1px solid black"),
             "HR should not have black border");
}

void TestThreadView::testCodeBlockCopyButton()
{
    ThreadView view;
    view.show();

    view.showStreamingChunk("```cpp\nint x = 1;\n```");
    view.endStreaming();

    QString html = view.toHtml();

    QVERIFY2(html.contains("copy-btn"), "Code block should have copy button class");
    QVERIFY2(html.contains("onclick"), "Copy button should have onclick handler");
}

QTEST_MAIN(TestThreadView)
#include "test_threadview.moc"
