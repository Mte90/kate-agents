#include <QtTest/QtTest>
#include "../src/toolregistry.h"
#include "../src/tools/readfiletool.h"
#include "../src/tools/editfiletool.h"
#include "../src/tools/greptool.h"
#include "../src/tools/applydifftool.h"
#include <QFile>

class TestToolsEdgeCases : public QObject
{
    Q_OBJECT

private slots:

    void testReadFileToolEmptyPath()
    {
        ReadFileTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", ""}});
        QVERIFY(result["success"] == false);
    }

    void testReadFileToolNonexistent()
    {
        ReadFileTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", "/nonexistent/file12345.cpp"}});
        QVERIFY(result["success"] == false);
    }

    void testReadFileToolWithContent()
    {
        QString path = "/tmp/test_read_edge.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("test content");
        file.close();
        
        ReadFileTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", path}});
        
        file.remove();
    }

    void testEditFileToolEmptyPath()
    {
        EditFileTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", ""}, {"search", "old"}, {"replace", "new"}});
        QVERIFY(result["success"] == false);
    }

    void testEditFileToolEmptySearch()
    {
        EditFileTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", "/test.cpp"}, {"search", ""}, {"replace", "new"}});
        QVERIFY(result["success"] == false);
    }

    void testGrepToolEmptyPath()
    {
        GrepTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", ""}, {"pattern", "test"}});
        QVERIFY(result["success"] == false);
    }

    void testGrepToolEmptyPattern()
    {
        GrepTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", "/test"}, {"pattern", ""}});
        QVERIFY(result["success"] == false);
    }

    void testGrepToolNonexistentPath()
    {
        GrepTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", "/nonexistent/file.cpp"}, {"pattern", "test"}});
        QVERIFY(result["success"] == false);
    }

    void testApplyDiffToolEmptyPath()
    {
        ApplyDiffTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", ""}, {"diff", "diff content"}});
        QVERIFY(result["success"] == false);
    }

    void testApplyDiffToolEmptyDiff()
    {
        ApplyDiffTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", "/test.cpp"}, {"diff", ""}});
        QVERIFY(result["success"] == false);
    }

    void testApplyDiffToolNonexistentPath()
    {
        ApplyDiffTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", "/nonexistent/file.cpp"}, {"diff", "diff"}});
        QVERIFY(result["success"] == false);
    }

    void testAllToolsHaveName()
    {
        QVERIFY(ReadFileTool().name() == "read_file");
        QVERIFY(EditFileTool().name() == "edit_file");
        QVERIFY(GrepTool().name() == "grep");
        QVERIFY(ApplyDiffTool().name() == "apply_diff");
    }

    void testAllToolsHaveDescription()
    {
        QVERIFY(!ReadFileTool().description().isEmpty());
        QVERIFY(!EditFileTool().description().isEmpty());
        QVERIFY(!GrepTool().description().isEmpty());
        QVERIFY(!ApplyDiffTool().description().isEmpty());
    }

    void testAllToolsHaveParametersSchema()
    {
        QVERIFY(!ReadFileTool().parametersSchema().isEmpty());
        QVERIFY(!EditFileTool().parametersSchema().isEmpty());
        QVERIFY(!GrepTool().parametersSchema().isEmpty());
        QVERIFY(!ApplyDiffTool().parametersSchema().isEmpty());
    }
};

QTEST_MAIN(TestToolsEdgeCases)
#include "test_tools_edge_cases.moc"