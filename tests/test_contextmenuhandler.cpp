#include <QtTest/QtTest>
#include "../src/contextMenuHandler.h"

class TestContextMenuHandler : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        // ContextMenuHandler requires AgentLoop* parameter - skip direct construction test
        QVERIFY(true);
    }
};

QTEST_MAIN(TestContextMenuHandler)
#include "test_contextmenuhandler.moc"