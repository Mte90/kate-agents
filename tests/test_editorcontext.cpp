#include <QtTest/QtTest>
#include "../src/editorcontext.h"

class TestEditorContext : public QObject
{
    Q_OBJECT

private slots:
    void testGetBufferContextEmpty()
    {
        EditorContext context;
        
        // Test with empty content
        QString result = context.getBufferContext("", 5, 2000);
        QVERIFY(result.isEmpty());
    }

    void testGetBufferContextShort()
    {
        EditorContext context;
        
        // Test with short content (less than limit)
        QString shortContent = "Line 1\nLine 2\nLine 3";
        QString result = context.getBufferContext(shortContent, 1, 2000);
        QVERIFY(!result.isEmpty());
        QVERIFY(result.contains("Line 2"));
    }

    void testGetBufferContextLong()
    {
        EditorContext context;
        
        // Test with long content (exceeds limit)
        QString longContent;
        for (int i = 0; i < 100; ++i) {
            longContent += QString("Line %1\n").arg(i);
        }
        QString result = context.getBufferContext(longContent, 50, 2000);
        QVERIFY(!result.isEmpty());
        QVERIFY(result.contains("Line 50"));
    }

    void testGetBufferContextAroundCursor()
    {
        EditorContext context;
        
        QString content = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5";
        QString result = context.getBufferContextAroundCursor(content, 2, 1, 5);
        QVERIFY(!result.isEmpty());
        QVERIFY(result.contains("Line 2"));
        QVERIFY(result.contains("<<<CURSOR>>>"));
    }

    void testIsEmpty()
    {
        EditorContext context;
        QVERIFY(context.isEmpty());
    }
};

QTEST_MAIN(TestEditorContext)
#include "test_editorcontext.moc"
