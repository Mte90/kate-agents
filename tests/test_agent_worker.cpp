#include <QtTest/QtTest>
#include "../src/engine/agent-worker.h"

class TestAgentWorker : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        AgentWorker worker;
        QVERIFY(worker.m_networkManager != nullptr);
    }

    void testPendingRepliesInitiallyEmpty()
    {
        AgentWorker worker;
        QVERIFY(worker.m_pendingReplies.isEmpty());
    }

    void testSignalEmission()
    {
        AgentWorker worker;
        
        QSignalSpy spy(&worker, &AgentWorker::requestCompleted);
        QVERIFY(spy.isValid());
    }
};

QTEST_MAIN(TestAgentWorker)
#include "test_agent_worker.moc"