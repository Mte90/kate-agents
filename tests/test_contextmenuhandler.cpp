#include <QtTest/QtTest>
#include "../src/contextmenhander.h"

class TestContextMenuHandler : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        ContextMenuHandler handler;
        QVERIFY(true);
    }
};

QTEST_MAIN(TestContextMenuHandler)
#include "test_contextmenuhandler.moc"