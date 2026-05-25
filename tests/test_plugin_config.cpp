#include <QTest>
#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>

#include "configmanager.h"

void testConfigDefaultValues()
{
    ConfigManager config(nullptr);
    
    QCOMPARE(config.getActiveProvider(), "regolo");
    QCOMPARE(config.getActiveModel(), "qwen3-coder-next");
    QCOMPARE(config.getMaxIterations(), 20);
    QCOMPARE(config.getTemperature(), 0.7);
    QCOMPARE(config.getMaxTokens(), 4096);
    QVERIFY(!config.getSystemPrompt().isEmpty());
}

void testConfigSaveLoadRoundtrip()
{
    ConfigManager config(nullptr);
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile file(configPath + "/test_roundtrip.json");
    
    file.open(QIODevice::WriteOnly);
    {
        QJsonObject obj;
        obj["activeProvider"] = "custom";
        obj["activeModel"] = "test-model";
        obj["maxIterations"] = 10;
        obj["temperature"] = 0.5;
        obj["maxTokens"] = 2048;
        file.write(QJsonDocument(obj).toJson());
    }
    file.close();
    
    file.open(QIODevice::ReadOnly);
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        
        QCOMPARE(obj["activeProvider"].toString(), "custom");
        QCOMPARE(obj["activeModel"].toString(), "test-model");
        QCOMPARE(obj["maxIterations"].toInt(), 10);
        QCOMPARE(obj["temperature"].toDouble(), 0.5);
    }
    file.close();
}

void testConfigPanelVisible()
{
    ConfigManager config(nullptr);
    
    config.setPanelVisible(true);
    QCOMPARE(config.panelVisible(), true);
    
    config.setPanelVisible(false);
    QCOMPARE(config.panelVisible(), false);
}

void testConfigMultipleChanges()
{
    ConfigManager config(nullptr);
    
    config.setActiveProvider("regolo");
    config.setActiveModel("qwen3-coder-next");
    config.setMaxIterations(15);
    config.setTemperature(0.3);
    config.setMaxTokens(8192);
    config.setPanelVisible(true);
    config.setBufferContextEnabled(true);
    
    QCOMPARE(config.getActiveProvider(), "regolo");
    QCOMPARE(config.getActiveModel(), "qwen3-coder-next");
    QCOMPARE(config.getMaxIterations(), 15);
    QCOMPARE(config.getTemperature(), 0.3);
    QCOMPARE(config.getMaxTokens(), 8192);
    QCOMPARE(config.panelVisible(), true);
    QCOMPARE(config.bufferContextEnabled(), true);
}

void testConfigProvidersList()
{
    ConfigManager config(nullptr);
    
    QCOMPARE(config.getProviders().size(), 1);
    QCOMPARE(config.getProviders().at(0).name, "regolo");
    QCOMPARE(config.getProviders().at(0).type, "openai-compatible");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug() << "=== Plugin Config Tests ===";
    
    testConfigDefaultValues();
    testConfigSaveLoadRoundtrip();
    testConfigPanelVisible();
    testConfigMultipleChanges();
    testConfigProvidersList();
    
    qDebug() << "=== All Plugin Config Tests Complete ===";
    return 0;
}
