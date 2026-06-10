#include <QtTest/QtTest>
#include "../src/threadstorage.h"

class TestThreadStorage : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        ThreadStorage storage;
        QVERIFY(storage.m_currentProjectId.isEmpty());
    }

    void testDatabasePath()
    {
        ThreadStorage storage;
        QString path = storage.databasePath();
        QVERIFY(!path.isEmpty());
        QVERIFY(path.contains("kate/agents"));
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

    void testLoadAllThreadsInitiallyEmpty()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("nonexistent-project-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        QMap<QString, ConversationThread> threads = storage.loadAllThreads();
        QVERIFY(threads.isEmpty());
    }
};

QTEST_MAIN(TestThreadStorage)
#include "test_threadstorage.moc"