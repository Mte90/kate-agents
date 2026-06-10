#include <QtTest/QtTest>
#include "../src/configmanager.h"
#include "../src/permissionmanager.h"

class TestToolsConfigPermission : public QObject
{
    Q_OBJECT

private slots:

    void testConfigManagerTemperatureRange()
    {
        ConfigManager cm;
        cm.setTemperature(1.5);
        QVERIFY(cm.temperature() > 1.0);
    }

    void testConfigManagerToolsDefault()
    {
        ConfigManager cm;
        QVERIFY(cm.toolsEnabled() == true);
    }

    void testPermissionManagerToolPolicy()
    {
        PermissionManager pm;
        pm.setToolPolicy("test_tool", PermissionPolicy::Deny);
        QVERIFY(pm.getToolPolicy("test_tool") == PermissionPolicy::Deny);
    }

    void testPermissionManagerDefaultDeny()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QVERIFY(pm.getDefaultPolicy() == PermissionPolicy::Deny);
    }
};

QTEST_MAIN(TestToolsConfigPermission)
#include "test_tools_config_permission.moc"