#include <QtTest/QtTest>
#include "../src/editorcontext.h"

class TestEditorContextAdvanced : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultConstruction()
    {
        EditorContext ctx;
        QVERIFY(ctx.isEmpty());
    }

    void testIsEmptyAfterConstruction()
    {
        EditorContext ctx;
        QVERIFY(ctx.isEmpty());
    }

    void testTruncateThroughGetBufferContext()
    {
        EditorContext ctx;
        QStringList lines;
        for (int i = 0; i < 200; ++i) {
            lines << QString(50, QChar('x'));
        }
        QString content = lines.join("\n");
        QString result = ctx.getBufferContext(content, 100, 100);
        if (result.contains("...")) {
            QVERIFY(result.length() <= 103);
        }
    }

    void testTruncationViaToSystemPromptChunk()
    {
        EditorContext ctx;
        QVERIFY(ctx.toSystemPromptChunk().isEmpty());
    }

    void testGetBufferContextEmptyContent()
    {
        EditorContext ctx;
        QString result = ctx.getBufferContext("", 5, 2000);
        QVERIFY(result.isEmpty());
    }

    void testGetBufferContextSingleLine()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Hello World");
        QString result = ctx.getBufferContext(content, 0, 2000);
        QVERIFY(!result.isEmpty());
        QVERIFY(result.contains("Hello World"));
    }

    void testGetBufferContextCursorOutOfRange()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Line 1\nLine 2\nLine 3");
        QString result = ctx.getBufferContext(content, 100, 2000);
        QVERIFY(!result.isEmpty());
    }

    void testGetBufferContextMaxCharsLimit()
    {
        EditorContext ctx;
        QString content;
        for (int i = 0; i < 50; ++i) {
            content += QString(100, QChar('a' + (i % 26))) + "\n";
        }
        QString result = ctx.getBufferContext(content, 25, 200);
        QVERIFY(result.length() <= 203);
    }

    void testGetBufferContextNegativeCursorLine()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Line 1\nLine 2\nLine 3");
        QString result = ctx.getBufferContext(content, -1, 2000);
        QVERIFY(!result.isEmpty());
    }

    void testGetBufferContextMarksCursorLine()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Line 0\nLine 1\nLine 2\nLine 3\nLine 4");
        QString result = ctx.getBufferContext(content, 2, 2000);
        QVERIFY(result.contains(">>>"));
        QVERIFY(result.contains("Line 2"));
    }

    void testGetBufferContextMultiLine()
    {
        EditorContext ctx;
        QStringList lines;
        for (int i = 0; i < 20; ++i) {
            lines << QString("Line %1").arg(i);
        }
        QString content = lines.join("\n");
        QString result = ctx.getBufferContext(content, 10, 2000);
        QVERIFY(result.contains("Line 10"));
    }

    void testGetBufferContextFiveLineWindow()
    {
        EditorContext ctx;
        QStringList lines;
        for (int i = 0; i < 30; ++i) {
            lines << QString("Line %1").arg(i);
        }
        QString content = lines.join("\n");
        QString result = ctx.getBufferContext(content, 15, 5000);
        QVERIFY(result.contains("Line 15"));
        QVERIFY(result.contains(">>>"));
    }

    void testGetBufferContextAroundCursorEdgeCases()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Line 0\nLine 1\nLine 2");
        QString result = ctx.getBufferContextAroundCursor(content, 0, 0, 0);
        QVERIFY(result.contains("Line 0"));
        QVERIFY(result.contains(">>>"));
    }

    void testGetBufferContextAroundCursorMultiLine()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Alpha\nBeta\nGamma\nDelta\nEpsilon");
        QString result = ctx.getBufferContextAroundCursor(content, 2, 2, 1);
        QVERIFY(result.contains("<<<CURSOR>>>"));
        QVERIFY(result.contains("Line 3"));
        QVERIFY(result.contains("Line 2"));
        QVERIFY(result.contains("Line 4"));
    }

    void testGetBufferContextAroundCursorEmptyContent()
    {
        EditorContext ctx;
        QString result = ctx.getBufferContextAroundCursor("", 0, 0, 5);
        QVERIFY(result.isEmpty());
    }

    void testGetBufferContextAroundCursorColOutOfBounds()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Hi\nWorld");
        QString result = ctx.getBufferContextAroundCursor(content, 0, 100, 1);
        QVERIFY(!result.isEmpty());
    }

    void testGetBufferContextAroundCursorNegativeCol()
    {
        EditorContext ctx;
        QString content = QStringLiteral("Hello\nWorld");
        QString result = ctx.getBufferContextAroundCursor(content, 0, -1, 1);
        QVERIFY(!result.isEmpty());
    }

    void testGetBufferContextAroundCursorCursorMarkerPosition()
    {
        EditorContext ctx;
        QString content = QStringLiteral("abcdefgh");
        QString result = ctx.getBufferContextAroundCursor(content, 0, 3, 0);
        QVERIFY(result.contains("<<<CURSOR>>>"));
        QVERIFY(result.contains("Line 1"));
    }

    void testGetBufferContextAroundCursorLineNumbers()
    {
        EditorContext ctx;
        QString content = QStringLiteral("A\nB\nC\nD\nE");
        QString result = ctx.getBufferContextAroundCursor(content, 2, 0, 2);
        QVERIFY(result.contains("Line 1"));
        QVERIFY(result.contains("Line 3"));
        QVERIFY(result.contains("Line 5"));
    }

    void testGetBufferContextSingleCharContent()
    {
        EditorContext ctx;
        QString result = ctx.getBufferContext("X", 0, 100);
        QVERIFY(!result.isEmpty());
        QVERIFY(result.contains("X"));
    }

    void testGetBufferContextNewlinesOnly()
    {
        EditorContext ctx;
        QString content = QStringLiteral("\n\n\n\n\n");
        QString result = ctx.getBufferContext(content, 2, 100);
        QVERIFY(!result.isEmpty());
    }

    void testGetBufferContextVerySmallMaxChars()
    {
        EditorContext ctx;
        QString content = QStringLiteral("abcdefghijklmnop");
        QString result = ctx.getBufferContext(content, 0, 5);
        QVERIFY(!result.isEmpty());
    }

    void testGetBufferContextAroundCursorSingleLine()
    {
        EditorContext ctx;
        QString content = QStringLiteral("only one line");
        QString result = ctx.getBufferContextAroundCursor(content, 0, 0, 5);
        QVERIFY(result.contains("only one line"));
        QVERIFY(result.contains(">>>"));
    }

    void testToSystemPromptChunkEmpty()
    {
        EditorContext ctx;
        QVERIFY(ctx.toSystemPromptChunk().isEmpty());
    }
};

QTEST_MAIN(TestEditorContextAdvanced)
#include "test_editorcontext_advanced.moc"
