#include <QtTest>
#include <QObject>

class TestStreamingChunk : public QObject
{
    Q_OBJECT

private slots:
    void testRemovePipeSeparators()
    {
        QString input = "|C|iao|!|";
        QString expected = "Ciao!";
        QString result = input.replace("|", "");
        QCOMPARE(result, expected);
    }

    void testRemovePipeSeparatorsMultipleWords()
    {
        QString input = "|Come| posso| aiut|arti|?|";
        QString expected = "Come posso aiutarti?";
        QString result = input.replace("|", "");
        QCOMPARE(result, expected);
    }

    void testEmptyString()
    {
        QString input = "";
        QString expected = "";
        QString result = input.replace("|", "");
        QCOMPARE(result, expected);
    }

    void testNoPipes()
    {
        QString input = "Ciao!";
        QString expected = "Ciao!";
        QString result = input.replace("|", "");
        QCOMPARE(result, expected);
    }

    void testOnlyPipes()
    {
        QString input = "||||";
        QString expected = "";
        QString result = input.replace("|", "");
        QCOMPARE(result, expected);
    }
};

QTEST_APPLESS_MAIN(TestStreamingChunk)
#include "test_streaming_chunk.moc"
