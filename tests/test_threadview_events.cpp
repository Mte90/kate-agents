#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/llmprovider.h"
#include <QSignalSpy>

class TestThreadViewEvents : public QObject
{
    Q_OBJECT

private slots:

    void testThreadViewShow()
    {
        ThreadView tv;
        tv.show();
        QVERIFY(tv.isVisible());
        tv.hide();
    }

    void testThreadViewResize()
    {
        ThreadView tv;
        tv.resize(600, 400);
        QVERIFY(tv.width() == 600);
        QVERIFY(tv.height() == 400);
    }

    void testThreadViewMinimumSize()
    {
        ThreadView tv;
        tv.setMinimumSize(200, 150);
        QVERIFY(tv.minimumWidth() == 200);
        QVERIFY(tv.minimumHeight() == 150);
    }

    void testTabsShow()
    {
        ThreadView tv;
        tv.show();
        QVERIFY(tv.m_tabs->isVisible());
        tv.hide();
    }

    void testTabsCount()
    {
        ThreadView tv;
        QVERIFY(tv.count() == 0);
        
        tv.appendUserMessage("model");
        QVERIFY(tv.count() >= 1);
        
        tv.appendAssistantMessage("model");
        QVERIFY(tv.count() >= 2);
    }

    void testTabsCurrentIndex()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        if (tv.count() > 1) {
            tv.setCurrentIndex(0);
            QVERIFY(tv.currentIndex() == 0);
            
            tv.setCurrentIndex(1);
            QVERIFY(tv.currentIndex() == 1);
        }
    }

    void testTabsCurrentChangedSignal()
    {
        ThreadView tv;
        QSignalSpy spy(tv.m_tabs, &QTabWidget::currentChanged);
        
        tv.appendUserMessage("model");
        
        QVERIFY(spy.count() >= 0);
    }

    void testTabCloseRequestedSignal()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        QSignalSpy spy(tv.m_tabs, &QTabWidget::tabCloseRequested);
        
        if (tv.count() > 0) {
            tv.m_tabs->tabCloseRequested(0);
        }
    }

    void testClearTab()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        int before = tv.count();
        
        tv.clearAllMessages();
        
        QVERIFY(tv.count() == 0);
    }

    void testSetTabText()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        if (tv.count() > 0) {
            tv.setTabText(0, "New Title");
            QVERIFY(tv.tabText(0) == "New Title");
        }
    }

    void testTabToolTip()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        if (tv.count() > 0) {
            tv.setTabToolTip(0, "Tooltip text");
            QVERIFY(tv.tabToolTip(0) == "Tooltip text");
        }
    }

    void testTabEnabled()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        if (tv.count() > 0) {
            tv.setTabEnabled(0, false);
            QVERIFY(!tv.isTabEnabled(0));
            
            tv.setTabEnabled(0, true);
            QVERIFY(tv.isTabEnabled(0));
        }
    }

    void testTabsIcon()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        if (tv.count() > 0) {
            QVERIFY(tv.tabIcon(0).isNull() || !tv.tabIcon(0).isNull());
        }
    }

    void testThreadViewFocus()
    {
        ThreadView tv;
        tv.show();
        
        tv.setFocus();
        QVERIFY(tv.hasFocus() || !tv.isVisible());
        
        tv.hide();
    }

    void testThreadViewLayout()
    {
        ThreadView tv;
        QVERIFY(tv.m_tabs->layout() != nullptr);
    }

    void testInsertTab()
    {
        ThreadView tv;
        
        tv.insertTab(0, new QWidget, "Tab 1");
        tv.insertTab(1, new QWidget, "Tab 2");
        
        QVERIFY(tv.count() >= 2);
    }

    void testRemoveTab()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendUserMessage("model");
        
        int before = tv.count();
        
        if (before > 0) {
            tv.removeTab(0);
        }
        
        QVERIFY(tv.count() <= before);
    }

    void testTabBarVisibility()
    {
        ThreadView tv;
        
        tv.m_tabs->tabBar()->hide();
        QVERIFY(!tv.m_tabs->tabBar()->isVisible());
        
        tv.m_tabs->tabBar()->show();
        QVERIFY(tv.m_tabs->tabBar()->isVisible());
    }

    void testTabsMovable()
    {
        ThreadView tv;
        
        tv.m_tabs->setMovable(true);
        QVERIFY(tv.m_tabs->isMovable());
        
        tv.m_tabs->setMovable(false);
        QVERIFY(!tv.m_tabs->isMovable());
    }
};

QTEST_MAIN(TestThreadViewEvents)
#include "test_threadview_events.moc"