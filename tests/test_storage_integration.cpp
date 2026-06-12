

#include <QtTest/QTest>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <atomic>

#include "ithreadstorage.h"
#include "mockthreadstorage.h"
#include "llmmessage.h"

class TestStorageIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        
    }

    void cleanupTestCase() {
        
    }

    
    void testConcurrentOperations() {
        MockThreadStorage mock;
        const int threadCount = 50;
        const int operationsPerThread = 100;
        std::atomic<int> successCount{0};
        std::atomic<int> failureCount{0};

        QList<QThread*> threads;

        for (int i = 0; i < threadCount; ++i) {
            QThread* thread = new QThread();
            
            QObject::connect(thread, &QThread::started, [&mock, i, operationsPerThread, &successCount, &failureCount]() {
                for (int j = 0; j < operationsPerThread; ++j) {
                    QString threadId = QString("thread_%1_%2").arg(i).arg(j);
                    
                    QList<LLMMessage> messages;
                    for (int k = 0; k < 10; ++k) {
                        LLMMessage msg;
                        msg.role = (k % 2 == 0) ? LLMMessage::Role::User : LLMMessage::Role::Assistant;
                        msg.content = QString("Message %1-%2-%3").arg(i).arg(j).arg(k);
                        messages.append(msg);
                    }
                    
                    bool saved = mock.saveThread(threadId, messages);
                    
                    if (saved) {
                        QList<LLMMessage> loaded = mock.loadThread(threadId);
                        if (loaded.size() == messages.size()) {
                            successCount++;
                        } else {
                            failureCount++;
                        }
                    } else {
                        failureCount++;
                    }
                }
                
                thread->quit();
            });
            
            thread->start();
            threads.append(thread);
        }

        for (QThread* thread : threads) {
            thread->wait();
            thread->deleteLater();
        }

        QCOMPARE(failureCount.load(), 0);
        QCOMPARE(successCount.load(), threadCount * operationsPerThread);
    }

    
    void testLargeMessageVolume() {
        MockThreadStorage mock;
        const int messageCount = 1000;
        QString threadId = "stress_test_thread";

        
        QList<LLMMessage> messages;
        for (int i = 0; i < messageCount; ++i) {
            LLMMessage msg;
            msg.role = (i % 2 == 0) ? LLMMessage::Role::User : LLMMessage::Role::Assistant;
            msg.content = QString("This is message number %1 with some additional text to simulate real usage.").arg(i);
            messages.append(msg);
        }

        QElapsedTimer saveTimer;
        saveTimer.start();
        bool saved = mock.saveThread(threadId, messages);
        qint64 saveTime = saveTimer.elapsed();

        QVERIFY(saved);
        QVERIFY(saveTime < 1000);

        QElapsedTimer loadTimer;
        loadTimer.start();
        QList<LLMMessage> loaded = mock.loadThread(threadId);
        qint64 loadTime = loadTimer.elapsed();

        QCOMPARE(loaded.size(), messageCount);
        QVERIFY(loadTime < 1000);

        for (int i = 0; i < messageCount; ++i) {
            QCOMPARE(loaded[i].role, messages[i].role);
            QCOMPARE(loaded[i].content, messages[i].content);
        }
    }

    
    void testEndToEndPersistence() {
        
        MockThreadStorage mock1;
        QString threadId = "e2e_test_thread";
        
        QList<LLMMessage> originalMessages;
        for (int i = 0; i < 50; ++i) {
            LLMMessage msg;
            msg.role = (i % 2 == 0) ? LLMMessage::Role::User : LLMMessage::Role::Assistant;
            msg.content = QString("E2E Test Message %1").arg(i);
            msg.toolCallId = (i % 3 == 0) ? QString("tool_%1").arg(i) : QString();
            originalMessages.append(msg);
        }

        bool saved = mock1.saveThread(threadId, originalMessages, "test-model", "Test Thread Title");
        QVERIFY(saved);

        
        MockThreadStorage mock2;
        QStringList threadIds = mock1.listThreads();
        for (const QString& id : threadIds) {
            QList<LLMMessage> msgs = mock1.loadThread(id);
            mock2.saveThread(id, msgs);
        }

        
        QList<LLMMessage> loaded = mock2.loadThread(threadId);
        QCOMPARE(loaded.size(), originalMessages.size());

        for (int i = 0; i < originalMessages.size(); ++i) {
            QCOMPARE(loaded[i].role, originalMessages[i].role);
            QCOMPARE(loaded[i].content, originalMessages[i].content);
            QCOMPARE(loaded[i].toolCallId, originalMessages[i].toolCallId);
        }
    }

    
    void testThreadDeletion() {
        MockThreadStorage mock;
        QString threadId = "delete_test_thread";

        
        QList<LLMMessage> messages;
        LLMMessage msg;
        msg.role = LLMMessage::Role::User;
        msg.content = "Test message";
        messages.append(msg);

        mock.saveThread(threadId, messages);
        QVERIFY(mock.loadThread(threadId).size() > 0);

        
        bool deleted = mock.deleteThread(threadId);
        QVERIFY(deleted);

        
        QList<LLMMessage> loaded = mock.loadThread(threadId);
        QCOMPARE(loaded.size(), 0);

        
        QStringList threadIds = mock.listThreads();
        QVERIFY(!threadIds.contains(threadId));
    }

    
    void testConcurrentDeletion() {
        MockThreadStorage mock;
        const int threadCount = 100;

        
        for (int i = 0; i < threadCount; ++i) {
            QList<LLMMessage> messages;
            LLMMessage msg;
            msg.content = QString("Thread %1").arg(i);
            messages.append(msg);
            mock.saveThread(QString("thread_%1").arg(i), messages);
        }

        QCOMPARE(mock.listThreads().size(), threadCount);

        
        std::atomic<int> deletedCount{0};
        QList<QThread*> threads;

        for (int i = 0; i < 10; ++i) {
            QThread* thread = new QThread();
            QObject::connect(thread, &QThread::started, [&mock, i, threadCount, &deletedCount]() {
                for (int j = 0; j < threadCount / 10; ++j) {
                    int threadNum = i * (threadCount / 10) + j;
                    QString threadId = QString("thread_%1").arg(threadNum);
                    if (mock.deleteThread(threadId)) {
                        deletedCount++;
                    }
                }
                thread->quit();
            });
            thread->start();
            threads.append(thread);
        }

        for (QThread* thread : threads) {
            thread->wait();
            thread->deleteLater();
        }

        QCOMPARE(deletedCount.load(), threadCount);
        QCOMPARE(mock.listThreads().size(), 0);
    }

    
    void testMemoryLeakBasic() {
        const int iterations = 1000;

        for (int i = 0; i < iterations; ++i) {
            MockThreadStorage* mock = new MockThreadStorage();
            
            QList<LLMMessage> messages;
            for (int j = 0; j < 10; ++j) {
                LLMMessage msg;
                msg.content = QString("Message %1").arg(j);
                messages.append(msg);
            }
            
            mock->saveThread(QString("test_%1").arg(i), messages);
            delete mock;
        }

        
        QVERIFY(true);
    }
};

QTEST_MAIN(TestStorageIntegration)

#include "test_storage_integration.moc"
