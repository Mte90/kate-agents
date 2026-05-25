#include <QtTest/QtTest>
#include "../src/configmanager.h"
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class TestConfigManagerAdvanced : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultConstructorValues()
    {
        ConfigManager mgr;
        QCOMPARE(mgr.getActiveProvider(), QStringLiteral("regolo"));
        QCOMPARE(mgr.getActiveModel(), QStringLiteral("qwen3-coder-next"));
        QCOMPARE(mgr.getMaxIterations(), 20);
        QCOMPARE(mgr.getMaxTokens(), 4096);
        QCOMPARE(mgr.getTemperature(), 0.7);
        QVERIFY(mgr.bufferContextEnabled());
        QVERIFY(!mgr.panelVisible());
    }

    void testDefaultSystemPrompt()
    {
        ConfigManager mgr;
        QString prompt = mgr.getSystemPrompt();
        QVERIFY(!prompt.isEmpty());
        QVERIFY(prompt.contains("Kate", Qt::CaseInsensitive));
    }

    void testDefaultProviderConfig()
    {
        ConfigManager mgr;
        ProviderConfig cfg = mgr.getProviderConfig("regolo");
        QCOMPARE(cfg.name, QStringLiteral("regolo"));
        QCOMPARE(cfg.type, QStringLiteral("openai-compatible"));
        QVERIFY(cfg.enabled);
    }

    void testSetGetActiveProvider()
    {
        ConfigManager mgr;
        mgr.setActiveProvider("ollama");
        QCOMPARE(mgr.getActiveProvider(), QStringLiteral("ollama"));
        mgr.setActiveProvider("");
        QCOMPARE(mgr.getActiveProvider(), QString());
    }

    void testSetGetActiveModel()
    {
        ConfigManager mgr;
        mgr.setActiveModel("llama3");
        QCOMPARE(mgr.getActiveModel(), QStringLiteral("llama3"));
    }

    void testSetGetMaxIterationsEdgeValues()
    {
        ConfigManager mgr;
        mgr.setMaxIterations(0);
        QCOMPARE(mgr.getMaxIterations(), 0);
        mgr.setMaxIterations(-1);
        QCOMPARE(mgr.getMaxIterations(), -1);
        mgr.setMaxIterations(10000);
        QCOMPARE(mgr.getMaxIterations(), 10000);
    }

    void testSetGetTemperatureEdgeValues()
    {
        ConfigManager mgr;
        mgr.setTemperature(0.0);
        QCOMPARE(mgr.getTemperature(), 0.0);
        mgr.setTemperature(1.0);
        QCOMPARE(mgr.getTemperature(), 1.0);
        mgr.setTemperature(-0.5);
        QCOMPARE(mgr.getTemperature(), -0.5);
        mgr.setTemperature(2.5);
        QCOMPARE(mgr.getTemperature(), 2.5);
    }

    void testSetGetMaxTokensEdgeValues()
    {
        ConfigManager mgr;
        mgr.setMaxTokens(0);
        QCOMPARE(mgr.getMaxTokens(), 0);
        mgr.setMaxTokens(100000);
        QCOMPARE(mgr.getMaxTokens(), 100000);
    }

    void testSetGetSystemPromptEmpty()
    {
        ConfigManager mgr;
        mgr.setSystemPrompt("");
        QCOMPARE(mgr.getSystemPrompt(), QString());
    }

    void testSetGetSystemPromptLong()
    {
        ConfigManager mgr;
        QString longPrompt(10000, QChar('a'));
        mgr.setSystemPrompt(longPrompt);
        QCOMPARE(mgr.getSystemPrompt(), longPrompt);
    }

    void testSetGetSystemPromptUnicode()
    {
        ConfigManager mgr;
        QString unicode = QStringLiteral("こんにちは世界 🌍 Hello");
        mgr.setSystemPrompt(unicode);
        QCOMPARE(mgr.getSystemPrompt(), unicode);
    }

    void testSetGetBufferContextEnabled()
    {
        ConfigManager mgr;
        QVERIFY(mgr.bufferContextEnabled());
        mgr.setBufferContextEnabled(false);
        QVERIFY(!mgr.bufferContextEnabled());
    }

    void testSetGetPanelVisible()
    {
        ConfigManager mgr;
        QVERIFY(!mgr.panelVisible());
        mgr.setPanelVisible(true);
        QVERIFY(mgr.panelVisible());
    }

    void testGetProviderConfigExisting()
    {
        ConfigManager mgr;
        ProviderConfig cfg = mgr.getProviderConfig("regolo");
        QCOMPARE(cfg.name, QStringLiteral("regolo"));
        QCOMPARE(cfg.baseUrl, QStringLiteral("https://api.regolo.ai/v1"));
    }

    void testGetProviderConfigNonExistent()
    {
        ConfigManager mgr;
        ProviderConfig cfg = mgr.getProviderConfig("does_not_exist");
        QVERIFY(cfg.name.isEmpty());
        QVERIFY(cfg.type.isEmpty());
        QVERIFY(!cfg.enabled);
    }

    void testSetProviderConfigUpdateExisting()
    {
        ConfigManager mgr;
        ProviderConfig cfg = mgr.getProviderConfig("regolo");
        cfg.baseUrl = "http://localhost:11434/v1";
        mgr.setProviderConfig("regolo", cfg);

        ProviderConfig updated = mgr.getProviderConfig("regolo");
        QCOMPARE(updated.baseUrl, QStringLiteral("http://localhost:11434/v1"));
    }

    void testSetProviderConfigAddNew()
    {
        ConfigManager mgr;
        ProviderConfig cfg;
        cfg.name = "ollama";
        cfg.type = "openai-compatible";
        cfg.baseUrl = "http://localhost:11434/v1";
        cfg.enabled = true;
        mgr.setProviderConfig("ollama", cfg);

        ProviderConfig retrieved = mgr.getProviderConfig("ollama");
        QCOMPARE(retrieved.name, QStringLiteral("ollama"));
        QCOMPARE(retrieved.baseUrl, QStringLiteral("http://localhost:11434/v1"));
    }

    void testSetMultipleProviders()
    {
        ConfigManager mgr;
        std::vector<ProviderConfig> providers;
        for (int i = 0; i < 5; ++i) {
            ProviderConfig cfg;
            cfg.name = QStringLiteral("provider_%1").arg(i);
            cfg.enabled = true;
            providers.push_back(cfg);
        }
        mgr.setProviders(providers);
        QCOMPARE(static_cast<int>(mgr.getProviders().size()), 5);
    }

    void testLoadWithoutConfigFile()
    {
        ConfigManager mgr;
        mgr.load();
        QCOMPARE(mgr.getActiveProvider(), QStringLiteral("regolo"));
    }

    void testSaveCreatesDirectory()
    {
        ConfigManager mgr;
        mgr.save();
    }

    void testSaveAndLoad()
    {
        ConfigManager mgr1;
        mgr1.setActiveProvider("test-provider");
        mgr1.setActiveModel("test-model");
        mgr1.setMaxIterations(42);
        mgr1.setTemperature(0.3);
        mgr1.setMaxTokens(8192);
        mgr1.setSystemPrompt("test prompt");
        mgr1.save();

        ConfigManager mgr2;
        mgr2.load();
        QCOMPARE(mgr2.getActiveProvider(), QStringLiteral("test-provider"));
        QCOMPARE(mgr2.getActiveModel(), QStringLiteral("test-model"));
        QCOMPARE(mgr2.getMaxIterations(), 42);
        QCOMPARE(mgr2.getMaxTokens(), 8192);

        mgr1.setActiveProvider("regolo");
        mgr1.setActiveModel("qwen3-coder-next");
        mgr1.setMaxIterations(20);
        mgr1.setMaxTokens(4096);
        mgr1.setSystemPrompt("");
        mgr1.save();
    }

    void testLoadMalformedJSON()
    {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(configPath);

        QString backupPath = configPath + "/config.json.bak";
        QString configFilePath = configPath + "/config.json";

        if (QFile::exists(configFilePath)) {
            QFile::rename(configFilePath, backupPath);
        }

        QFile file(configFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("THIS IS NOT JSON {{{}}}");
            file.close();
        }

        ConfigManager mgr;
        mgr.load();
        QCOMPARE(mgr.getActiveProvider(), QStringLiteral("regolo"));

        if (QFile::exists(backupPath)) {
            QFile::remove(configFilePath);
            QFile::rename(backupPath, configFilePath);
        }
    }

    void testLoadEmptyJSONObject()
    {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(configPath);

        QString backupPath = configPath + "/config.json.bak2";
        QString configFilePath = configPath + "/config.json";

        if (QFile::exists(configFilePath)) {
            QFile::rename(configFilePath, backupPath);
        }

        QFile file(configFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("{}");
            file.close();
        }

        ConfigManager mgr;
        mgr.load();
        QCOMPARE(mgr.getActiveProvider(), QStringLiteral("regolo"));
        QCOMPARE(mgr.getMaxIterations(), 20);

        if (QFile::exists(backupPath)) {
            QFile::remove(configFilePath);
            QFile::rename(backupPath, configFilePath);
        }
    }

    void testLoadPartialJSON()
    {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(configPath);

        QString backupPath = configPath + "/config.json.bak3";
        QString configFilePath = configPath + "/config.json";

        if (QFile::exists(configFilePath)) {
            QFile::rename(configFilePath, backupPath);
        }

        QJsonObject partial;
        partial["activeProvider"] = "partial-provider";
        QFile file(configFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(partial).toJson());
            file.close();
        }

        ConfigManager mgr;
        mgr.load();
        QCOMPARE(mgr.getActiveProvider(), QStringLiteral("partial-provider"));
        QCOMPARE(mgr.getActiveModel(), QStringLiteral("qwen3-coder-next"));
        QCOMPARE(mgr.getMaxIterations(), 20);

        if (QFile::exists(backupPath)) {
            QFile::remove(configFilePath);
            QFile::rename(backupPath, configFilePath);
        }
    }

    void testLoadProvidersEmptyArray()
    {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(configPath);

        QString backupPath = configPath + "/config.json.bak4";
        QString configFilePath = configPath + "/config.json";

        if (QFile::exists(configFilePath)) {
            QFile::rename(configFilePath, backupPath);
        }

        QJsonObject obj;
        obj["providers"] = QJsonArray();
        QFile file(configFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(obj).toJson());
            file.close();
        }

        ConfigManager mgr;
        mgr.load();
        QCOMPARE(static_cast<int>(mgr.getProviders().size()), 0);

        if (QFile::exists(backupPath)) {
            QFile::remove(configFilePath);
            QFile::rename(backupPath, configFilePath);
        }
    }

    void testConfigChangedSignalExists()
    {
        ConfigManager mgr;
        QSignalSpy spy(&mgr, &ConfigManager::configChanged);
        QVERIFY(spy.isValid());
    }

    void testDoubleSave()
    {
        ConfigManager mgr;
        mgr.save();
        mgr.save();
    }
};

QTEST_MAIN(TestConfigManagerAdvanced)
#include "test_configmanager_advanced.moc"
