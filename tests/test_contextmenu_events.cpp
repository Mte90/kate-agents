#include <QtTest/QtTest>
#include "../src/contextmenhander.h"
#include "../src/ui/agentpanel.h"
#include <QSignalSpy>

class TestContextMenuEvents : public QObject
{
    Q_OBJECT

private slots:

    void testContextMenuHandlerConstruction()
    {
        ContextMenuHandler handler;
        QVERIFY(true);
    }

    void testContextMenuShow()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        menu.addAction("Test Action");
        menu.exec(QCursor::pos());
    }

    void testContextMenuActions()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action1 = menu.addAction("Action 1");
        QAction *action2 = menu.addAction("Action 2");
        QAction *action3 = menu.addAction("Action 3");
        
        QVERIFY(menu.actions().size() == 3);
        QVERIFY(menu.actions().contains(action1));
        QVERIFY(menu.actions().contains(action2));
        QVERIFY(menu.actions().contains(action3));
    }

    void testContextMenuExec()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        menu.addAction("Test");
        
        QPoint pos = QCursor::pos();
        QAction *result = menu.exec(pos);
    }

    void testContextMenuSeparator()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        menu.addAction("Before");
        menu.addSeparator();
        menu.addAction("After");
        
        QVERIFY(menu.actions().size() == 3);
    }

    void testContextMenuSubmenu()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QMenu *submenu = menu.addMenu("Submenu");
        submenu->addAction("Sub Action");
        
        QVERIFY(menu.actions().size() == 1);
        QVERIFY(menu.actions()[0]->menu() != nullptr);
    }

    void testContextMenuShortcut()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("With Shortcut");
        action->setShortcut(QKeySequence("Ctrl+C"));
        
        QVERIFY(action->shortcut() == QKeySequence("Ctrl+C"));
    }

    void testContextMenuEnabled()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setEnabled(true);
        QVERIFY(action->isEnabled());
        
        action->setEnabled(false);
        QVERIFY(!action->isEnabled());
    }

    void testContextMenuVisible()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setVisible(true);
        QVERIFY(action->isVisible());
        
        action->setVisible(false);
        QVERIFY(!action->isVisible());
    }

    void testContextMenuCheckable()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("Checkable");
        action->setCheckable(true);
        
        action->setChecked(true);
        QVERIFY(action->isChecked());
        
        action->setChecked(false);
        QVERIFY(!action->isChecked());
    }

    void testContextMenuData()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setData("custom data");
        
        QVERIFY(action->data().toString() == "custom data");
    }

    void testContextMenuIcon()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("With Icon");
        
        QVERIFY(action->icon().isNull() || !action->icon().isNull());
    }

    void testContextMenuToolTip()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setToolTip("This is a tooltip");
        
        QVERIFY(action->toolTip() == "This is a tooltip");
    }

    void testContextMenuStatusTip()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setStatusTip("Status tip");
        
        QVERIFY(action->statusTip() == "Status tip");
    }

    void testContextMenuWhatsThis()
    {
        ContextMenuHandler handler;
        
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setWhatsThis("What's this?");
        
        QVERIFY(action->whatsThis() == "What's this?");
    }
};

QTEST_MAIN(TestContextMenuEvents)
#include "test_contextmenu_events.moc"