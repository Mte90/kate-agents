#include <QtTest/QtTest>
#include "../src/threadjson.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class TestThreadJsonPersistence : public QObject
{
    Q_OBJECT

private:
    QString getUniqueProjectId() {
        return "test-proj-" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "-" + QString::number(qrand());
    }

    void cleanupProject(const QString &projectId) {
        // Find the actual file on disk. 
        // Based on ARCHITECTURE_DECISIONS: {project}_threads.json
        // We need to know where it's stored. 
        // Since we can't easily find the path without the storage object,
        // we rely on the test creating unique IDs that don't collide.
    }

private slots:
    void testSaveAndLoadThread()
    {
        QString projectId = getUniqueProjectId();
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        QString threadId = "thread-1";
        QList<LLMMessage> originalMessages = {
            LLMMessage{{{"role", "user"}, {"content", "Hello"}}},
            LLMMessage{{{"role", "assistant"}, {"content", "Hi there!"}}}
        };
        
        bool saved = ThreadJsonStorage::saveThread(threadId, originalMessages);
        QVERIFY2(saved, "Thread should be saved successfully");
        
        // Verify by loading back
        QList<LLMMessage> loadedMessages = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loadedMessages.size(), 2);
        QCOMPARE(loadedMessages[0].role, QString("user"));
        QCOMPARE(loadedMessages[0].content, QString("Hello"));
        QCOMPARE(loadedMessages[1].role, QString("assistant"));
        QCOMPARE(loadedMessages[1].content, QString("Hi there!"));
    }

    void testDeleteThread()
    {
        QString projectId = getUniqueProjectId();
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        QString threadId = "thread-del";
        QList<LLMMessage> messages = {
            LLMMessage{{{"role", "user"}, {"content", "Delete me"}}}
        };
        
        ThreadJsonStorage::saveThread(threadId, messages);
        
        // Verify it exists first
        QVERIFY(ThreadJsonStorage::loadThread(threadId).size() > 0);
        
        bool deleted = ThreadJsonStorage::deleteThread(threadId);
        QVERIFY2(deleted, "Delete operation should return true");
        
        // Verify it is gone
        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QVERIFY(loaded.isEmpty());
    }

    void testLoadThreadNotExists()
    {
        QString projectId = getUniqueProjectId();
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread("non-existent-id");
        QVERIFY(loaded.isEmpty());
    }

    void testLoadAllThreads()
    {
        QString projectId = getUniqueProjectId();
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        // Setup 3 threads
        ThreadJsonStorage::saveThread("t1", {LLMMessage{{{"role", "user"}, {"content", "1"}}}});
        ThreadJsonStorage::saveThread("t2", {LLMMessage{{{"role", "user"}, {"content", "2"}}}});
        ThreadJsonStorage::saveThread("t3", {LLMMessage{{{"role", "user"}, {"content", "3"}}}});
        
        auto allThreads = ThreadJsonStorage::loadAllThreads();
        QCOMPARE(allThreads.size(), 3);
    }
};

QTEST_MAIN(TestThreadJsonPersistence)
#include "test_thread_json_persistence.moc"
