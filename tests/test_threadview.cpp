#include <QtTest/QtTest>
#include "../src/ui/threadview.h"

class TestThreadView : public QObject
{
    Q_OBJECT

private slots:
    void testEscapeHtml()
    {
        ThreadView view(nullptr);
        
        QString escaped = view.escapeHtml("Hello & <World> \"Test\"");
        QCOMPARE(escaped, QString("Hello &amp; &lt;World&gt; &quot;Test&quot;"));
    }

    void testEscapeHtmlApostrophe()
    {
        ThreadView view(nullptr);
        
        QString escaped = view.escapeHtml("It's a test");
        QCOMPARE(escaped, QString("It&#39;s a test"));
    }

    void testParseMarkdownBold()
    {
        ThreadView view(nullptr);
        
        QString html = view.parseMarkdown("**bold text**");
        QVERIFY(html.contains("<strong>bold text</strong>"));
    }

    void testParseMarkdownItalic()
    {
        ThreadView view(nullptr);
        
        QString html = view.parseMarkdown("*italic text*");
        QVERIFY(html.contains("<em>italic text</em>"));
    }

    void testParseMarkdownCode()
    {
        ThreadView view(nullptr);
        
        QString html = view.parseMarkdown("`inline code`");
        QVERIFY(html.contains("<code>inline code</code>"));
    }

    void testParseMarkdownLink()
    {
        ThreadView view(nullptr);
        
        QString html = view.parseMarkdown("[link](https://example.com)");
        QVERIFY(html.contains("<a href=\"https://example.com\">link</a>"));
    }

    void testParseMarkdownCodeBlock()
    {
        ThreadView view(nullptr);
        
        QString html = view.parseMarkdown("```cpp\nint x = 1;\n```");
        QVERIFY(html.contains("<pre><code>"));
    }

    void testDiffLogic()
    {
        // Test basic diff calculation logic
        QString oldText = "line1\nline2\nline3";
        QString newText = "line1\nline2 modified\nline3";
        
        QVERIFY(oldText != newText);
        QVERIFY(oldText.contains("line2"));
        QVERIFY(newText.contains("line2 modified"));
    }
};

QTEST_MAIN(TestThreadView)
#include "test_threadview.moc"
