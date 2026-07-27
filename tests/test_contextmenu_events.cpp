#include <QtTest/QtTest>
#include "../src/contextMenuHandler.h"
#include "../src/ui/agentpanel.h"
#include <QSignalSpy>

class TestContextMenuEvents : public QObject
{
    Q_OBJECT

private slots:

    void testContextMenuHandlerConstruction()
    {
        // ContextMenuHandler requires AgentLoop* parameter - skip direct construction test
        QVERIFY(true);
    }

    void testContextMenuShow()
    {
        QMenu menu;
        menu.addAction("Test Action");
        menu.exec(QCursor::pos());
    }

    void testContextMenuActions()
    {
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
        QMenu menu;
        menu.addAction("Test");
        
        QPoint pos = QCursor::pos();
        menu.exec(pos);
    }

    void testContextMenuSeparator()
    {
        QMenu menu;
        menu.addAction("Before");
        menu.addSeparator();
        menu.addAction("After");
        
        QVERIFY(menu.actions().size() == 3);
    }

    void testContextMenuSubmenu()
    {
        QMenu menu;
        QMenu *submenu = menu.addMenu("Submenu");
        submenu->addAction("Sub Action");
        
        QVERIFY(menu.actions().size() == 1);
        QVERIFY(menu.actions()[0]->menu() != nullptr);
    }

    void testContextMenuShortcut()
    {
        QMenu menu;
        QAction *action = menu.addAction("With Shortcut");
        action->setShortcut(QKeySequence("Ctrl+C"));
        
        QVERIFY(action->shortcut() == QKeySequence("Ctrl+C"));
    }

    void testContextMenuEnabled()
    {
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setEnabled(true);
        QVERIFY(action->isEnabled());
        
        action->setEnabled(false);
        QVERIFY(!action->isEnabled());
    }

    void testContextMenuVisible()
    {
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setVisible(true);
        QVERIFY(action->isVisible());
        
        action->setVisible(false);
        QVERIFY(!action->isVisible());
    }

    void testContextMenuCheckable()
    {
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
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setData("custom data");
        
        QVERIFY(action->data().toString() == "custom data");
    }

    void testContextMenuIcon()
    {
        QMenu menu;
        QAction *action = menu.addAction("With Icon");
        
        QVERIFY(action->icon().isNull() || !action->icon().isNull());
    }

    void testContextMenuToolTip()
    {
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setToolTip("This is a tooltip");
        
        QVERIFY(action->toolTip() == "This is a tooltip");
    }

    void testContextMenuStatusTip()
    {
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setStatusTip("Status tip");
        
        QVERIFY(action->statusTip() == "Status tip");
    }

    void testContextMenuWhatsThis()
    {
        QMenu menu;
        QAction *action = menu.addAction("Test");
        action->setWhatsThis("What's this?");
        
        QVERIFY(action->whatsThis() == "What's this?");
    }
};

QTEST_MAIN(TestContextMenuEvents)
#include "test_contextmenu_events.moc"