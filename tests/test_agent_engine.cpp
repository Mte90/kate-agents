#include <QtTest/QtTest>
#include "../src/engine/agent-engine.h"

class TestAgentEngine : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        KateAgents::AgentEngine engine;
        QVERIFY(engine.m_networkManager != nullptr);
        QVERIFY(engine.m_retryCount == 0);
    }

    void testRetryCountInitiallyZero()
    {
        KateAgents::AgentEngine engine;
        QVERIFY(engine.m_retryCount == 0);
    }

    void testSignalEmission()
    {
        KateAgents::AgentEngine engine;
        
        QSignalSpy spy(&engine, &KateAgents::AgentEngine::responseReceived);
        QVERIFY(spy.isValid());
    }

    void testErrorSignalEmission()
    {
        KateAgents::AgentEngine engine;
        
        QSignalSpy spy(&engine, &KateAgents::AgentEngine::errorOccurred);
        QVERIFY(spy.isValid());
    }
};

QTEST_MAIN(TestAgentEngine)
#include "test_agent_engine.moc"