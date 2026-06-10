#include <QtTest/QtTest>
#include "../src/ui/inputbar.h"
#include "../src/agentloop.h"

class TestInputBarUIDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstructionWithProfile()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_currentProfile == AgentProfile::Write);
    }

    void testConstructionWithAskProfile()
    {
        InputBar bar(nullptr, AgentProfile::Ask);
        QVERIFY(bar.m_currentProfile == AgentProfile::Ask);
    }

    void testConstructionWithMinimalProfile()
    {
        InputBar bar(nullptr, AgentProfile::Minimal);
        QVERIFY(bar.m_currentProfile == AgentProfile::Minimal);
    }

    void testMessageEditExists()
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

    void testModelsListInitiallyEmpty()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_models.isEmpty());
    }

    void testAddModelsToCombo()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_models << "gpt-4" << "claude-3";
        QVERIFY(bar.m_models.size() == 2);
    }

    void testCurrentProfileWritable()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_currentProfile = AgentProfile::Ask;
        QVERIFY(bar.m_currentProfile == AgentProfile::Ask);
    }

    void testMinHeightSet()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.minimumHeight() > 0);
    }

    void testMaxHeightSet()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.maximumHeight() > 0);
    }

    void testSizePolicy()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.sizePolicy().horizontalPolicy() == QSizePolicy::Expanding);
    }

    void testMessageEditSizePolicy()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QVERIFY(bar.m_messageEdit->sizePolicy().horizontalPolicy() == QSizePolicy::Expanding);
    }
};

QTEST_MAIN(TestInputBarUIDetailed)
#include "test_inputbar_ui_detailed.moc"