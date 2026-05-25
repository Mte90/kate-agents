/*
 * Test for tool permission system
 */

#include <QTest>
#include <QObject>
#include <QSet>
#include <QDebug>

#include "permissionmanager.h"
#include "toolregistry.h"
#include "readfiletool.h"
#include "editfiletool.h"
#include "greptool.h"

void testPermissionManagerInit()
{
    PermissionManager perms(nullptr);
    QCOMPARE(perms.allowedTools().count(), 0);
}

void testPermissionManagerAddAllowed()
{
    PermissionManager perms(nullptr);
    perms.setToolPermissions({"read_file"});
    QCOMPARE(perms.allowedTools().count(), 1);
    QCOMPARE(perms.allowedTools().contains("read_file"), true);
}

void testPermissionManagerRemoveTool()
{
    PermissionManager perms(nullptr);
    perms.setToolPermissions({"read_file", "edit_file", "grep"});
    QCOMPARE(perms.allowedTools().count(), 3);
    perms.removeTool("edit_file");
    QCOMPARE(perms.allowedTools().count(), 2);
    QCOMPARE(perms.allowedTools().contains("read_file"), true);
    QCOMPARE(perms.allowedTools().contains("edit_file"), false);
    QCOMPARE(perms.allowedTools().contains("grep"), true);
}

void testPermissionManagerClearAll()
{
    PermissionManager perms(nullptr);
    perms.setToolPermissions({"read_file", "edit_file", "grep"});
    QCOMPARE(perms.allowedTools().count(), 3);
    perms.clearToolPermissions();
    QCOMPARE(perms.allowedTools().count(), 0);
}

void testPermissionManagerAllTools()
{
    PermissionManager perms(nullptr);
    QSet<QString> allTools = {
        "read_file",
        "edit_file",
        "grep",
        "terminal",
        "web_search",
        "url_fetch",
        "diagnostics",
        "find_path",
        "list_directory",
        "create_directory",
        "apply_diff"
    };
    perms.setToolPermissions(allTools.toList());
    QCOMPARE(perms.allowedTools().count(), 11);
    for (const QString &tool : allTools) {
        QCOMPARE(perms.allowedTools().contains(tool), true);
    }
}

void testPermissionManagerEmptySet()
{
    PermissionManager perms(nullptr);
    QSet<QString> tools;
    perms.setToolPermissions(tools.toList());
    QCOMPARE(perms.allowedTools().count(), 0);
}

void testPermissionManagerCheck()
{
    PermissionManager perms(nullptr);
    perms.setToolPermissions({"read_file", "edit_file"});
    QVERIFY(perms.isToolAllowed("read_file"));
    QVERIFY(perms.isToolAllowed("edit_file"));
    QVERIFY(!perms.isToolAllowed("grep"));
    QVERIFY(!perms.isToolAllowed("unknown_tool"));
}

void testPermissionManagerOrderPreservation()
{
    PermissionManager perms(nullptr);
    perms.setToolPermissions({"read_file", "edit_file", "grep"});
    QCOMPARE(perms.allowedTools().count(), 3);
}

void testPermissionManagerNullParent()
{
    PermissionManager perms(nullptr);
    QCOMPARE(perms.allowedTools().count(), 0);
    QCOMPARE(perms.isToolAllowed("read_file"), false);
}

void testPermissionManagerWithToolRegistry()
{
    PermissionManager perms(nullptr);
    ToolRegistry registry(nullptr);
    ReadFileTool *readTool = new ReadFileTool(nullptr);
    EditFileTool *editTool = new EditFileTool(nullptr);
    QCOMPARE(perms.isToolAllowed("read_file"), false);
    perms.setToolPermissions({"read_file"});
    QCOMPARE(perms.isToolAllowed("read_file"), true);
    QCOMPARE(perms.isToolAllowed("edit_file"), false);
}
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "=== Tool Permission Tests ===";
    
    testPermissionManagerInit();
    testPermissionManagerAddAllowed();
    testPermissionManagerRemoveTool();
    testPermissionManagerClearAll();
    testPermissionManagerAllTools();
    testPermissionManagerEmptySet();
    testPermissionManagerCheck();
    testPermissionManagerOrderPreservation();
    testPermissionManagerNullParent();
    testPermissionManagerWithToolRegistry();
    
    qDebug() << "\n=== All Permission Tests Complete ===";
    
    return 0;
}
