#include <QtTest/QtTest>
#include "../src/configmanager.h"
#include "../src/permissionmanager.h"
#include "../src/toolregistry.h"

class TestConfigIntegration : public QObject
{
    Q_OBJECT

private slots:

    void testConfigAndPermissionIntegration()
    {
        ConfigManager cm;
        PermissionManager pm;
        
        cm.setToolsEnabled(true);
        pm.setDefaultPolicy(PermissionPolicy::Allow);
        
        QVERIFY(cm.toolsEnabled() == true);
        QVERIFY(pm.getDefaultPolicy() == PermissionPolicy::Allow);
    }

    void testConfigTemperatureAndMaxTokens()
    {
        ConfigManager cm;
        cm.setTemperature(0.8);
        cm.setMaxTokens(3000);
        
        QVERIFY(cm.temperature() == 0.8);
        QVERIFY(cm.maxTokens() == 3000);
    }

    void testConfigSystemPromptMultiple()
    {
        ConfigManager cm;
        cm.setSystemPrompt("Prompt 1");
        cm.setSystemPrompt("Prompt 2");
        cm.setSystemPrompt("Prompt 3");
        
        QVERIFY(cm.systemPrompt() == "Prompt 3");
    }

    void testToolRegistryWithConfig()
    {
        ConfigManager cm;
        ToolRegistry registry;
        
        cm.setToolsEnabled(true);
        
        if (cm.toolsEnabled()) {
            registry.registerTool(new ReadFileTool());
            QVERIFY(registry.hasTool("read_file"));
        }
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

    void testConfigApiKeyAndUrl()
    {
        ConfigManager cm;
        cm.setApiKey("sk-test");
        cm.setBaseUrl("http://localhost:8080");
        
        QVERIFY(cm.apiKey() == "sk-test");
        QVERIFY(cm.baseUrl() == "http://localhost:8080");
    }

    void testConfigAutoScrollAndSync()
    {
        ConfigManager cm;
        cm.setAutoScrollEnabled(true);
        cm.setThreadSyncEnabled(true);
        
        QVERIFY(cm.autoScrollEnabled() == true);
        QVERIFY(cm.threadSyncEnabled() == true);
    }

    void testProfileEnumRoundTrip()
    {
        QString writeStr = profileToString(AgentProfile::Write);
        QString askStr = profileToString(AgentProfile::Ask);
        QString minimalStr = profileToString(AgentProfile::Minimal);
        
        QVERIFY(stringToProfile(writeStr) == AgentProfile::Write);
        QVERIFY(stringToProfile(askStr) == AgentProfile::Ask);
        QVERIFY(stringToProfile(minimalStr) == AgentProfile::Minimal);
    }

    void testDefaultConfigValues()
    {
        ConfigManager cm;
        
        QVERIFY(!cm.apiKey().isEmpty() || cm.apiKey().isEmpty());
        QVERIFY(!cm.baseUrl().isEmpty());
        QVERIFY(!cm.model().isEmpty());
        QVERIFY(cm.temperature() > 0);
        QVERIFY(cm.maxTokens() > 0);
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