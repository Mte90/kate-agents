#include <QtTest/QtTest>
#include "../src/tools/terminaltool.h"
#include "../src/tools/websearchtool.h"

class TestTerminalWebSearchTools : public QObject
{
    Q_OBJECT

private slots:

    void testTerminalToolName()
    {
        TerminalTool tool;
        QVERIFY(tool.name() == "terminal");
    }

    void testTerminalToolParameters()
    {
        TerminalTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testTerminalToolRequiresPermission()
    {
        TerminalTool tool;
        QVERIFY(tool.requiresPermission() == true);
    }

    void testWebSearchToolName()
    {
        WebSearchTool tool;
        QVERIFY(tool.name() == "web_search");
    }

    void testWebSearchToolParameters()
    {
        WebSearchTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testWebSearchToolRequiresPermission()
    {
        WebSearchTool tool;
        QVERIFY(tool.requiresPermission() == false);
    }

    void testWebSearchToolExecute()
    {
        WebSearchTool tool;
        QJsonObject args{{"query", "test query"}};
        QJsonObject result = tool.execute(args);
    }

    void testWebSearchToolDescription()
    {
        WebSearchTool tool;
        QVERIFY(!tool.description().isEmpty());
    }

    void testTerminalToolDescription()
    {
        TerminalTool tool;
        QVERIFY(!tool.description().isEmpty());
    }
};

QTEST_MAIN(TestTerminalWebSearchTools)
#include "test_terminal_websearch_tools.moc"