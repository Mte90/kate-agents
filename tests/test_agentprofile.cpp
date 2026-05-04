#include <QtTest/QtTest>
#include "../src/agentloop.h"

class TestAgentProfile : public QObject
{
    Q_OBJECT

private slots:
    void testStringToProfile()
    {
        QCOMPARE(stringToProfile("write"), AgentProfile::Write);
        QCOMPARE(stringToProfile("ask"), AgentProfile::Ask);
        QCOMPARE(stringToProfile("minimal"), AgentProfile::Minimal);
    }

    void testProfileToString()
    {
        QCOMPARE(profileToString(AgentProfile::Write), QString("write"));
        QCOMPARE(profileToString(AgentProfile::Ask), QString("ask"));
        QCOMPARE(profileToString(AgentProfile::Minimal), QString("minimal"));
    }

    void testSystemPromptForProfile()
    {
        QString writePrompt = systemPromptForProfile(AgentProfile::Write);
        QString askPrompt = systemPromptForProfile(AgentProfile::Ask);
        QString minimalPrompt = systemPromptForProfile(AgentProfile::Minimal);
        
        QVERIFY(!writePrompt.isEmpty());
        QVERIFY(!askPrompt.isEmpty());
        QVERIFY(!minimalPrompt.isEmpty());
        
        // Each profile should have different prompts
        QVERIFY(writePrompt != askPrompt);
        QVERIFY(askPrompt != minimalPrompt);
    }

    void testInvalidProfile()
    {
        QCOMPARE(stringToProfile("invalid"), AgentProfile::Write); // Default
    }

    void testWriteProfileHasToolInstructions()
    {
        QString prompt = systemPromptForProfile(AgentProfile::Write);
        QVERIFY(prompt.contains("tool") || prompt.contains("Tool"));
    }

    void testMinimalProfileIsConcise()
    {
        QString minimalPrompt = systemPromptForProfile(AgentProfile::Minimal);
        QString writePrompt = systemPromptForProfile(AgentProfile::Write);
        
        // Minimal should be shorter
        QVERIFY(minimalPrompt.length() <= writePrompt.length());
    }
};

QTEST_MAIN(TestAgentProfile)
#include "test_agentprofile.moc"
