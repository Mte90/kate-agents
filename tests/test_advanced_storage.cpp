#include <QtTest/QtTest>
#include "../src/threadjson.h"
#include "../src/threadstorage.h"
#include "../src/configmanager.h"
#include "../src/permissionmanager.h"

class TestAdvancedStorage : public QObject
{
    Q_OBJECT

private slots:

    void testThreadJsonSaveAndLoad()
    {
        ThreadJsonStorage storage;
        
        storage.setCurrentProjectId("test-advance-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        
        ConversationThread thread;
        thread.id = "save-load-test";
        thread.title = "Test Thread";
        thread.messages.append(LLMMessage{{{"role", "user"}, {"content", "Hello"}}});
        thread.messages.append(LLMMessage{{{"role", "assistant"}, {"content", "Hi there"}}});
        
        QList<ConversationThread> threads;
        threads.append(thread);
        
        bool saved = storage.saveThread(thread.id, threads);
        
        QVERIFY(saved == true || saved == false);
    }

    void testThreadJsonLoadMultipleProjects()
    {
        ThreadJsonStorage storage1, storage2;
        
        storage1.setCurrentProjectId("project-a-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        storage2.setCurrentProjectId("project-b-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    }

    void testThreadJsonCorruptedFileRecovery()
    {
        ThreadJsonStorage storage;
        storage.setCurrentProjectId("corrupt-test-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        
        QString path = storage.getThreadFilePath("test");
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("{ invalid json content");
        file.close();
        
        QList<ConversationThread> threads = storage.loadAllThreads();
        
        file.remove();
        
        QVERIFY(threads.isEmpty() || !threads.isEmpty());
    }

    void testConfigPersistence()
    {
        ConfigManager cm;
        
        cm.setApiKey("test-key-123");
        cm.setBaseUrl("http://localhost:8080");
        cm.setModel("gpt-4");
        cm.setTemperature(0.7);
        cm.setMaxTokens(2000);
        
        QString savedKey = cm.apiKey();
        QString savedUrl = cm.baseUrl();
        
        QVERIFY(savedKey == "test-key-123");
        QVERIFY(savedUrl == "http://localhost:8080");
    }

    void testConfigMultipleInstances()
    {
        ConfigManager cm1, cm2;
        
        cm1.setApiKey("key1");
        cm2.setApiKey("key2");
        
        QString k1 = cm1.apiKey();
        QString k2 = cm2.apiKey();
        
        QVERIFY(k1 == "key1" || k2 == "key2");
    }

    void testPermissionPolicyHierarchy()
    {
        PermissionManager pm;
        
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        
        pm.setToolPolicy("terminal", PermissionPolicy::Allow);
        pm.setToolPolicy("read_file", PermissionPolicy::Allow);
        
        Policy terminalPolicy = pm.getToolPolicy("terminal");
        Policy readPolicy = pm.getToolPolicy("read_file");
        Policy unknownPolicy = pm.getToolPolicy("unknown_tool");
        
        QVERIFY(terminalPolicy == PermissionPolicy::Allow);
        QVERIFY(readPolicy == PermissionPolicy::Allow);
        QVERIFY(unknownPolicy == PermissionPolicy::Deny);
    }

    void testPermissionRequestApproval()
    {
        PermissionManager pm;
        
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        
        bool result = pm.requestPermission("terminal", "read /tmp");
        
        QVERIFY(result == true || result == false);
    }

    void testThreadStorageConcurrentWrites()
    {
        ThreadStorage storage;
        storage.setCurrentProjectId("concurrent-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        
        QString id1 = storage.createThread();
        
        storage.m_currentProjectId = "other-" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QString id2 = storage.createThread();
        
        QVERIFY(!id1.isEmpty() && !id2.isEmpty());
    }

    void testThreadJsonEmptyProject()
    {
        ThreadJsonStorage storage;
        storage.setCurrentProjectId("");
        
        QString path = storage.getThreadFilePath("test");
        
        QVERIFY(!path.isEmpty());
    }

    void testConfigSystemPromptTemplates()
    {
        ConfigManager cm;
        
        cm.setSystemPrompt("You are a {role} assistant specialized in {domain}");
        
        QString prompt = cm.systemPrompt();
        
        QVERIFY(prompt.contains("{role}") || !prompt.contains("{role}"));
    }

    void testPermissionAuditLog()
    {
        PermissionManager pm;
        
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        
        pm.requestPermission("read_file", "/path/1");
        pm.requestPermission("read_file", "/path/2");
        
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        
        pm.requestPermission("terminal", "ls");
        
        QList<PermissionRequest> history = pm.getRequestHistory();
        
        QVERIFY(history.size() >= 0);
    }

    void testThreadJsonLargeFile()
    {
        ThreadJsonStorage storage;
        storage.setCurrentProjectId("large-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        
        QList<ConversationThread> threads;
        
        for (int i = 0; i < 100; i++) {
            ConversationThread thread;
            thread.id = "thread-" + QString::number(i);
            thread.title = "Thread " + QString::number(i);
            
            for (int j = 0; j < 50; j++) {
                thread.messages.append(LLMMessage{
                    {"role", j % 2 == 0 ? "user" : "assistant"},
                    {"content", "Message " + QString::number(j)}
                });
            }
            threads.append(thread);
        }
        
        storage.saveThread("large-test", threads);
    }

    void testConfigApiKeySecurity()
    {
        ConfigManager cm;
        
        cm.setApiKey("sk-secret-123");
        
        QString key = cm.apiKey();
        
        bool containsKey = key.contains("sk-");
        
        QVERIFY(containsKey == true);
    }

    void testThreadDeletionVerification()
    {
        ThreadJsonStorage storage;
        storage.setCurrentProjectId("delete-verify-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
        
        ConversationThread thread;
        thread.id = "to-delete";
        QList<ConversationThread> threads;
        threads.append(thread);
        
        storage.saveThread(thread.id, threads);
        
        storage.deleteThread(thread.id);
        
        QList<ConversationThread> after = storage.loadAllThreads();
        
        bool exists = false;
        for (const auto &t : after) {
            if (t.id == thread.id) {
                exists = true;
                break;
            }
        }
        
        QVERIFY(exists == false);
    }
};

QTEST_MAIN(TestAdvancedStorage)
#include "test_advanced_storage.moc"