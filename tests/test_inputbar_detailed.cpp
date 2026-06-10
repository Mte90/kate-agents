#include <QtTest/QtTest>
#include "../src/ui/inputbar.h"
#include "../src/agentloop.h"

class TestInputBarDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_messageEdit != nullptr);
    }

    void testSendButtonExists()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_sendButton != nullptr);
    }

    void testProfileComboExists()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_profileCombo != nullptr);
    }

    void testModelComboExists()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_modelCombo != nullptr);
    }

    void testInitialProfile()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_currentProfile == AgentProfile::Write);
    }

    void testSetProfile()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_currentProfile = AgentProfile::Ask;
        QVERIFY(bar.m_currentProfile == AgentProfile::Ask);
    }

    void testModelsEmptyInitially()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_models.isEmpty());
    }

    void testSetModels()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_models << "gpt-4" << "claude-3";
        QVERIFY(bar.m_models.size() == 2);
    }

    void testInputBarMinHeight()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.minimumHeight() > 0);
    }
};

QTEST_MAIN(TestInputBarDetailed)
#include "test_inputbar_detailed.moc"