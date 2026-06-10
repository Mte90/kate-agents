#include <QtTest/QtTest>
#include "../src/ui/diffpreviewdialog.h"

class TestDiffPreviewDialog : public QObject
{
    Q_OBJECT

private slots:

    void testDialogConstruction()
    {
        DiffPreviewDialog dialog("/test/file.cpp", "old", "new");
        QVERIFY(dialog.windowTitle().contains("file.cpp"));
    }

    void testDialogWithIdenticalContent()
    {
        DiffPreviewDialog dialog("/test/file.cpp", "line1\nline2", "line1\nline2");
        QVERIFY(dialog.windowTitle().contains("file.cpp"));
    }

    void testDialogWithDifferentContent()
    {
        DiffPreviewDialog dialog("/test/file.cpp", "original", "modified");
        QVERIFY(dialog.windowTitle().contains("file.cpp"));
    }

    void testDialogWithEmptyOriginal()
    {
        DiffPreviewDialog dialog("/test/file.cpp", "", "new content");
        QVERIFY(dialog.windowTitle().contains("file.cpp"));
    }

    void testDialogWithEmptyModified()
    {
        DiffPreviewDialog dialog("/test/file.cpp", "old content", "");
        QVERIFY(dialog.windowTitle().contains("file.cpp"));
    }

    void testDialogWithMultilineContent()
    {
        QString multiline = "line1\nline2\nline3\nline4\nline5";
        DiffPreviewDialog dialog("/test/file.cpp", multiline, multiline);
        QVERIFY(dialog.windowTitle().contains("file.cpp"));
    }
};

QTEST_MAIN(TestDiffPreviewDialog)
#include "test_diffpreviewdialog.moc"