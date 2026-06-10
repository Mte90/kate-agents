#include <QtTest/QtTest>
#include "../src/tools/urlfetchtool.h"
#include "../src/tools/diagnosticstool.h"
#include "../src/tools/createdirectorytool.h"
#include "../src/tools/listdirectorytool.h"

class TestURLFetchDiagnosticTools : public QObject
{
    Q_OBJECT

private slots:

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

    void testListDirectoryToolParameters()
    {
        ListDirectoryTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }
};

QTEST_MAIN(TestURLFetchDiagnosticTools)
#include "test_urlfetch_diagnostic_tools.moc"