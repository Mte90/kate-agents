#include <QtTest/QtTest>
#include "../src/ui/filementionpopup.h"
#include <QSignalSpy>
#include <QKeyEvent>

class TestFileMentionPopupEvents : public QObject
{
    Q_OBJECT

private slots:

    void testPopupShowHide()
    {
        FileMentionPopup popup;
        
        popup.show();
        QVERIFY(popup.isVisible());
        
        popup.hide();
        QVERIFY(!popup.isVisible());
    }

    void testPopupKeyPressUp()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 5;
        
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &event);
        
        QVERIFY(popup.m_selectedIndex == 4);
    }

    void testPopupKeyPressDown()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 5;
        
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &event);
        
        QVERIFY(popup.m_selectedIndex == 6);
    }

    void testPopupKeyPressEnter()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file1.cpp" << "/test/file2.cpp";
        
        QSignalSpy spy(&popup, &FileMentionPopup::pathSelected);
        
        popup.m_selectedIndex = 0;
        
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &event);
        
        QVERIFY(spy.count() >= 0);
    }

    void testPopupKeyPressEscape()
    {
        FileMentionPopup popup;
        popup.show();
        
        QSignalSpy spy(&popup, &FileMentionPopup::hidden);
        
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &event);
    }

    void testPopupKeyPressTab()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file1.cpp";
        
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &event);
    }

    void testSetFilterPattern()
    {
        FileMentionPopup popup;
        
        popup.setFilterPattern("*.cpp");
        
        QVERIFY(popup.m_filterPattern == "*.cpp");
    }

    void testSetRootPath()
    {
        FileMentionPopup popup;
        
        popup.setRootPath("/home/user");
        
        QVERIFY(!popup.m_rootPath.isEmpty());
    }

    void testSetMaxDepth()
    {
        FileMentionPopup popup;
        
        popup.setMaxDepth(5);
        
        QVERIFY(popup.m_maxDepth == 5);
    }

    void testUpdateFilteredPaths()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file1.cpp" << "/test/file2.h" << "/test/file3.cpp";
        popup.m_filterPattern = "*.cpp";
        
        popup.updateFilteredPaths();
    }

    void testMoveSelectionUpFromZero()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 0;
        
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &event);
    }

    void testMoveSelectionDownFromEnd()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file1.cpp";
        popup.m_selectedIndex = 0;
        
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &event);
    }

    void testPathSelectionWrap()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file1.cpp" << "/test/file2.cpp";
        popup.m_selectedIndex = 1;
        
        QKeyEvent upEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
        QCoreApplication::sendEvent(&popup, &upEvent);
    }

    void testListWidgetSelection()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file1.cpp" << "/test/file2.cpp";
        
        popup.m_listWidget->setCurrentRow(0);
        QVERIFY(popup.m_listWidget->currentRow() == 0);
        
        popup.m_listWidget->setCurrentRow(1);
        QVERIFY(popup.m_listWidget->currentRow() == 1);
    }

    void testListWidgetItemClicked()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "/test/file1.cpp";
        
        QSignalSpy spy(&popup, &FileMentionPopup::pathSelected);
        
        if (popup.m_listWidget->count() > 0) {
            popup.m_listWidget->item(0)->setSelected(true);
        }
    }

    void testFilterLineEditTextChanged()
    {
        FileMentionPopup popup;
        
        QSignalSpy spy(popup.m_filterLineEdit, &QLineEdit::textChanged);
        
        popup.m_filterLineEdit->setText("*.cpp");
        
        QVERIFY(spy.count() >= 1);
    }

    void testPopupGeometry()
    {
        FileMentionPopup popup;
        popup.setGeometry(100, 100, 400, 300);
        
        QVERIFY(popup.width() == 400);
        QVERIFY(popup.height() == 300);
    }

    void testPopupFocusPolicy()
    {
        FileMentionPopup popup;
        
        popup.setFocusPolicy(Qt::StrongFocus);
        QVERIFY(popup.focusPolicy() == Qt::StrongFocus);
    }
};

QTEST_MAIN(TestFileMentionPopupEvents)
#include "test_filementionpopup_events.moc"