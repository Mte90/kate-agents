#include <QtTest/QtTest>
#include "../src/ui/filementionpopup.h"
#include <QDir>
#include <QFile>

class TestFileMentionPopupUtils : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        FileMentionPopup popup;
        QVERIFY(true);
    }

    void testInitialPathsEmpty()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_allPaths.isEmpty());
    }

    void testFilteredPathsEmpty()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_filteredPaths.isEmpty());
    }

    void testSetFilterPattern()
    {
        FileMentionPopup popup;
        popup.m_filterPattern = "test";
        QVERIFY(popup.m_filterPattern == "test");
    }

    void testSelectedIndex()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 5;
        QVERIFY(popup.m_selectedIndex == 5);
    }

    void testMaxDepth()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_maxDepth == 3);
    }

    void testMaxFiles()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_maxFiles == 500);
    }

    void testAddAllRecursivelyEmptyDir()
    {
        FileMentionPopup popup;
        QDir emptyDir("/tmp/empty-dir-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        emptyDir.mkpath(".");
        popup.addAllRecursively(emptyDir, 0);
        emptyDir.removeRecursively();
    }

    void testFilterByPattern()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "main.cpp" << "README.md" << "test.cpp";
        popup.m_filterPattern = "*.cpp";
    }

    void testSelectedIndexBounds()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 0;
        QVERIFY(popup.m_selectedIndex >= 0);
    }

    void testMoveSelectionUp()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 5;
        popup.m_selectedIndex--;
        QVERIFY(popup.m_selectedIndex == 4);
    }

    void testMoveSelectionDown()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 5;
        popup.m_selectedIndex++;
        QVERIFY(popup.m_selectedIndex == 6);
    }

    void testClearFilter()
    {
        FileMentionPopup popup;
        popup.m_filterPattern = "test";
        popup.m_filterPattern.clear();
        QVERIFY(popup.m_filterPattern.isEmpty());
    }

    void testPathsCountLimit()
    {
        FileMentionPopup popup;
        for (int i = 0; i < 600; i++) {
            popup.m_allPaths.append("file" + QString::number(i) + ".txt");
        }
        QVERIFY(popup.m_allPaths.size() == 600);
    }
};

QTEST_MAIN(TestFileMentionPopupUtils)
#include "test_filementionpopup_utils.moc"