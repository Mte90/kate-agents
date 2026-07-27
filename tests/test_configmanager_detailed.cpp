#include <QtTest/QtTest>
#include "../src/configmanager.h"

class TestConfigManagerDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testSetAndGetActiveProvider()
    {
        ConfigManager cm;
        cm.setActiveProvider("test-provider");
        QVERIFY(cm.getActiveProvider() == "test-provider");
    }

    void testSetAndGetActiveModel()
    {
        ConfigManager cm;
        cm.setActiveModel("custom-model");
        QVERIFY(cm.getActiveModel() == "custom-model");
    }

    void testSetAndGetSystemPrompt()
    {
        ConfigManager cm;
        cm.setSystemPrompt("Custom system prompt");
        QVERIFY(cm.getSystemPrompt() == "Custom system prompt");
    }

    void testSetAndGetTemperature()
    {
        ConfigManager cm;
        cm.setTemperature(0.9);
        QVERIFY(cm.getTemperature() == 0.9);
    }

    void testSetAndGetMaxTokens()
    {
        ConfigManager cm;
        cm.setMaxTokens(4000);
        QVERIFY(cm.getMaxTokens() == 4000);
    }

    void testSetAndGetBufferContextEnabled()
    {
        ConfigManager cm;
        cm.setBufferContextEnabled(false);
        QVERIFY(cm.bufferContextEnabled() == false);
    }

    void testSetAndGetPanelVisible()
    {
        ConfigManager cm;
        cm.setPanelVisible(false);
        QVERIFY(cm.panelVisible() == false);
    }

    void testDefaultValues()
    {
        ConfigManager cm;
        QVERIFY(cm.getActiveProvider().isEmpty());
        QVERIFY(cm.getActiveModel().isEmpty());
        QVERIFY(cm.getSystemPrompt().isEmpty());
    }

    void testMultipleConfigChanges()
    {
        ConfigManager cm;
        cm.setActiveProvider("provider1");
        cm.setActiveModel("model1");
        cm.setTemperature(0.5);
        cm.setMaxTokens(1000);
        cm.setBufferContextEnabled(false);
        
        QVERIFY(cm.getActiveProvider() == "provider1");
        QVERIFY(cm.getActiveModel() == "model1");
        QVERIFY(cm.getTemperature() == 0.5);
        QVERIFY(cm.getMaxTokens() == 1000);
        QVERIFY(cm.bufferContextEnabled() == false);
    }
};

QTEST_MAIN(TestConfigManagerDetailed)
#include "test_configmanager_detailed.moc"