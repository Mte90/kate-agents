#include <QtTest/QtTest>
#include "../src/ui/syntaxhighlighter.h"

class TestSyntaxHighlighterEvents : public QObject
{
    Q_OBJECT

private slots:

    void testHighlightEmptyDocument()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("");
        highlighter.rehighlight();
    }

    void testHighlightSimpleText()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("simple text");
        highlighter.rehighlight();
    }

    void testHighlightKeywords()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("void int char return");
        highlighter.rehighlight();
    }

    void testHighlightStrings()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("\"string literal\"");
        highlighter.rehighlight();
    }

    void testHighlightComments()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("// comment line");
        highlighter.rehighlight();
    }

    void testHighlightMultiLineComments()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("/* multi line\ncomment */");
        highlighter.rehighlight();
    }

    void testHighlightNumbers()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("42 3.14 0xFF");
        highlighter.rehighlight();
    }

    void testHighlightFunctions()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("function call()");
        highlighter.rehighlight();
    }

    void testHighlightMixed()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("int main() {\nreturn 0;\n}");
        highlighter.rehighlight();
    }

    void testHighlightLargeDocument()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        QString large;
        for (int i = 0; i < 100; i++) {
            large += "int var" + QString::number(i) + " = " + QString::number(i) + ";\n";
        }
        edit.setPlainText(large);
        highlighter.rehighlight();
    }

    void testHighlightUnicode()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("// Ciao 世界\nint 日本語 = 42;");
        highlighter.rehighlight();
    }

    void testHighlightSpecialChars()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("a + b * c / d % e");
        highlighter.rehighlight();
    }

    void testHighlightBrackets()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("if (a) { b(); }");
        highlighter.rehighlight();
    }

    void testHighlightPreprocessor()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("#include <iostream>\n#define MAX 100");
        highlighter.rehighlight();
    }

    void testHighlightMultipleFormats()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("// comment\n\"string\"\n42\nfunction()");
        highlighter.rehighlight();
    }

    void testHighlighterSetDocument()
    {
        QTextEdit edit1, edit2;
        SyntaxHighlighter highlighter(&edit1);
        
        highlighter.setDocument(&edit2);
        QVERIFY(highlighter.document() == &edit2);
    }

    void testHighlighterDefaultFormat()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        QVERIFY(highlighter.defaultFormat().isValid());
    }

    void testHighlighterFormats()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        QVERIFY(highlighter.m_highlightingRules.size() > 0);
        QVERIFY(highlighter.m_keywordPatterns.size() > 0);
    }

    void testHighlighterRehighlightBlock()
    {
        QTextEdit edit;
        SyntaxHighlighter highlighter(&edit);
        
        edit.setPlainText("test");
        
        QTextBlock block = edit.document()->findBlock(0);
        highlighter.rehighlightBlock(block);
    }
};

QTEST_MAIN(TestSyntaxHighlighterEvents)
#include "test_syntaxhighlighter_events.moc"