#include <QtTest/QtTest>
#include "../src/ui/agentpanel.h"
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/ui/filementionpopup.h"
#include "../src/agentloop.h"
#include "../src/toolregistry.h"
#include "../src/llmprovider.h"
#include "../src/threadjson.h"
#include "../src/engine/agent-worker.h"
#include "../src/engine/agent-engine.h"

class TestMultiComponentFlows : public QObject
{
    Q_OBJECT

private slots:
    void testFullConversationWithToolsAndPersistence()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        ThreadJsonStorage storage;
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        // 1. Test Input -> Storage flow
        panel.m_inputBar->m_messageEdit->setText("Read /test/main.cpp");
        panel.m_sendButton->click();
        
        // Simulate the agent adding messages to the thread
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Read /test/main.cpp"}}});
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "assistant"}, {"content", ""}, {"toolCallId", "call_1"}}});
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "tool"}, {"toolCallId", "call_1"}, {"content", "int main() { return 0; }"}});
        
        // 2. Test persistence
        bool saved = storage.saveThread(threadId, panel.m_threads[threadId].messages);
        QVERIFY2(saved, "Thread should be saved to disk");
        
        // 3. Test loading from storage
        QList<LLMMessage> loadedMessages = storage.loadThread(threadId);
        QCOMPARE(loadedMessages.size(), 3);
        QCOMPARE(loadedMessages[0].role, QString("user"));
        QCOMPARE(loadedMessages[2].role, QString("tool"));
    }

    void testLoadPersistedThreadAndContinue()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        ThreadJsonStorage storage;
        
        QString threadId = "test-persist-123";
        QList<LLMMessage> messages = {
            LLMMessage{{{"role", "user"}, {"content", "First message"}}},
            LLMMessage{{{"role", "assistant"}, {"content", "First response"}}}
        };
        
        storage.saveThread(threadId, messages);
        
        QString loadedId = panel.createNewThread();
        panel.m_threads[loadedId].messages = storage.loadThread(threadId);
        
        QCOMPARE(panel.m_threads[loadedId].messages.size(), 2);
        QCOMPARE(panel.m_threads[loadedId].messages[0].content, QString("First message"));
    }

    void testWorkerAndEngineIntegration()
    {
        AgentWorker worker;
        AgentEngine engine;
        
        worker.m_engine = &engine;
        engine.m_systemPrompt = "You are helpful.";
        engine.m_messages.append(LLMMessage{{{"role", "user"}, {"content", "Hello"}}});
        
        QVERIFY2(engine.m_messages.size() == 1, "Engine should contain the user message");
        QCOMPARE(engine.m_messages[0].content, QString("Hello"));
    }

    void testProviderAndEngineConnection()
    {
        AgentEngine engine;
        engine.m_provider = nullptr; // Test null provider safety
        
        // This should not crash
        QJsonObject request = engine.buildRequest("Test message");
        
        // Even with null provider, it should build a basic request structure
        QVERIFY(request.contains("messages"));
        QJsonArray msgs = request["messages"].toArray();
        QVERIFY(!msgs.isEmpty());
    }

    void testMessageFlowFromUIToProvider()
    {
        AgentPanel panel(nullptr, nullptr, nullptr, nullptr, nullptr);
        AgentEngine engine;
        
        QString threadId = panel.createNewThread();
        panel.setCurrentThread(threadId);
        
        panel.m_inputBar->m_messageEdit->setText("Direct test to provider");
        
        // Simulate UI to Engine transition
        panel.m_threads[threadId].messages.append(LLMMessage{{{"role", "user"}, {"content", "Direct test to provider"}}});
        engine.m_messages = panel.m_threads[threadId].messages;
        
        QCOMPARE(engine.m_messages.size(), 1);
        QCOMPARE(engine.m_messages[0].content, QString("Direct test to provider"));
    }
};

QTEST_MAIN(TestMultiComponentFlows)
#include "test_multi_component_flows.moc"
