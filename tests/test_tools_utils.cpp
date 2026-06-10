#include <QtTest/QtTest>
#include "../src/tools/editfiletool.h"
#include "../src/tools/applydifftool.h"

class TestToolsExecute : public QObject
{
    Q_OBJECT

private slots:

    void testEditFileToolExecuteInvalidPath()
    {
        EditFileTool tool;
        QJsonObject args{{"path", ""}, {"search", "old"}, {"replace", "new"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }

    void testEditFileToolExecuteEmptySearch()
    {
        EditFileTool tool;
        QJsonObject args{{"path", "/test.cpp"}, {"search", ""}, {"replace", "new"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }

    void testApplyDiffToolRequiresPermission()
    {
        ApplyDiffTool tool;
        QVERIFY(tool.requiresPermission() == true);
    }

    void testApplyDiffToolParametersHaveRequired()
    {
        ApplyDiffTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("required"));
    }

    void testApplyDiffToolExecuteInvalidPath()
    {
        ApplyDiffTool tool;
        QJsonObject args{{"path", ""}, {"diff", "diff content"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
        QVERIFY(result.contains("error"));
    }
};

QTEST_MAIN(TestToolsExecute)
#include "test_tools_utils.moc"