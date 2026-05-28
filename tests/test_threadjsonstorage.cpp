#include <QtTest/QtTest>
#include "../src/threadjson.h"
#include <QFile>
#include <QDir>

class TestThreadJsonStorage : public QObject
{
    Q_OBJECT

private slots:
    void testGetThreadDir()
    {
        QString dir = ThreadJsonStorage::getThreadDir();
        QVERIFY(!dir.isEmpty());
        QVERIFY(QDir(dir).exists());
    }

    void testSaveAndLoadThread()
    {
        QString threadId = "test-thread-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append(LLMMessage{"user", "Hello", "", "", ""});
        messages.append(LLMMessage{"assistant", "Hi there!", "", "", ""});
        
        bool saved = ThreadJsonStorage::saveThread(threadId, messages);
        QVERIFY(saved);
        
        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 2);
        QCOMPARE(loaded[0].role, QString("user"));
        QCOMPARE(loaded[0].content, QString("Hello"));
    }

    void testDeleteThread()
    {
        QString threadId = "test-delete-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append(LLMMessage{"user", "Test", "", "", ""});
        
        bool saved = ThreadJsonStorage::saveThread(threadId, messages);
        QVERIFY(saved);
        
        bool deleted = ThreadJsonStorage::deleteThread(threadId);
        QVERIFY(deleted);
    }

    void testListThreads()
    {
        QString threadId = "test-list-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append(LLMMessage{"user", "Test", "", "", ""});
        
        bool saved = ThreadJsonStorage::saveThread(threadId, messages);
        QVERIFY(saved);
        
        QStringList threads = ThreadJsonStorage::listThreads();
        QVERIFY(threads.contains(threadId));
        
        ThreadJsonStorage::deleteThread(threadId);
    }
};

QTEST_MAIN(TestThreadJsonStorage)
#include "test_threadjsonstorage.moc"
