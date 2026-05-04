#include <QtTest>

class TestBasics : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
    }

    void cleanup()
    {
    }

    void testSimple()
    {
        QVERIFY(true);
    }

    void testString()
    {
        QString str = "Hello";
        QVERIFY(str.contains("ell"));
    }

    void testNumbers()
    {
        int i = 1;
        QVERIFY(i == 1);
    }
};

QTEST_MAIN(TestBasics)
#include "test_basics.moc"