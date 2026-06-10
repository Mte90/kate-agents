#include <QtTest/QtTest>
#include "../src/configmanager.h"

class TestConfigManagerUtils : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        ConfigManager cm;
        QVERIFY(true);
    }

    void testDefaultApiKeyEmpty()
    {
        ConfigManager cm;
        QVERIFY(cm.apiKey().isEmpty());
    }

    void testDefaultBaseUrl()
    {
        ConfigManager cm;
        QVERIFY(!cm.baseUrl().isEmpty());
    }

    void testSetApiKey()
    {
        ConfigManager cm;
        cm.setApiKey("test-key-123");
        QVERIFY(cm.apiKey() == "test-key-123");
    }

    void testSetBaseUrl()
    {
        ConfigManager cm;
        cm.setBaseUrl("http://localhost:8080");
        QVERIFY(cm.baseUrl() == "http://localhost:8080");
    }

    void testSetModel()
    {
        ConfigManager cm;
        cm.setModel("gpt-4");
        QVERIFY(cm.model() == "gpt-4");
    }

    void testDefaultModel()
    {
        ConfigManager cm;
        QVERIFY(!cm.model().isEmpty());
    }

    void testSystemPrompt()
    {
        ConfigManager cm;
        QVERIFY(!cm.systemPrompt().isEmpty());
    }

    void testSetSystemPrompt()
    {
        ConfigManager cm;
        cm.setSystemPrompt("Custom prompt");
        QVERIFY(cm.systemPrompt() == "Custom prompt");
    }

    void testTemperature()
    {
        ConfigManager cm;
        QVERIFY(cm.temperature() > 0.0);
    }

    void testSetTemperature()
    {
        ConfigManager cm;
        cm.setTemperature(0.5);
        QVERIFY(cm.temperature() == 0.5);
    }

    void testMaxTokens()
    {
        ConfigManager cm;
        QVERIFY(cm.maxTokens() > 0);
    }

    void testSetMaxTokens()
    {
        ConfigManager cm;
        cm.setMaxTokens(2000);
        QVERIFY(cm.maxTokens() == 2000);
    }

    void testToolsEnabled()
    {
        ConfigManager cm;
        QVERIFY(cm.toolsEnabled() == true);
    }

    void testSetToolsEnabled()
    {
        ConfigManager cm;
        cm.setToolsEnabled(false);
        QVERIFY(cm.toolsEnabled() == false);
    }

    void testAutoScrollEnabled()
    {
        ConfigManager cm;
        QVERIFY(cm.autoScrollEnabled() == true);
    }

    void testSetAutoScrollEnabled()
    {
        ConfigManager cm;
        cm.setAutoScrollEnabled(false);
        QVERIFY(cm.autoScrollEnabled() == false);
    }

    void testThreadSyncEnabled()
    {
        ConfigManager cm;
        QVERIFY(cm.threadSyncEnabled() == true);
    }

    void testSetThreadSyncEnabled()
    {
        ConfigManager cm;
        cm.setThreadSyncEnabled(false);
        QVERIFY(cm.threadSyncEnabled() == false);
    }

    void testMultipleSetOperations()
    {
        ConfigManager cm;
        cm.setApiKey("key1");
        cm.setBaseUrl("url1");
        cm.setModel("model1");
        cm.setTemperature(0.9);
        
        QVERIFY(cm.apiKey() == "key1");
        QVERIFY(cm.baseUrl() == "url1");
        QVERIFY(cm.model() == "model1");
        QVERIFY(cm.temperature() == 0.9);
    }
};

QTEST_MAIN(TestConfigManagerUtils)
#include "test_configmanager_utils.moc"