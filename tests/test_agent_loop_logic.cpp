#include <QtTest/QtTest>
#include "../src/llmprovider.h"
#include "../src/toolregistry.h"

class TestAgentLoopLogic : public QObject
{
    Q_OBJECT

private slots:

    void testToolCallIdGeneration()
    {
        LLMMessage msg;
        msg.role = "tool";
        msg.toolCallId = "call_" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QVERIFY(msg.toolCallId.startsWith("call_"));
    }

    void testToolCallArguments()
    {
        ToolCall tc;
        tc.id = "call_1";
        tc.name = "test_tool";
        tc.arguments = QJsonObject{{"arg1", "value1"}};
        QVERIFY(tc.arguments["arg1"].toString() == "value1");
    }
};

QTEST_MAIN(TestAgentLoopLogic)
#include "test_agent_loop_logic.moc"