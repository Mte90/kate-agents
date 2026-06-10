#include <QtTest/QtTest>
#include "../src/threadstorage.h"

class TestThreadStorageDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testDatabasePathNotEmpty()
    {
        ThreadStorage storage;
        QString path = storage.databasePath();
        QVERIFY(!path.isEmpty());
    }

    void testInitialize()
    {
        ThreadStorage storage;
        bool result = storage.initialize();
        QVERIFY(result == true);
    }

    void testSetCurrentProjectId()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("test-project");
        QVERIFY(storage.m_currentProjectId == "test-project");
    }

    void testLoadAllThreadsEmpty()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("nonexistent-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        auto threads = storage.loadAllThreads();
        QVERIFY(threads.isEmpty());
    }

    void testLoadAllThreadsWithData()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("test-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        auto threads = storage.loadAllThreads();
    }

    void testCreateThread()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("create-test-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        QString id = storage.createThread();
        QVERIFY(!id.isEmpty());
    }

    void testDeleteThread()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("delete-test-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    }

    void testSaveThread()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("save-test-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    }

    void testGetThread()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("get-test-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    }

    void testListThreads()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("list-test-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    }
};

QTEST_MAIN(TestThreadStorageDetailed)
#include "test_threadstorage_detailed.moc"