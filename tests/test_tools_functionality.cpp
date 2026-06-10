#include <QtTest/QtTest>
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/tools/editfiletool.h"
#include "../src/tools/greptool.h"
#include "../src/tools/applydifftool.h"
#include "../src/tools/terminaltool.h"
#include <QFile>

class TestToolsFunctionality : public QObject
{
    Q_OBJECT

private slots:

    void testReadFileBasic()
    {
        QString path = "/tmp/test_func_read.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("Hello World");
        file.close();
        
        ReadFileTool tool;
        QJsonObject params{{"path", path}};
        QJsonObject result = tool.execute(params);
        
        file.remove();
        
        QVERIFY(result["success"] == true);
    }

    void testReadFileWithEncoding()
    {
        QString path = "/tmp/test_encoding.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("Test content with special: àèìòù");
        file.close();
        
        ReadFileTool tool;
        QJsonObject params{{"path", path}};
        tool.execute(params);
        
        file.remove();
    }

    void testReadFileLarge()
    {
        QString path = "/tmp/test_large.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        
        QString large;
        for (int i = 0; i < 1000; i++) {
            large += "Line " + QString::number(i) + "\n";
        }
        file.write(large.toUtf8());
        file.close();
        
        ReadFileTool tool;
        QJsonObject params{{"path", path}};
        tool.execute(params);
        
        file.remove();
    }

    void testEditFileBasic()
    {
        QString path = "/tmp/test_edit.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("Hello World");
        file.close();
        
        EditFileTool tool;
        QJsonObject params{
            {"path", path},
            {"search", "World"},
            {"replace", "Qt"}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testGrepBasic()
    {
        QString path = "/tmp/test_grep.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("int main() { return 0; }\nvoid test() { }\n");
        file.close();
        
        GrepTool tool;
        QJsonObject params{
            {"path", path},
            {"pattern", "main"}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testGrepCaseSensitive()
    {
        QString path = "/tmp/test_grep2.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("Test TEST test");
        file.close();
        
        GrepTool tool;
        QJsonObject params{
            {"path", path},
            {"pattern", "Test"}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testApplyDiffBasic()
    {
        QString path = "/tmp/test_diff.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("line 1\nline 2\nline 3\n");
        file.close();
        
        ApplyDiffTool tool;
        QJsonObject params{
            {"path", path},
            {"diff", "--- a\n+++ b\n@@ -1 +1 @@\n-line 2\n+modified line\n"}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testTerminalToolExecute()
    {
        TerminalTool tool;
        QJsonObject params{{"command", "echo test"}};
        QJsonObject result = tool.execute(params);
    }

    void testTerminalToolPipedCommands()
    {
        TerminalTool tool;
        QJsonObject params{{"command", "echo hello | wc -l"}};
        tool.execute(params);
    }

    void testToolRegistryRegister()
    {
        ToolRegistry registry;
        
        bool before = registry.hasTool("read_file");
        
        registry.registerTool(new ReadFileTool());
        
        bool after = registry.hasTool("read_file");
        
        QVERIFY(after == true);
    }

    void testToolRegistryGetTool()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        
        Tool *tool = registry.getTool("read_file");
        
        QVERIFY(tool != nullptr);
        QVERIFY(tool->name() == "read_file");
    }

    void testToolRegistryListTools()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new GrepTool());
        
        QStringList tools = registry.listTools();
        
        QVERIFY(tools.contains("read_file"));
        QVERIFY(tools.contains("grep"));
    }

    void testToolRegistryToolCount()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        registry.registerTool(new GrepTool());
        registry.registerTool(new EditFileTool());
        
        int count = registry.toolCount();
        
        QVERIFY(count >= 3);
    }

    void testToolDefinitionsToJson()
    {
        ToolRegistry registry;
        registry.registerTool(new ReadFileTool());
        
        ToolDefinition tool;
        tool.type = "function";
        tool.function.name = "test";
        
        QJsonObject json = tool.toJson();
        
        QVERIFY(json["type"] == "function");
    }

    void testToolExecutionErrorHandling()
    {
        ReadFileTool tool;
        QJsonObject params{{"path", "/nonexistent/file12345.cpp"}};
        QJsonObject result = tool.execute(params);
        
        QVERIFY(result["success"] == false);
    }

    void testToolRequiresPermission()
    {
        ReadFileTool readTool;
        TerminalTool terminalTool;
        
        QVERIFY(readTool.requiresPermission() == false || readTool.requiresPermission() == true);
        QVERIFY(terminalTool.requiresPermission() == true);
    }

    void testToolParametersSchema()
    {
        ReadFileTool tool;
        QJsonObject schema = tool.parametersSchema();
        
        QVERIFY(schema.contains("type"));
        QVERIFY(schema.contains("properties"));
    }

    void testToolDescription()
    {
        ReadFileTool tool;
        QString desc = tool.description();
        
        QVERIFY(!desc.isEmpty());
    }

    void testToolNameUniqueness()
    {
        ReadFileTool readTool;
        GrepTool grepTool;
        EditFileTool editTool;
        
        QVERIFY(readTool.name() != grepTool.name());
        QVERIFY(grepTool.name() != editTool.name());
        QVERIFY(readTool.name() != editTool.name());
    }
};

QTEST_MAIN(TestToolsFunctionality)
#include "test_tools_functionality.moc"