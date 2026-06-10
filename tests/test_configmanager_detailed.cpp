#include <QtTest/QtTest>
#include "../src/configmanager.h"

class TestConfigManagerDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testSetAndGetApiKey()
    {
        ConfigManager cm;
        cm.setApiKey("sk-test-123");
        QVERIFY(cm.apiKey() == "sk-test-123");
    }

    void testSetAndGetBaseUrl()
    {
        ConfigManager cm;
        cm.setBaseUrl("http://custom:8080/v1");
        QVERIFY(cm.baseUrl() == "http://custom:8080/v1");
    }

    void testSetAndGetModel()
    {
        ConfigManager cm;
        cm.setModel("custom-model");
        QVERIFY(cm.model() == "custom-model");
    }

    void testSetAndGetSystemPrompt()
    {
        ConfigManager cm;
        cm.setSystemPrompt("Custom system prompt");
        QVERIFY(cm.systemPrompt() == "Custom system prompt");
    }

    void testSetAndGetTemperature()
    {
        ConfigManager cm;
        cm.setTemperature(0.9);
        QVERIFY(cm.temperature() == 0.9);
    }

    void testSetAndGetMaxTokens()
    {
        ConfigManager cm;
        cm.setMaxTokens(4000);
        QVERIFY(cm.maxTokens() == 4000);
    }

    void testSetAndGetToolsEnabled()
    {
        ConfigManager cm;
        cm.setToolsEnabled(false);
        QVERIFY(cm.toolsEnabled() == false);
    }

    void testSetAndGetAutoScrollEnabled()
    {
        ConfigManager cm;
        cm.setAutoScrollEnabled(false);
        QVERIFY(cm.autoScrollEnabled() == false);
    }

    void testSetAndGetThreadSyncEnabled()
    {
        ConfigManager cm;
        cm.setThreadSyncEnabled(false);
        QVERIFY(cm.threadSyncEnabled() == false);
    }

    void testDefaultValuesNotEmpty()
    {
        ConfigManager cm;
        QVERIFY(!cm.baseUrl().isEmpty());
        QVERIFY(!cm.model().isEmpty());
        QVERIFY(!cm.systemPrompt().isEmpty());
    }

    void testMultipleConfigChanges()
    {
        ConfigManager cm;
        cm.setApiKey("key1");
        cm.setBaseUrl("url1");
        cm.setModel("model1");
        cm.setTemperature(0.5);
        cm.setMaxTokens(1000);
        cm.setToolsEnabled(false);
        
        QVERIFY(cm.apiKey() == "key1");
        QVERIFY(cm.baseUrl() == "url1");
        QVERIFY(cm.model() == "model1");
        QVERIFY(cm.temperature() == 0.5);
        QVERIFY(cm.maxTokens() == 1000);
        QVERIFY(cm.toolsEnabled() == false);
    }
};

QTEST_MAIN(TestConfigManagerDetailed)
#include "test_configmanager_detailed.moc"