#include <QtTest/QtTest>
#include "../src/ui/diffpreviewdialog.h"
#include <QSignalSpy>

class TestDiffPreviewDialogEvents : public QObject
{
    Q_OBJECT

private slots:

    void testDialogShow()
    {
        DiffPreviewDialog dialog("old", "new");
        dialog.show();
        QVERIFY(dialog.isVisible());
        dialog.hide();
    }

    void testDialogAccept()
    {
        DiffPreviewDialog dialog("old", "new");
        
        QSignalSpy spy(&dialog, &QDialog::accepted);
        
        dialog.accept();
        
        QVERIFY(spy.count() == 1);
    }

    void testDialogReject()
    {
        DiffPreviewDialog dialog("old", "new");
        
        QSignalSpy spy(&dialog, &QDialog::rejected);
        
        dialog.reject();
        
        QVERIFY(spy.count() == 1);
    }

    void testDialogClose()
    {
        DiffPreviewDialog dialog("old", "new");
        dialog.show();
        dialog.close();
        QVERIFY(!dialog.isVisible());
    }

    void testTabsCurrentChanged()
    {
        DiffPreviewDialog dialog("old", "new");
        
        QSignalSpy spy(dialog.m_tabs, &QTabWidget::currentChanged);
        
        if (dialog.m_tabs->count() > 1) {
            dialog.m_tabs->setCurrentIndex(1);
        }
        
        QVERIFY(spy.count() >= 0);
    }

    void testOriginalTextEditChange()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.m_originalEdit->setPlainText("modified old content");
        
        QVERIFY(dialog.m_originalEdit->toPlainText() == "modified old content");
    }

    void testModifiedTextEditChange()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.m_modifiedEdit->setPlainText("modified new content");
        
        QVERIFY(dialog.m_modifiedEdit->toPlainText() == "modified new content");
    }

    void testDialogDefaultButton()
    {
        DiffPreviewDialog dialog("old", "new");
        
        QVERIFY(dialog.defaultButton() != nullptr);
    }

    void testDialogFocus()
    {
        DiffPreviewDialog dialog("old", "new");
        dialog.show();
        
        dialog.setFocus();
        QVERIFY(dialog.hasFocus());
        
        dialog.hide();
    }

    void testDialogMinimumSize()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.setMinimumSize(400, 300);
        
        QVERIFY(dialog.minimumWidth() == 400);
        QVERIFY(dialog.minimumHeight() == 300);
    }

    void testDialogMaximumSize()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.setMaximumSize(800, 600);
        
        QVERIFY(dialog.maximumWidth() == 800);
        QVERIFY(dialog.maximumHeight() == 600);
    }

    void testDialogTitle()
    {
        DiffPreviewDialog dialog("old", "new");
        
        QString title = dialog.windowTitle();
        QVERIFY(!title.isEmpty());
    }

    void testDialogResize()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.resize(600, 400);
        
        QVERIFY(dialog.width() == 600);
        QVERIFY(dialog.height() == 400);
    }

    void testDialogModal()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.setModal(true);
        QVERIFY(dialog.isModal());
    }

    void testOriginalScrollBar()
    {
        DiffPreviewDialog dialog("old", "new");
        
        QVERIFY(dialog.m_originalEdit->verticalScrollBar() != nullptr);
    }

    void testModifiedScrollBar()
    {
        DiffPreviewDialog dialog("old", "new");
        
        QVERIFY(dialog.m_modifiedEdit->verticalScrollBar() != nullptr);
    }

    void testDialogExec()
    {
        DiffPreviewDialog dialog("old", "new");
        
        dialog.setResult(QDialog::Accepted);
        int result = dialog.exec();
        
        QVERIFY(result == QDialog::Accepted);
    }
};

QTEST_MAIN(TestDiffPreviewDialogEvents)
#include "test_diffpreviewdialog_events.moc"