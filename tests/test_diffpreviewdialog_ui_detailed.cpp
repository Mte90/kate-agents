#include <QtTest/QtTest>
#include "../src/ui/diffpreviewdialog.h"

class TestDiffPreviewDialogUIDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        DiffPreviewDialog dialog("old", "new");
        QVERIFY(true);
    }

    void testOriginalContent()
    {
        DiffPreviewDialog dialog("old content", "new content");
        QVERIFY(dialog.m_originalContent == "old content");
    }

    void testModifiedContent()
    {
        DiffPreviewDialog dialog("old content", "new content");
        QVERIFY(dialog.m_modifiedContent == "new content");
    }

    void testOriginalTextEdit()
    {
        DiffPreviewDialog dialog("old", "new");
        QVERIFY(dialog.m_originalEdit != nullptr);
    }

    void testModifiedTextEdit()
    {
        DiffPreviewDialog dialog("old", "new");
        QVERIFY(dialog.m_modifiedEdit != nullptr);
    }

    void testDiffTextEdit()
    {
        DiffPreviewDialog dialog("old", "new");
        QVERIFY(dialog.m_diffEdit != nullptr);
    }

    void testTabs()
    {
        DiffPreviewDialog dialog("old", "new");
        QVERIFY(dialog.m_tabs != nullptr);
    }

    void testEmptyOriginal()
    {
        DiffPreviewDialog dialog("", "new");
        QVERIFY(dialog.m_originalContent.isEmpty());
    }

    void testEmptyModified()
    {
        DiffPreviewDialog dialog("old", "");
        QVERIFY(dialog.m_modifiedContent.isEmpty());
    }

    void testBothEmpty()
    {
        DiffPreviewDialog dialog("", "");
        QVERIFY(dialog.m_originalContent.isEmpty());
        QVERIFY(dialog.m_modifiedContent.isEmpty());
    }

    void testLongContent()
    {
        QString longStr(10000, 'x');
        DiffPreviewDialog dialog(longStr, longStr);
        QVERIFY(dialog.m_originalContent.length() == 10000);
    }

    void testUnicodeContent()
    {
        DiffPreviewDialog dialog("Ciao 世界 🌍", "Hello 世界 🌍");
        QVERIFY(dialog.m_originalContent.contains("Ciao"));
        QVERIFY(dialog.m_modifiedContent.contains("Hello"));
    }

    void testNewlinesContent()
    {
        DiffPreviewDialog dialog("line1\nline2\nline3", "line1\nline2modified\nline3");
        QVERIFY(dialog.m_originalContent.contains('\n'));
    }

    void testTabsCount()
    {
        DiffPreviewDialog dialog("old", "new");
        QVERIFY(dialog.m_tabs->count() >= 2);
    }
};

QTEST_MAIN(TestDiffPreviewDialogUIDetailed)
#include "test_diffpreviewdialog_ui_detailed.moc"