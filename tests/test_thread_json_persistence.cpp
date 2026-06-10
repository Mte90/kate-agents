#include <QtTest/QtTest>
#include "../src/threadjson.h"

class TestThreadJsonPersistence : public QObject
{
    Q_OBJECT

private slots:

    void testSaveAndLoadThread()
    {
        QString projectId = "test-project-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        ConversationThread thread;
        thread.id = "thread-1";
        thread.title = "Test Thread";
        
        bool saved = ThreadJsonStorage::saveThread("thread-1", thread.messages);
        QVERIFY(saved == true);
        
        ThreadJsonStorage::deleteThread("thread-1");
    }

    void testDeleteThread()
    {
        QString projectId = "test-delete-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        ConversationThread thread;
        thread.id = "thread-del";
        
        ThreadJsonStorage::saveThread("thread-del", thread.messages);
        bool deleted = ThreadJsonStorage::deleteThread("thread-del");
    }

    void testLoadThreadNotExists()
    {
        QString projectId = "test-nonexist-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        auto threads = ThreadJsonStorage::loadAllThreads();
    }
};

QTEST_MAIN(TestThreadJsonPersistence)
#include "test_thread_json_persistence.moc"