#include <QtTest/QtTest>
#include "../src/configmanager.h"
#include "../src/permissionmanager.h"

class TestConfigIntegration : public QObject
{
    Q_OBJECT

private slots:

    void testConfigAndPermissionIntegration()
    {
        ConfigManager cm;
        PermissionManager pm;
        
        cm.setBufferContextEnabled(true);
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        
        QVERIFY(cm.bufferContextEnabled() == true);
        QVERIFY(pm.getDefaultPolicy() == PermissionPolicy::Allow);
    }

    void testConfigTemperatureAndMaxTokens()
    {
        ConfigManager cm;
        cm.setTemperature(0.8);
        cm.setMaxTokens(3000);
        
        QVERIFY(cm.getTemperature() == 0.8);
        QVERIFY(cm.getMaxTokens() == 3000);
    }

    void testConfigSystemPromptMultiple()
    {
        ConfigManager cm;
        cm.setSystemPrompt("Prompt 1");
        cm.setSystemPrompt("Prompt 2");
        cm.setSystemPrompt("Prompt 3");
        
        QVERIFY(cm.getSystemPrompt() == "Prompt 3");
    }


    void testMultipleToolsWithPermissions()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Deny);
        
        pm.setToolPolicy("terminal", PermissionPolicy::Allow);
        pm.setToolPolicy("read_file", PermissionPolicy::Allow);
        
        QVERIFY(pm.getToolPolicy("terminal") == PermissionPolicy::Allow);
        QVERIFY(pm.getToolPolicy("read_file") == PermissionPolicy::Allow);
        QVERIFY(pm.getToolPolicy("unknown") == PermissionPolicy::Deny);
    }

    void testConfigActiveProviderAndModel()
    {
        ConfigManager cm;
        cm.setActiveProvider("test-provider");
        cm.setActiveModel("test-model");
        
        QVERIFY(cm.getActiveProvider() == "test-provider");
        QVERIFY(cm.getActiveModel() == "test-model");
    }

    void testConfigPanelVisibleAndBufferContext()
    {
        ConfigManager cm;
        cm.setPanelVisible(true);
        cm.setBufferContextEnabled(true);
        
        QVERIFY(cm.panelVisible() == true);
        QVERIFY(cm.bufferContextEnabled() == true);
    }

    void testDefaultConfigValues()
    {
        ConfigManager cm;
        
        QVERIFY(cm.getActiveProvider().isEmpty());
        QVERIFY(cm.getActiveModel().isEmpty());
        QVERIFY(cm.getTemperature() > 0);
        QVERIFY(cm.getMaxTokens() > 0);
    }

    void testPermissionDenyOverridesAllow()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        pm.setToolPolicy("terminal", PermissionPolicy::Deny);
        
        QVERIFY(pm.getToolPolicy("terminal") == PermissionPolicy::Deny);
    }
};

QTEST_MAIN(TestConfigIntegration)
#include "test_config_integration.moc"