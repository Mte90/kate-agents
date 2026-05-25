#include <QtTest/QtTest>
#include "../src/threadjson.h"
#include <QFile>
#include <QDir>
#include <QTemporaryDir>

class TestThreadJsonAdvanced : public QObject
{
    Q_OBJECT

private:
    QString m_testProjectId;

private slots:
    void initTestCase()
    {
        m_testProjectId = QStringLiteral("test-kate-agent-%1").arg(QDateTime::currentMSecsSinceEpoch());
        ThreadJsonStorage::setCurrentProjectId(m_testProjectId);
    }

    void cleanup()
    {
        QStringList threads = ThreadJsonStorage::listThreads();
        for (const QString &id : threads) {
            ThreadJsonStorage::deleteThread(id);
        }
    }

    void testGetThreadDir()
    {
        QString dir = ThreadJsonStorage::getThreadDir();
        QVERIFY(!dir.isEmpty());
        QVERIFY(QDir(dir).exists());
    }

    void testGetProjectPrefixSimple()
    {
        QCOMPARE(ThreadJsonStorage::getProjectPrefix("myproject"), QStringLiteral("myproject_"));
    }

    void testGetProjectPrefixWithSpaces()
    {
        QCOMPARE(ThreadJsonStorage::getProjectPrefix("my project"), QStringLiteral("my_project_"));
    }

    void testGetProjectPrefixWithSlashes()
    {
        QCOMPARE(ThreadJsonStorage::getProjectPrefix("my/project"), QStringLiteral("my_project_"));
        QCOMPARE(ThreadJsonStorage::getProjectPrefix("my\\path"), QStringLiteral("my_path_"));
    }

    void testGetProjectPrefixWithSpecialChars()
    {
        QString result = ThreadJsonStorage::getProjectPrefix("proj:name*test?\"<>");
        QCOMPARE(result, QStringLiteral("proj_name_test_____"));
    }

    void testGetProjectPrefixWithPipe()
    {
        QCOMPARE(ThreadJsonStorage::getProjectPrefix("a|b"), QStringLiteral("a_b_"));
    }

    void testGetProjectPrefixEmpty()
    {
        QCOMPARE(ThreadJsonStorage::getProjectPrefix(""), QString());
    }

    void testSetCurrentProjectId()
    {
        QString testId = QStringLiteral("proj-test-%1").arg(QDateTime::currentMSecsSinceEpoch());
        ThreadJsonStorage::setCurrentProjectId(testId);
        QCOMPARE(ThreadJsonStorage::getCurrentProjectId(), testId);
        ThreadJsonStorage::setCurrentProjectId(m_testProjectId);
    }

    void testGetThreadFilePath()
    {
        QString path = ThreadJsonStorage::getThreadFilePath(m_testProjectId);
        QVERIFY(path.contains("threads.json"));
        QVERIFY(path.contains(m_testProjectId));
    }

    void testSaveAndLoadRoundTrip()
    {
        QString threadId = QStringLiteral("roundtrip-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append({QStringLiteral("user"), QStringLiteral("Hello"), QString(), QString()});
        messages.append({QStringLiteral("assistant"), QStringLiteral("Hi there!"), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, messages));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 2);
        QCOMPARE(loaded[0].role, QStringLiteral("user"));
        QCOMPARE(loaded[0].content, QStringLiteral("Hello"));
        QCOMPARE(loaded[1].role, QStringLiteral("assistant"));
        QCOMPARE(loaded[1].content, QStringLiteral("Hi there!"));
    }

    void testSaveEmptyMessages()
    {
        QString threadId = QStringLiteral("empty-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;

        QVERIFY(ThreadJsonStorage::saveThread(threadId, messages));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 0);
    }

