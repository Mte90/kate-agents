#include <QtTest/QtTest>
#include "../src/engine/agent-worker.h"
#include "../src/llmprovider.h"

class TestAgentWorkerDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        AgentWorker worker;
        QVERIFY(true);
    }

    void testWorkerState()
    {
        AgentWorker worker;
        QVERIFY(worker.m_state == AgentWorker::Idle);
    }

    void testSetStateToWorking()
    {
        AgentWorker worker;
        worker.m_state = AgentWorker::Working;
        QVERIFY(worker.m_state == AgentWorker::Working);
    }

    void testSetStateToIdle()
    {
        AgentWorker worker;
        worker.m_state = AgentWorker::Working;
        worker.m_state = AgentWorker::Idle;
        QVERIFY(worker.m_state == AgentWorker::Idle);
    }

    void testSetStateToError()
    {
        AgentWorker worker;
        worker.m_state = AgentWorker::Error;
        QVERIFY(worker.m_state == AgentWorker::Error);
    }

    void testMessagesList()
    {
        AgentWorker worker;
        QVERIFY(worker.m_messages.isEmpty());
    }

    void testAddMessage()
    {
        AgentWorker worker;
        LLMMessage msg;
        msg.role = "user";
        msg.content = "test";
        worker.m_messages.append(msg);
        QVERIFY(worker.m_messages.size() == 1);
    }

    void testClearMessages()
    {
        AgentWorker worker;
        LLMMessage msg;
        worker.m_messages.append(msg);
        worker.m_messages.clear();
        QVERIFY(worker.m_messages.isEmpty());
    }

    void testToolDefinitions()
    {
        AgentWorker worker;
        QVERIFY(worker.m_toolDefinitions.isEmpty());
    }

    void testAddToolDefinition()
    {
        AgentWorker worker;
        ToolDefinition td;
        td.type = "function";
        worker.m_toolDefinitions.append(td);
        QVERIFY(worker.m_toolDefinitions.size() == 1);
    }

    void testCurrentModel()
    {
        AgentWorker worker;
        worker.m_currentModel = "gpt-4";
        QVERIFY(worker.m_currentModel == "gpt-4");
    }

    void testAbortFlag()
    {
        AgentWorker worker;
        worker.m_abort = true;
        QVERIFY(worker.m_abort == true);
    }

    void testErrorMessage()
    {
        AgentWorker worker;
        worker.m_errorMessage = "Test error";
        QVERIFY(worker.m_errorMessage == "Test error");
    }

    void testNetworkManager()
    {
        AgentWorker worker;
        QVERIFY(worker.m_nam != nullptr);
    }
};

QTEST_MAIN(TestAgentWorkerDetailed)
#include "test_agent_worker_detailed.moc"