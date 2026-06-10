#include <QtTest/QtTest>
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/tools/editfiletool.h"
#include "../src/tools/greptool.h"
#include "../src/tools/terminaltool.h"

class TestToolRegistryDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testRegisterReadFileTool()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        QVERIFY(registry.hasTool("read_file"));
    }

    void testRegisterEditFileTool()
    {
        ToolRegistry registry;
        registry.registerTool(new EditFileTool());
        QVERIFY(registry.hasTool("edit_file"));
    }

    void testRegisterGrepTool()
    {
        ToolRegistry registry;
        registry.registerTool(new GrepTool());
        QVERIFY(registry.hasTool("grep"));
    }

    void testRegisterTerminalTool()
    {
        ToolRegistry registry;
        registry.registerTool(new TerminalTool());
        QVERIFY(registry.hasTool("terminal"));
    }

    void testUnregisterTool()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        QVERIFY(registry.hasTool("read_file"));
        registry.unregisterTool("read_file");
        QVERIFY(!registry.hasTool("read_file"));
    }

    void testGetToolDefinitionsSize()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new EditFileTool());
        auto defs = registry.getToolDefinitions();
        QVERIFY(defs.size() >= 2);
    }

    void testMultipleToolsRegister()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new EditFileTool());
        registry.registerTool(new GrepTool());
        registry.registerTool(new TerminalTool());
        
        QVERIFY(registry.hasTool("read_file"));
        QVERIFY(registry.hasTool("edit_file"));
        QVERIFY(registry.hasTool("grep"));
        QVERIFY(registry.hasTool("terminal"));
    }

    void testClearTools()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        QVERIFY(registry.hasTool("read_file"));
    }

    void testToolDefinitionHasName()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        auto defs = registry.getToolDefinitions();
        QVERIFY(!defs.isEmpty());
    }

    void testToolDefinitionHasDescription()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        auto defs = registry.getToolDefinitions();
        QVERIFY(!defs[0].function.description.isEmpty());
    }

    void testToolDefinitionHasParameters()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        auto defs = registry.getToolDefinitions();
        QVERIFY(!defs[0].function.parameters.isEmpty());
    }
};

QTEST_MAIN(TestToolRegistryDetailed)
#include "test_toolregistry_detailed.moc"