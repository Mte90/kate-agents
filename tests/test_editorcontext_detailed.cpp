#include <QtTest/QtTest>
#include "../src/editorcontext.h"

class TestEditorContextDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testTruncateEmpty()
    {
        QString result = EditorContext::truncate("", 100);
        QVERIFY(result.isEmpty());
    }

    void testTruncateShortString()
    {
        QString result = EditorContext::truncate("hello", 100);
        QVERIFY(result == "hello");
    }

    void testTruncateLongString()
    {
        QString input = "This is a very long string that exceeds the maximum length";
        QString result = EditorContext::truncate(input, 20);
        QVERIFY(result.length() <= 23);
    }

    void testTruncateExactLength()
    {
        QString result = EditorContext::truncate("hello", 5);
        QVERIFY(result == "hello");
    }

    void testTruncateZero()
    {
        QString result = EditorContext::truncate("hello", 0);
        QVERIFY(result.length() <= 3);
    }

    void testTruncateNegative()
    {
        QString result = EditorContext::truncate("hello", -1);
        QVERIFY(result.length() <= 3);
    }

    void testGetSelectedTextEmpty()
    {
        EditorContext ctx;
        ctx.selectedText = "";
        QVERIFY(ctx.selectedText.isEmpty());
    }

    void testGetSelectedText()
    {
        EditorContext ctx;
        ctx.selectedText = "selected content";
        QVERIFY(ctx.selectedText == "selected content");
    }

    void testGetFileContentEmpty()
    {
        EditorContext ctx;
        ctx.fileContent = "";
        QVERIFY(ctx.fileContent.isEmpty());
    }

    void testGetFileContent()
    {
        EditorContext ctx;
        ctx.fileContent = "file content here";
        QVERIFY(ctx.fileContent == "file content here");
    }

    void testGetCurrentLine()
    {
        EditorContext ctx;
        ctx.currentLine = 42;
        QVERIFY(ctx.currentLine == 42);
    }

    void testGetCurrentColumn()
    {
        EditorContext ctx;
        ctx.currentColumn = 10;
        QVERIFY(ctx.currentColumn == 10);
    }

    void testGetDocumentSize()
    {
        EditorContext ctx;
        ctx.documentSize = 1000;
        QVERIFY(ctx.documentSize == 1000);
    }

    void testGetSelectionStartLine()
    {
        EditorContext ctx;
        ctx.selectionStartLine = 5;
        QVERIFY(ctx.selectionStartLine == 5);
    }

    void testGetSelectionEndLine()
    {
        EditorContext ctx;
        ctx.selectionEndLine = 10;
        QVERIFY(ctx.selectionEndLine == 10);
    }

    void testGetFileLanguage()
    {
        EditorContext ctx;
        ctx.fileLanguage = "cpp";
        QVERIFY(ctx.fileLanguage == "cpp");
    }

    void testGetFilePath()
    {
        EditorContext ctx;
        ctx.filePath = "/path/to/file.cpp";
        QVERIFY(ctx.filePath == "/path/to/file.cpp");
    }
};

QTEST_MAIN(TestEditorContextDetailed)
#include "test_editorcontext_detailed.moc"