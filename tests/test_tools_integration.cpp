#include <QtTest/QtTest>
#include "../src/llmprovider.h"
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/tools/greptool.h"

class TestToolsIntegration : public QObject
{
    Q_OBJECT

private slots:

    void testRegistryWithMultipleTools()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new GrepTool());
        QVERIFY(registry.getToolDefinitions().size() >= 2);
    }

    void testGetAllMessagesEmpty()
    {
        ToolRegistry registry;
        auto defs = registry.getToolDefinitions();
        int initial = defs.size();
        
        registry.registerTool(new ReadFileTool());
        QVERIFY(registry.getToolDefinitions().size() > initial);
    }
};

QTEST_MAIN(TestToolsIntegration)
#include "test_tools_integration.moc"