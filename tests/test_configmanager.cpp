#include <QtTest/QtTest>
#include "../src/configmanager.h"

class TestConfigManager : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultValues()
    {
        ConfigManager manager;
        
        QCOMPARE(manager.getActiveProvider(), QString("regolo"));
        QCOMPARE(manager.getActiveModel(), QString("qwen3-coder-next"));
        QCOMPARE(manager.getMaxIterations(), 20);
        QCOMPARE(manager.getMaxTokens(), 4096);
        QVERIFY(manager.bufferContextEnabled());
    }

    void testSetterGetters()
    {
        ConfigManager manager;
        
        manager.setActiveProvider("ollama");
        QCOMPARE(manager.getActiveProvider(), QString("ollama"));
        
        manager.setActiveModel("llama3");
        QCOMPARE(manager.getActiveModel(), QString("llama3"));
        
        manager.setMaxIterations(50);
        QCOMPARE(manager.getMaxIterations(), 50);
        
        manager.setMaxTokens(8192);
        QCOMPARE(manager.getMaxTokens(), 8192);
        
        manager.setBufferContextEnabled(false);
        QVERIFY(!manager.bufferContextEnabled());
    }

    void testProviderConfig()
    {
        ConfigManager manager;
        
        ProviderConfig config = manager.getProviderConfig("regolo");
        QCOMPARE(config.name, QString("regolo"));
        QCOMPARE(config.type, QString("openai-compatible"));
        QVERIFY(config.enabled);
    }

    void testLoadSave()
    {
        ConfigManager manager;
        
        // Test that load() doesn't crash
        manager.load();
        
        // Test that save() doesn't crash
        manager.save();
    }

    void testSystemPrompt()
    {
        ConfigManager manager;
        
        QString prompt = manager.getSystemPrompt();
        manager.setSystemPrompt("Custom prompt");
        QCOMPARE(manager.getSystemPrompt(), QString("Custom prompt"));
    }
};

QTEST_MAIN(TestConfigManager)
#include "test_configmanager.moc"
