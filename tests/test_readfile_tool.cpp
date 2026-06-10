#include <QtTest/QtTest>
#include "../src/tools/readfiletool.h"

class TestReadFileTool : public QObject
{
    Q_OBJECT

private slots:

    void testReadFileToolName()
    {
        ReadFileTool tool;
        QVERIFY(tool.name() == "read_file");
    }

    void testReadFileToolDescription()
    {
        ReadFileTool tool;
        QVERIFY(!tool.description().isEmpty());
    }

    void testReadFileToolParameters()
    {
        ReadFileTool tool;
        QJsonObject schema = tool.parametersSchema();
        QVERIFY(schema.contains("properties"));
    }

    void testReadFileToolRequiresPermission()
    {
        ReadFileTool tool;
        QVERIFY(tool.requiresPermission() == false);
    }

    void testReadFileToolExecuteEmptyPath()
    {
        ReadFileTool tool;
        QJsonObject args{{"path", ""}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }

    void testReadFileToolExecuteNotExists()
    {
        ReadFileTool tool;
        QJsonObject args{{"path", "/nonexistent/file.txt"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
    }
};

QTEST_MAIN(TestReadFileTool)
#include "test_readfile_tool.moc"