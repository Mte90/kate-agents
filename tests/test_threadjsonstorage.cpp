#include <QtTest/QtTest>
#include "../src/threadjson.h"
#include "../src/threadstorage.h"
#include <QFile>
#include <QDir>
#include <QDateTime>

class TestThreadJsonStorage : public QObject
{
    Q_OBJECT

private:
    ThreadJsonStorage *m_storage;

private slots:
    void init() {
        m_storage = new ThreadJsonStorage();
    }

    void cleanup() {
        delete m_storage;
    }

    void testGetThreadDir()
    {
        QString dir = m_storage->getThreadDir();
        QVERIFY(!dir.isEmpty());
        QVERIFY(QDir(dir).exists());
    }

    void testSaveAndLoadThread()
    {
        QString threadId = "test-thread-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append(LLMMessage{"user", "Hello", "", "", ""});
        messages.append(LLMMessage{"assistant", "Hi there!", "", "", ""});
        
        bool saved = m_storage->saveThread(threadId, messages);
        QVERIFY(saved);
        
        QList<LLMMessage> loaded = m_storage->loadThread(threadId);
        QCOMPARE(loaded.size(), 2);
        QCOMPARE(loaded[0].role, QString("user"));
        QCOMPARE(loaded[0].content, QString("Hello"));
    }

    void testDeleteThread()
    {
        QString threadId = "test-delete-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append(LLMMessage{"user", "Test", "", "", ""});
        
        bool saved = m_storage->saveThread(threadId, messages);
        QVERIFY(saved);
        
        bool deleted = m_storage->deleteThread(threadId);
        QVERIFY(deleted);
    }

    void testListThreads()
    {
        QString threadId = "test-list-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append(LLMMessage{"user", "Test", "", "", ""});
        
        bool saved = m_storage->saveThread(threadId, messages);
        QVERIFY(saved);
        
        QStringList threads = m_storage->listThreads();
        QVERIFY(threads.contains(threadId));
        
        m_storage->deleteThread(threadId);
    }
};

QTEST_MAIN(TestThreadJsonStorage)
#include "test_threadjsonstorage.moc"

