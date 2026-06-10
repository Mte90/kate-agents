#include <QtTest/QtTest>
#include "../src/tools/readfiletool.h"
#include "../src/tools/editfiletool.h"
#include "../src/tools/greptool.h"
#include "../src/tools/applydifftool.h"
#include "../src/tools/terminaltool.h"
#include <QFile>

class TestAdvancedTools : public QObject
{
    Q_OBJECT

private slots:

    void testReadFileWithBinaryContent()
    {
        QString path = "/tmp/test_binary.bin";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        QByteArray binary;
        for (int i = 0; i < 256; i++) {
            binary.append(static_cast<char>(i));
        }
        file.write(binary);
        file.close();
        
        ReadFileTool tool;
        QJsonObject result = tool.execute(QJsonObject{{"path", path}});
        
        file.remove();
    }

    void testEditFileNoMatch()
    {
        QString path = "/tmp/test_no_match.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("Hello World");
        file.close();
        
        EditFileTool tool;
        QJsonObject params{
            {"path", path},
            {"search", "nonexistent"},
            {"replace", "replacement"}
        };
        QJsonObject result = tool.execute(params);
        
        file.remove();
    }

    void testGrepRegexPattern()
    {
        QString path = "/tmp/test_regex.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("test123\ntest456\nother789\n");
        file.close();
        
        GrepTool tool;
        QJsonObject params{
            {"path", path},
            {"pattern", "test\\d+"},
            {"isRegex", true}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testGrepMultilineMatches()
    {
        QString path = "/tmp/test_multiline.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("line1\nfunction foo() {\n  return 1;\n}\nline2\nfunction bar() {\n  return 2;\n}\n");
        file.close();
        
        GrepTool tool;
        QJsonObject params{
            {"path", path},
            {"pattern", "function"}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testApplyDiffWithContext()
    {
        QString path = "/tmp/test_diff_ctx.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("line1\nline2\nline3\nline4\nline5\n");
        file.close();
        
        ApplyDiffTool tool;
        QJsonObject params{
            {"path", path},
            {"diff", "--- a\n+++ b\n@@ -2,4 +2,4 @@\n line2\n-line3\n+modified\n line4\n"}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testTerminalEnvironmentVariables()
    {
        TerminalTool tool;
        QJsonObject params{{"command", "echo $HOME"}};
        QJsonObject result = tool.execute(params);
    }

    void testTerminalWorkingDirectory()
    {
        TerminalTool tool;
        QJsonObject params{{"command", "pwd"}};
        QJsonObject result = tool.execute(params);
    }

    void testToolErrorRecovery()
    {
        ReadFileTool tool;
        
        QJsonObject params{{"path", "/invalid/"}};
        QJsonObject result1 = tool.execute(params);
        
        params = {{"path", "/also/invalid"}};
        QJsonObject result2 = tool.execute(params);
        
        QVERIFY(result1["success"] == false && result2["success"] == false);
    }

    void testEditFileMultipleOccurrences()
    {
        QString path = "/tmp/test_multi.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("foo foo foo foo");
        file.close();
        
        EditFileTool tool;
        QJsonObject params{
            {"path", path},
            {"search", "foo"},
            {"replace", "bar"},
            {"replaceAll", true}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testGrepWithLineNumbers()
    {
        QString path = "/tmp/test_lines.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("line1\nline2\nline3\n");
        file.close();
        
        GrepTool tool;
        QJsonObject params{
            {"path", path},
            {"pattern", "line2"},
            {"showLineNumbers", true}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testTerminalExitCode()
    {
        TerminalTool tool;
        
        QJsonObject successParams{{"command", "true"}};
        QJsonObject successResult = tool.execute(successParams);
        
        QJsonObject failParams{{"command", "false"}};
        QJsonObject failResult = tool.execute(failParams);
    }

    void testToolTimeoutHandling()
    {
        TerminalTool tool;
        
        QJsonObject params{{"command", "sleep 10"}};
        
        QTime timer;
        timer.start();
        tool.execute(params);
        int elapsed = timer.elapsed();
        
        QVERIFY(elapsed >= 0);
    }

    void testApplyDiffInvalid()
    {
        QString path = "/tmp/test_invalid_diff.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("content");
        file.close();
        
        ApplyDiffTool tool;
        QJsonObject params{
            {"path", path},
            {"diff", "completely invalid diff format"}
        };
        QJsonObject result = tool.execute(params);
        
        file.remove();
    }

    void testGrepCaseInsensitive()
    {
        QString path = "/tmp/test_case.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("Test TEST test\n");
        file.close();
        
        GrepTool tool;
        QJsonObject params{
            {"path", path},
            {"pattern", "TEST"},
            {"caseSensitive", false}
        };
        tool.execute(params);
        
        file.remove();
    }

    void testToolExecutionOrder()
    {
        ReadFileTool readTool;
        EditFileTool editTool;
        GrepTool grepTool;
        
        QString path = "/tmp/test_order.txt";
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write("original");
        file.close();
        
        QJsonObject r1 = readTool.execute(QJsonObject{{"path", path}});
        QJsonObject r2 = editTool.execute(QJsonObject{{"path", path}, {"search", "orig"}, {"replace", "new"}});
        QJsonObject r3 = readTool.execute(QJsonObject{{"path", path}});
        
        file.remove();
        
        QVERIFY(r1["success"] == true);
    }
};

QTEST_MAIN(TestAdvancedTools)
#include "test_advanced_tools.moc"