    void testSaveMessagesWithAllFields()
    {
        QString threadId = QStringLiteral("allfields-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> messages;
        messages.append({QStringLiteral("user"), QStringLiteral("content"), QStringLiteral("Write"), QString()});
        messages.append({QStringLiteral("tool"), QStringLiteral("result"), QString(), QStringLiteral("call_123")});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, messages));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 2);
        QCOMPARE(loaded[0].profile, QStringLiteral("Write"));
        QCOMPARE(loaded[1].toolCallId, QStringLiteral("call_123"));
    }

    void testSaveMultipleThreads()
    {
        QString id1 = QStringLiteral("multi1-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QString id2 = QStringLiteral("multi2-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QStringLiteral("test"), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(id1, msgs));
        QVERIFY(ThreadJsonStorage::saveThread(id2, msgs));

        QStringList threads = ThreadJsonStorage::listThreads();
        QVERIFY(threads.contains(id1));
        QVERIFY(threads.contains(id2));
    }

    void testLoadNonExistentThread()
    {
        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread("nonexistent-thread-xyz");
        QCOMPARE(loaded.size(), 0);
    }

    void testLoadThreadTitle()
    {
        QString threadId = QStringLiteral("title-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QStringLiteral("test"), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs, QString(), "My Test Title"));
        QCOMPARE(ThreadJsonStorage::loadThreadTitle(threadId), QStringLiteral("My Test Title"));
    }

    void testLoadThreadTitleMissing()
    {
        QString threadId = QStringLiteral("notitle-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QStringLiteral("test"), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs));
        QVERIFY(ThreadJsonStorage::loadThreadTitle(threadId).isEmpty());
    }

    void testDeleteThread()
    {
        QString threadId = QStringLiteral("del-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QStringLiteral("test"), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs));
        QVERIFY(ThreadJsonStorage::deleteThread(threadId));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 0);
    }

    void testDeleteNonExistentThread()
    {
        QVERIFY(!ThreadJsonStorage::deleteThread("nonexistent-del-xyz"));
    }

    void testDeleteThenSave()
    {
        QString threadId = QStringLiteral("delresave-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs1;
        msgs1.append({QStringLiteral("user"), QStringLiteral("first"), QString(), QString()});
        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs1));

        QVERIFY(ThreadJsonStorage::deleteThread(threadId));

        QList<LLMMessage> msgs2;
        msgs2.append({QStringLiteral("user"), QStringLiteral("second"), QString(), QString()});
        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs2));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 1);
        QCOMPARE(loaded[0].content, QStringLiteral("second"));
    }

    void testOverwriteThread()
    {
        QString threadId = QStringLiteral("overwrite-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs1;
        msgs1.append({QStringLiteral("user"), QStringLiteral("version1"), QString(), QString()});
        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs1));

        QList<LLMMessage> msgs2;
        msgs2.append({QStringLiteral("user"), QStringLiteral("version2"), QString(), QString()});
        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs2));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 1);
        QCOMPARE(loaded[0].content, QStringLiteral("version2"));
    }

    void testMalformedJSONFile()
    {
        QString filePath = ThreadJsonStorage::getThreadFilePath(m_testProjectId);
        QString backupPath = filePath + ".bak";
        if (QFile::exists(filePath)) {
            QFile::rename(filePath, backupPath);
        }

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("NOT VALID JSON {{{}}}");
            file.close();
        }

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread("any-thread");
        QCOMPARE(loaded.size(), 0);

        if (QFile::exists(backupPath)) {
            QFile::remove(filePath);
            QFile::rename(backupPath, filePath);
        }
    }

    void testEmptyJSONFile()
    {
        QString filePath = ThreadJsonStorage::getThreadFilePath(m_testProjectId);
        QString backupPath = filePath + ".bak2";
        if (QFile::exists(filePath)) {
            QFile::rename(filePath, backupPath);
        }

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
        }

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread("any-thread");
        QCOMPARE(loaded.size(), 0);

        if (QFile::exists(backupPath)) {
            QFile::remove(filePath);
            QFile::rename(backupPath, filePath);
        }
    }

    void testProjectIsolation()
    {
        QString projA = QStringLiteral("iso-proj-A-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QString projB = QStringLiteral("iso-proj-B-%1").arg(QDateTime::currentMSecsSinceEpoch());

        QString threadIdA = QStringLiteral("thread-a-%1").arg(QDateTime::currentMSecsSinceEpoch());

        QList<LLMMessage> msgsA;
        msgsA.append({QStringLiteral("user"), QStringLiteral("project A message"), QString(), QString()});

        ThreadJsonStorage::setCurrentProjectId(projA);
        QVERIFY(ThreadJsonStorage::saveThread(threadIdA, msgsA));

        ThreadJsonStorage::setCurrentProjectId(projB);
        QStringList threadsB = ThreadJsonStorage::listThreads();
        QVERIFY(!threadsB.contains(threadIdA));

        ThreadJsonStorage::setCurrentProjectId(projA);
        QStringList threadsA = ThreadJsonStorage::listThreads();
        QVERIFY(threadsA.contains(threadIdA));
        ThreadJsonStorage::deleteThread(threadIdA);

        ThreadJsonStorage::setCurrentProjectId(m_testProjectId);
    }

    void testLargeMessageContent()
    {
        QString threadId = QStringLiteral("large-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QString(50000, QChar('x')), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 1);
        QCOMPARE(loaded[0].content.size(), 50000);
    }

    void testUnicodeContent()
    {
        QString threadId = QStringLiteral("unicode-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QStringLiteral("こんにちは世界 🌍 Привет"), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded[0].content, QStringLiteral("こんにちは世界 🌍 Привет"));
    }

    void testListThreadsForProject()
    {
        QString projId = QStringLiteral("list-proj-%1").arg(QDateTime::currentMSecsSinceEpoch());
        ThreadJsonStorage::setCurrentProjectId(projId);

        QString threadId = QStringLiteral("list-thread-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QStringLiteral("test"), QString(), QString()});
        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs));

        QStringList threads = ThreadJsonStorage::listThreadsForProject(projId);
        QVERIFY(threads.contains(threadId));

        ThreadJsonStorage::deleteThread(threadId);
        ThreadJsonStorage::setCurrentProjectId(m_testProjectId);
    }

    void testSaveThreadWithModel()
    {
        QString threadId = QStringLiteral("model-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        msgs.append({QStringLiteral("user"), QStringLiteral("test"), QString(), QString()});

        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs, "gpt-4"));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 1);
    }

    void testManyMessagesInThread()
    {
        QString threadId = QStringLiteral("many-msgs-%1").arg(QDateTime::currentMSecsSinceEpoch());
        QList<LLMMessage> msgs;
        for (int i = 0; i < 100; ++i) {
            msgs.append({QStringLiteral("user"), QStringLiteral("msg %1").arg(i), QString(), QString()});
            msgs.append({QStringLiteral("assistant"), QStringLiteral("resp %1").arg(i), QString(), QString()});
        }

        QVERIFY(ThreadJsonStorage::saveThread(threadId, msgs));

        QList<LLMMessage> loaded = ThreadJsonStorage::loadThread(threadId);
        QCOMPARE(loaded.size(), 200);
        QCOMPARE(loaded[0].content, QStringLiteral("msg 0"));
        QCOMPARE(loaded[199].content, QStringLiteral("resp 99"));
    }
};

QTEST_MAIN(TestThreadJsonAdvanced)
#include "test_threadjson_advanced.moc"
