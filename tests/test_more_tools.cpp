#include <QtTest/QtTest>
#include "../src/tools/applydifftool.h"
#include "../src/tools/findpathtool.h"
#include "../src/tools/editfiletool.h"
#include <QFile>

class TestMoreTools : public QObject
{
    Q_OBJECT

private slots:

    void testApplyDiffToolName()
    {
        ApplyDiffTool tool;
        QVERIFY(tool.name() == "apply_diff");
    }

    void testApplyDiffToolDescription()
    {
        ApplyDiffTool tool;
        QVERIFY(!tool.description().isEmpty());
    }

    void testApplyDiffToolParameters()
    {
        ApplyDiffTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
        QVERIFY(schema["properties"].toObject().contains("path"));
        QVERIFY(schema["properties"].toObject().contains("diff"));
    }

    void testApplyDiffToolRequiresPermission()
    {
        ApplyDiffTool tool;
        QVERIFY(tool.requiresPermission() == true);
    }

    void testApplyDiffToolExecuteEmptyPath()
    {
        ApplyDiffTool tool;
        QJsonObject args{{"path", ""}, {"diff", "test"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }

    void testApplyDiffToolExecuteEmptyDiff()
    {
        ApplyDiffTool tool;
        QJsonObject args{{"path", "/test.cpp"}, {"diff", ""}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
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

    void testEditFileToolName()
    {
        EditFileTool tool;
        QVERIFY(tool.name() == "edit_file");
    }

    void testEditFileToolParameters()
    {
        EditFileTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testEditFileToolRequiresPermission()
    {
        EditFileTool tool;
        QVERIFY(tool.requiresPermission() == true);
    }

    void testEditFileToolExecuteNotExists()
    {
        EditFileTool tool;
        QJsonObject args{
            {"path", "/nonexistent/file.cpp"},
            {"search", "old"},
            {"replace", "new"}
        };
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }

    void testEditFileToolExecute()
    {
        QString testFile = "/tmp/test_edit_file.txt";
        QFile file(testFile);
        file.open(QIODevice::WriteOnly);
        file.write("hello world");
        file.close();
        
        EditFileTool tool;
        QJsonObject args{
            {"path", testFile},
            {"search", "hello"},
            {"replace", "goodbye"}
        };
        QJsonObject result = tool.execute(args);
        
        file.remove();
    }
};

QTEST_MAIN(TestMoreTools)
#include "test_more_tools.moc"