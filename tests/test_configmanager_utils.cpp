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

    void testDefaultActiveProviderEmpty()
    {
        ConfigManager cm;
        QVERIFY(cm.getActiveProvider().isEmpty());
    }

    void testDefaultActiveModelEmpty()
    {
        ConfigManager cm;
        QVERIFY(cm.getActiveModel().isEmpty());
    }

    void testSetActiveProvider()
    {
        ConfigManager cm;
        cm.setActiveProvider("test-provider-123");
        QVERIFY(cm.getActiveProvider() == "test-provider-123");
    }

    void testSetActiveModel()
    {
        ConfigManager cm;
        cm.setActiveModel("gpt-4");
        QVERIFY(cm.getActiveModel() == "gpt-4");
    }

    void testDefaultModelEmpty()
    {
        ConfigManager cm;
        QVERIFY(cm.getActiveModel().isEmpty());
    }

    void testSystemPromptEmptyByDefault()
    {
        ConfigManager cm;
        QVERIFY(cm.getSystemPrompt().isEmpty());
    }

    void testSetSystemPrompt()
    {
        ConfigManager cm;
        cm.setSystemPrompt("Custom prompt");
        QVERIFY(cm.getSystemPrompt() == "Custom prompt");
    }

    void testTemperature()
    {
        ConfigManager cm;
        QVERIFY(cm.getTemperature() > 0.0);
    }

    void testSetTemperature()
    {
        ConfigManager cm;
        cm.setTemperature(0.5);
        QVERIFY(cm.getTemperature() == 0.5);
    }

    void testMaxTokens()
    {
        ConfigManager cm;
        QVERIFY(cm.getMaxTokens() > 0);
    }

    void testSetMaxTokens()
    {
        ConfigManager cm;
        cm.setMaxTokens(2000);
        QVERIFY(cm.getMaxTokens() == 2000);
    }

    void testBufferContextEnabled()
    {
        ConfigManager cm;
        QVERIFY(cm.bufferContextEnabled() == true);
    }

    void testSetBufferContextEnabled()
    {
        ConfigManager cm;
        cm.setBufferContextEnabled(false);
        QVERIFY(cm.bufferContextEnabled() == false);
    }

    void testPanelVisible()
    {
        ConfigManager cm;
        QVERIFY(cm.panelVisible() == false);
    }

    void testSetPanelVisible()
    {
        ConfigManager cm;
        cm.setPanelVisible(true);
        QVERIFY(cm.panelVisible() == true);
    }

    void testMultipleSetOperations()
    {
        ConfigManager cm;
        cm.setActiveProvider("provider1");
        cm.setActiveModel("model1");
        cm.setTemperature(0.9);
        
        QVERIFY(cm.getActiveProvider() == "provider1");
        QVERIFY(cm.getActiveModel() == "model1");
        QVERIFY(cm.getTemperature() == 0.9);
    }
};

QTEST_MAIN(TestConfigManagerUtils)
#include "test_configmanager_utils.moc"