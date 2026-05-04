#include <QtTest/QtTest>
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/tools/greptool.h"
#include <QJsonObject>

class TestToolRegistry : public QObject
{
    Q_OBJECT

private slots:
    void testRegisterTool()
    {
        ToolRegistry registry;
        
        auto *tool = new ReadFileTool();
        registry.registerTool(tool);
        
        QVERIFY(registry.hasTool("read_file"));
        QCOMPARE(registry.getToolDefinitions().size(), 1);
    }

    void testMultipleTools()
    {
        ToolRegistry registry;
        
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new GrepTool());
        
        QCOMPARE(registry.getToolDefinitions().size(), 2);
        QVERIFY(registry.hasTool("read_file"));
        QVERIFY(registry.hasTool("grep"));
    }

    void testUnregisterTool()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        
        QVERIFY(registry.hasTool("read_file"));
        registry.unregisterTool("read_file");
        QVERIFY(!registry.hasTool("read_file"));
    }
};

QTEST_MAIN(TestToolRegistry)
#include "test_toolregistry.moc"
