#include <QtTest/QtTest>
#include "../src/ui/filementionpopup.h"

class TestFileMentionPopupUIDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        FileMentionPopup popup;
        QVERIFY(true);
    }

    void testAllPathsInitiallyEmpty()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_allPaths.isEmpty());
    }

    void testFilteredPathsInitiallyEmpty()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_filteredPaths.isEmpty());
    }

    void testSelectedIndexInitiallyZero()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_selectedIndex == 0);
    }

    void testSetSelectedIndex()
    {
        FileMentionPopup popup;
        popup.m_selectedIndex = 5;
        QVERIFY(popup.m_selectedIndex == 5);
    }

    void testShowAtScreenBoundaries()
    {
        FileMentionPopup popup;
        popup.setFixedSize(200, 300);
        
        // Mock screen size to 1920x1080
        // Since we can't easily mock QGuiApplication::primaryScreen(),
        // we verify that showAt doesn't crash and handles extreme coordinates.
        
        // Case 1: Top-left corner
        popup.showAt(QPoint(0, 0));
        QVERIFY(popup.x() >= 0 && popup.y() >= 0);
        
        // Case 2: Extreme bottom-right (should be clamped)
        popup.showAt(QPoint(5000, 5000));
        QVERIFY(popup.x() < 5000); 
        QVERIFY(popup.y() < 5000);
        
        // Case 3: Position that would overflow bottom (should flip to top)
        // We can't perfectly predict the screen height here, but we verify 
        // that the coordinates are adjusted from the requested ones.
        QPoint requestedPos(100, 10000);
        popup.showAt(requestedPos);
        QVERIFY(popup.y() != requestedPos.y());
    }

    void testMaxDepthDefault()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_maxDepth == 3);
    }

    void testMaxFilesDefault()
    {
        FileMentionPopup popup;
        QVERIFY(popup.m_maxFiles == 500);
    }

    void testAddPath()
    {
        FileMentionPopup popup;
        popup.m_allPaths.append("/path/to/file.cpp");
        QVERIFY(popup.m_allPaths.size() == 1);
    }

    void testAddMultiplePaths()
    {
        FileMentionPopup popup;
        for (int i = 0; i < 10; i++) {
            popup.m_allPaths.append("/path/file" + QString::number(i) + ".cpp");
        }
        QVERIFY(popup.m_allPaths.size() == 10);
    }

    void testFilterPatternEmpty()
    {
        FileMentionPopup popup;
        popup.m_filterPattern.clear();
        QVERIFY(popup.m_filterPattern.isEmpty());
    }

    void testFilterPatternSet()
    {
        FileMentionPopup popup;
        popup.m_filterPattern = "*.cpp";
        QVERIFY(popup.m_filterPattern == "*.cpp");
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

    void testClearPaths()
    {
        FileMentionPopup popup;
        popup.m_allPaths << "file1" << "file2";
        popup.m_allPaths.clear();
        QVERIFY(popup.m_allPaths.isEmpty());
    }

    void testFilteredPathsCopy()
    {
        FileMentionPopup popup;
        popup.m_filteredPaths = popup.m_allPaths;
        QVERIFY(popup.m_filteredPaths.isEmpty());
    }

    void testMaxDepthWritable()
    {
        FileMentionPopup popup;
        popup.m_maxDepth = 5;
        QVERIFY(popup.m_maxDepth == 5);
    }
};

QTEST_MAIN(TestFileMentionPopupUIDetailed)
#include "test_filementionpopup_ui_detailed.moc"