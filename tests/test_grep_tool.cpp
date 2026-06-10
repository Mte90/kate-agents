#include <QtTest/QtTest>
#include "../src/tools/greptool.h"

class TestGrepTool : public QObject
{
    Q_OBJECT

private slots:

    void testGrepToolName()
    {
        GrepTool tool;
        QVERIFY(tool.name() == "grep");
    }

    void testGrepToolDescription()
    {
        GrepTool tool;
        QVERIFY(!tool.description().isEmpty());
    }

    void testGrepToolParameters()
    {
        GrepTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testGrepToolRequiresPermission()
    {
        GrepTool tool;
        QVERIFY(tool.requiresPermission() == false);
    }

    void testGrepToolExecuteEmptyPath()
    {
        GrepTool tool;
        QJsonObject args{{"path", ""}, {"pattern", "test"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }

    void testGrepToolExecuteEmptyPattern()
    {
        GrepTool tool;
        QJsonObject args{{"path", "/test"}, {"pattern", ""}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }

    void testGrepToolExecuteNotExists()
    {
        GrepTool tool;
        QJsonObject args{{"path", "/nonexistent/file"}, {"pattern", "test"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }
};

QTEST_MAIN(TestGrepTool)
#include "test_grep_tool.moc"