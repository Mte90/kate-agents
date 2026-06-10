#include <QtTest/QtTest>
#include "../src/permissionmanager.h"

class TestPermissionManagerDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testDefaultPolicyIsConfirm()
    {
        PermissionManager pm;
        QVERIFY(pm.getDefaultPolicy() == PermissionPolicy::Confirm);
    }

    void testSetDefaultPolicyToAllow()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QVERIFY(pm.getDefaultPolicy() == PermissionPolicy::Allow);
    }

    void testSetDefaultPolicyToDeny()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QVERIFY(pm.getDefaultPolicy() == PermissionPolicy::Deny);
    }

    void testSetDefaultPolicyToConfirm()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        pm.setDefaultPolicy(PermissionPolicy::Confirm);
        QVERIFY(pm.getDefaultPolicy() == PermissionPolicy::Confirm);
    }

    void testSetToolPolicyToAllow()
    {
        PermissionManager pm;
        pm.setToolPolicy("terminal", PermissionPolicy::Allow);
        QVERIFY(pm.getToolPolicy("terminal") == PermissionPolicy::Allow);
    }

    void testSetToolPolicyToDeny()
    {
        PermissionManager pm;
        pm.setToolPolicy("read_file", PermissionPolicy::Deny);
        QVERIFY(pm.getToolPolicy("read_file") == PermissionPolicy::Deny);
    }

    void testToolPolicyOverridesDefault()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        pm.setToolPolicy("terminal", PermissionPolicy::Allow);
        QVERIFY(pm.getToolPolicy("terminal") == PermissionPolicy::Allow);
    }

    void testUnsetToolFallsBackToDefault()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        pm.setToolPolicy("tool1", PermissionPolicy::Deny);
        QVERIFY(pm.getToolPolicy("tool1") == PermissionPolicy::Deny);
    }

    void testMultipleToolPolicies()
    {
        PermissionManager pm;
        pm.setToolPolicy("read_file", PermissionPolicy::Allow);
        pm.setToolPolicy("edit_file", PermissionPolicy::Deny);
        pm.setToolPolicy("terminal", PermissionPolicy::Confirm);
        
        QVERIFY(pm.getToolPolicy("read_file") == PermissionPolicy::Allow);
        QVERIFY(pm.getToolPolicy("edit_file") == PermissionPolicy::Deny);
        QVERIFY(pm.getToolPolicy("terminal") == PermissionPolicy::Confirm);
    }
};

QTEST_MAIN(TestPermissionManagerDetailed)
#include "test_permissionmanager_detailed.moc"