#include <QtTest/QtTest>
#include "../src/permissionmanager.h"
#include <QSignalSpy>

class TestPermissionManager : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultPolicyIsConfirm()
    {
        PermissionManager pm;
        QCOMPARE(pm.getDefaultPolicy(), PermissionPolicy::Confirm);
    }

    void testSetDefaultPolicyAllow()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QCOMPARE(pm.getDefaultPolicy(), PermissionPolicy::Allow);
    }

    void testSetDefaultPolicyDeny()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QCOMPARE(pm.getDefaultPolicy(), PermissionPolicy::Deny);
    }

    void testSetDefaultPolicyConfirm()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        pm.setDefaultPolicy(PermissionPolicy::Confirm);
        QCOMPARE(pm.getDefaultPolicy(), PermissionPolicy::Confirm);
    }

    void testToolPolicyOverridesDefault()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        pm.setToolPolicy("terminal", PermissionPolicy::Allow);

        QCOMPARE(pm.getToolPolicy("terminal"), PermissionPolicy::Allow);
        QCOMPARE(pm.getToolPolicy("other_tool"), PermissionPolicy::Deny);
    }

    void testGetToolPolicyUnsetTool()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QCOMPARE(pm.getToolPolicy("unset_tool"), PermissionPolicy::Allow);
    }

    void testRequiresConfirmationConfirmPolicy()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Confirm);
        QVERIFY(pm.requiresConfirmation("any_tool"));
    }

    void testRequiresConfirmationAllowPolicy()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QVERIFY(!pm.requiresConfirmation("any_tool"));
    }

    void testRequiresConfirmationDenyPolicy()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QVERIFY(!pm.requiresConfirmation("any_tool"));
    }

    void testRequiresConfirmationToolSpecific()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        pm.setToolPolicy("dangerous_tool", PermissionPolicy::Confirm);

        QVERIFY(pm.requiresConfirmation("dangerous_tool"));
        QVERIFY(!pm.requiresConfirmation("safe_tool"));
    }

    void testIsAllowedWithAllowPolicy()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QVERIFY(pm.isAllowed("any_tool"));
    }

    void testIsAllowedWithDenyPolicy()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QVERIFY(!pm.isAllowed("any_tool"));
    }

    void testIsAllowedWithConfirmPolicy()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Confirm);
        QVERIFY(!pm.isAllowed("any_tool"));
    }

    void testGrantPermission()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QVERIFY(!pm.isAllowed("tool_a"));

        pm.grantPermission("tool_a");
        QVERIFY(pm.isAllowed("tool_a"));
    }

    void testDenyPermission()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QVERIFY(pm.isAllowed("tool_a"));

        pm.denyPermission("tool_a");
        QVERIFY(!pm.isAllowed("tool_a"));
    }

    void testGrantThenDeny()
    {
        PermissionManager pm;
        pm.grantPermission("tool_a");
        QVERIFY(pm.isAllowed("tool_a"));

        pm.denyPermission("tool_a");
        QVERIFY(!pm.isAllowed("tool_a"));
    }

    void testDenyThenGrant()
    {
        PermissionManager pm;
        pm.denyPermission("tool_a");
        QVERIFY(!pm.isAllowed("tool_a"));

        pm.grantPermission("tool_a");
        QVERIFY(pm.isAllowed("tool_a"));
    }

    void testClearSessionPermissions()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);

        pm.grantPermission("tool_a");
        pm.grantPermission("tool_b");
        QVERIFY(pm.isAllowed("tool_a"));
        QVERIFY(pm.isAllowed("tool_b"));

        pm.clearSessionPermissions();
        QVERIFY(!pm.isAllowed("tool_a"));
        QVERIFY(!pm.isAllowed("tool_b"));
    }

    void testClearSessionPermissionsEmpty()
    {
        PermissionManager pm;
        pm.clearSessionPermissions();
    }

    void testMultipleToolsIndependent()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);

        pm.grantPermission("tool_a");
        QVERIFY(pm.isAllowed("tool_a"));
        QVERIFY(!pm.isAllowed("tool_b"));
    }

    void testSessionPermissionOverridesPolicy()
    {
        PermissionManager pm;
        pm.setToolPolicy("tool_a", PermissionPolicy::Deny);
        QVERIFY(!pm.isAllowed("tool_a"));

        pm.grantPermission("tool_a");
        QVERIFY(pm.isAllowed("tool_a"));
    }

    void testRequestPermissionEmitsSignal()
    {
        PermissionManager pm;
        QSignalSpy spy(&pm, &PermissionManager::permissionRequested);

        pm.requestPermission("tool_a");
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("tool_a"));
    }

    void testGrantEmitsSignal()
    {
        PermissionManager pm;
        QSignalSpy spy(&pm, &PermissionManager::permissionGranted);

        pm.grantPermission("tool_a");
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("tool_a"));
    }

    void testDenyEmitsSignal()
    {
        PermissionManager pm;
        QSignalSpy spy(&pm, &PermissionManager::permissionDenied);

        pm.denyPermission("tool_a");
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("tool_a"));
    }

    void testRequestPermissionAllowDefault()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QVERIFY(pm.requestPermission("tool_a"));
    }

    void testRequestPermissionDenyDefault()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QVERIFY(!pm.requestPermission("tool_a"));
    }

    void testRequestPermissionConfirmDefault()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Confirm);
        QVERIFY(!pm.requestPermission("tool_a"));
    }

    void testPolicyChangeAfterGrant()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        pm.grantPermission("tool_a");

        pm.setDefaultPolicy(PermissionPolicy::Deny);
        QVERIFY(pm.isAllowed("tool_a"));

        pm.clearSessionPermissions();
        QVERIFY(!pm.isAllowed("tool_a"));
    }

    void testToolPolicyOverwritten()
    {
        PermissionManager pm;
        pm.setToolPolicy("tool_a", PermissionPolicy::Allow);
        QCOMPARE(pm.getToolPolicy("tool_a"), PermissionPolicy::Allow);

        pm.setToolPolicy("tool_a", PermissionPolicy::Deny);
        QCOMPARE(pm.getToolPolicy("tool_a"), PermissionPolicy::Deny);
    }

    void testEmptyToolName()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QVERIFY(pm.isAllowed(""));
        pm.grantPermission("");
        QVERIFY(pm.isAllowed(""));
    }

    void testManyTools()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);

        for (int i = 0; i < 100; ++i) {
            pm.grantPermission(QStringLiteral("tool_%1").arg(i));
        }

        for (int i = 0; i < 100; ++i) {
            QVERIFY(pm.isAllowed(QStringLiteral("tool_%1").arg(i)));
        }
        QVERIFY(!pm.isAllowed("tool_100"));
    }
};

QTEST_MAIN(TestPermissionManager)
#include "test_permissionmanager.moc"
