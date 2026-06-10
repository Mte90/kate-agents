#include <QtTest/QtTest>
#include "../src/toolregistry.h"
#include "../src/tools/terminaltool.h"
#include "../src/tools/websearchtool.h"
#include "../src/tools/urlfetchtool.h"
#include "../src/tools/diagnosticstool.h"
#include "../src/tools/findpathtool.h"
#include "../src/tools/createdirectorytool.h"
#include "../src/tools/listdirectorytool.h"

class TestToolsComprehensive : public QObject
{
    Q_OBJECT

private slots:

    void testTerminalToolName()
    {
        TerminalTool tool;
        QVERIFY(tool.name() == "terminal");
    }

    void testTerminalToolDescription()
    {
        TerminalTool tool;
        QVERIFY(!tool.description().isEmpty());
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

    void testWebSearchToolDescription()
    {
        WebSearchTool tool;
        QVERIFY(!tool.description().isEmpty());
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

    void testURLFetchToolName()
    {
        URLFetchTool tool;
        QVERIFY(tool.name() == "url_fetch");
    }

    void testURLFetchToolParameters()
    {
        URLFetchTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testURLFetchToolRequiresPermission()
    {
        URLFetchTool tool;
        QVERIFY(tool.requiresPermission() == false);
    }

    void testDiagnosticsToolName()
    {
        DiagnosticsTool tool;
        QVERIFY(tool.name() == "diagnostics");
    }

    void testDiagnosticsToolParameters()
    {
        DiagnosticsTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testDiagnosticsToolRequiresPermission()
    {
        DiagnosticsTool tool;
        QVERIFY(tool.requiresPermission() == false);
    }

    void testFindPathToolName()
    {
        FindPathTool tool;
        QVERIFY(tool.name() == "find_path");
    }

    void testFindPathToolParameters()
    {
        FindPathTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testFindPathToolRequiresPermission()
    {
        FindPathTool tool;
        QVERIFY(tool.requiresPermission() == false);
    }

    void testCreateDirectoryToolName()
    {
        CreateDirectoryTool tool;
        QVERIFY(tool.name() == "create_directory");
    }

    void testCreateDirectoryToolParameters()
    {
        CreateDirectoryTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testCreateDirectoryToolRequiresPermission()
    {
        CreateDirectoryTool tool;
        QVERIFY(tool.requiresPermission() == true);
    }

    void testListDirectoryToolName()
    {
        ListDirectoryTool tool;
        QVERIFY(tool.name() == "list_directory");
    }

    void testListDirectoryToolParameters()
    {
        ListDirectoryTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testListDirectoryToolRequiresPermission()
    {
        ListDirectoryTool tool;
        QVERIFY(tool.requiresPermission() == false);
    }

    void testAllToolsHaveUniqueNames()
    {
        QSet<QString> names;
        names.insert(TerminalTool().name());
        names.insert(WebSearchTool().name());
        names.insert(URLFetchTool().name());
        names.insert(DiagnosticsTool().name());
        names.insert(FindPathTool().name());
        names.insert(CreateDirectoryTool().name());
        names.insert(ListDirectoryTool().name());
        QVERIFY(names.size() == 7);
    }
};

QTEST_MAIN(TestToolsComprehensive)
#include "test_tools_comprehensive.moc"