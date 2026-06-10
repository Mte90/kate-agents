#include <QtTest/QtTest>
#include "../src/tools/readfiletool.h"
#include "../src/tools/greptool.h"
#include "../src/tools/listdirectorytool.h"
#include "../src/tools/editfiletool.h"
#include "../src/tools/findpathtool.h"
#include <QDir>
#include <QFile>

class TestTools : public QObject
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
        QVERIFY(schema["properties"].toObject().contains("path"));
    }

    void testReadFileToolExecuteNotExists()
    {
        ReadFileTool tool;
        QJsonObject args{{"path", "/nonexistent/file.cpp"}};
        QJsonObject result = tool.execute(args);
        QVERIFY(result["success"] == false);
        QVERIFY(result.contains("error"));
    }

    void testReadFileToolExecuteExists()
    {
        QString testFile = "/tmp/test_read_file.txt";
        QFile file(testFile);
        file.open(QIODevice::WriteOnly);
        file.write("line1\nline2\nline3");
        file.close();
        
        ReadFileTool tool;
        QJsonObject args{{"path", testFile}};
        QJsonObject result = tool.execute(args);
        
        QVERIFY(result["success"] == true);
        QVERIFY(result["content"].toString().contains("line1"));
        QVERIFY(result["lines"].toInt() == 3);
        
        file.remove();
    }

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

    void testListDirectoryToolName()
    {
        ListDirectoryTool tool;
        QVERIFY(tool.name() == "list_directory");
    }

    void testListDirectoryToolDescription()
    {
        ListDirectoryTool tool;
        QVERIFY(!tool.description().isEmpty());
    }

    void testToolsRequirePermission()
    {
        ReadFileTool readTool;
        QVERIFY(readTool.requiresPermission() == false);
        
        GrepTool grepTool;
        QVERIFY(grepTool.requiresPermission() == false);
    }

    void testEditFileToolName()
    {
        EditFileTool tool;
        QVERIFY(tool.name() == "edit_file");
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

    void testGrepToolExecute()
    {
        QString testFile = "/tmp/test_grep.txt";
        QFile file(testFile);
        file.open(QIODevice::WriteOnly);
        file.write("hello world\nhello again\ngoodbye");
        file.close();
        
        GrepTool tool;
        QJsonObject args{
            {"path", testFile},
            {"pattern", "hello"}
        };
        QJsonObject result = tool.execute(args);
        
        QVERIFY(result["success"] == true);
        
        file.remove();
    }
};

QTEST_MAIN(TestTools)
#include "test_tools.moc"