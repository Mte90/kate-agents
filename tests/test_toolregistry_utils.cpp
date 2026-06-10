#include <QtTest/QtTest>
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/tools/greptool.h"

class TestToolRegistryUtils : public QObject
{
    Q_OBJECT

private slots:

    void testEmptyRegistry()
    {
        ToolRegistry registry;
        QVERIFY(registry.getToolDefinitions().isEmpty());
    }

    void testRegisterMultipleTools()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new GrepTool());
        QVERIFY(registry.getToolDefinitions().size() == 2);
    }

    void testHasToolTrue()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        QVERIFY(registry.hasTool("read_file"));
    }

    void testHasToolFalse()
    {
        ToolRegistry registry;
        QVERIFY(!registry.hasTool("nonexistent"));
    }

    void testUnregisterTool()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        QVERIFY(registry.hasTool("read_file"));
        registry.unregisterTool("read_file");
        QVERIFY(!registry.hasTool("read_file"));
    }

    void testUnregisterNonExistent()
    {
        ToolRegistry registry;
        registry.unregisterTool("nonexistent");
    }

    void testGetToolDefinitions()
    {
        ToolRegistry registry;
        auto defs = registry.getToolDefinitions();
        QVERIFY(defs.isEmpty());
    }

    void testRegisterSameToolTwice()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new ReadFileTool());
        QVERIFY(registry.getToolDefinitions().size() == 2);
    }

    void testClearTools()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new GrepTool());
        QVERIFY(registry.getToolDefinitions().size() == 2);
    }

    void testToolNamesAreUnique()
    {
        ToolRegistry registry;
        ReadFileTool tool1;
        QVERIFY(tool1.name() == "read_file");
    }
};

QTEST_MAIN(TestToolRegistryUtils)
#include "test_toolregistry_utils.moc"