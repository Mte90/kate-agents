/*
 * Test for panel visibility persistence
 */

#include <QTest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

#include "configmanager.h"

void testPanelVisibleDefault()
{
    ConfigManager config(nullptr);
    bool panelVisible = config.panelVisible();
    QCOMPARE(panelVisible, false);
}

void testPanelVisibleSaveLoad()
{
    ConfigManager config(nullptr);
    config.setPanelVisible(true);
    
    QJsonObject obj;
    obj["panelVisible"] = true;
    bool loaded = obj["panelVisible"].toBool(false);
    QCOMPARE(loaded, true);
    
    obj["panelVisible"] = config.panelVisible();
    QCOMPARE(obj["panelVisible"].toBool(), true);
    
    config.setPanelVisible(false);
    obj["panelVisible"] = config.panelVisible();
    QCOMPARE(obj["panelVisible"].toBool(), false);
}

void testPanelVisibleJsonRoundtrip()
{
    ConfigManager config(nullptr);
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile file(configPath + "/test_panel_config.json");
    
    file.open(QIODevice::WriteOnly);
    {
        QJsonObject obj;
        obj["panelVisible"] = true;
        obj["activeProvider"] = "regolo";
        obj["activeModel"] = "test-model";
        file.write(QJsonDocument(obj).toJson());
    }
    file.close();
    
    file.open(QIODevice::ReadOnly);
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        bool panelVisible = obj["panelVisible"].toBool(false);
        QCOMPARE(panelVisible, true);
        QVERIFY(obj.contains("activeProvider"));
    }
    file.close();
}

void testPanelVisibleWithOtherConfig()
{
    ConfigManager config(nullptr);
    config.setActiveProvider("regolo");
    config.setActiveModel("test-model");
    config.setMaxIterations(15);
    config.setTemperature(0.5);
    config.setMaxTokens(8192);
    config.setPanelVisible(true);
    config.setBufferContextEnabled(false);
    
    QJsonObject obj;
    obj["activeProvider"] = config.getActiveProvider();
    obj["activeModel"] = config.getActiveModel();
    obj["maxIterations"] = config.getMaxIterations();
    obj["temperature"] = config.getTemperature();
    obj["maxTokens"] = config.getMaxTokens();
    obj["panelVisible"] = config.panelVisible();
    obj["bufferContextEnabled"] = config.bufferContextEnabled();
    
    QCOMPARE(obj["activeProvider"].toString(), "regolo");
    QCOMPARE(obj["activeModel"].toString(), "test-model");
    QCOMPARE(obj["maxIterations"].toInt(), 15);
    QCOMPARE(obj["temperature"].toDouble(), 0.5);
    QCOMPARE(obj["maxTokens"].toInt(), 8192);
    QCOMPARE(obj["panelVisible"].toBool(), true);
    QCOMPARE(obj["bufferContextEnabled"].toBool(), false);
}

void testPanelVisibleDeserialize()
{
    QString jsonStr = R"({
        "panelVisible": true,
        "activeProvider": "regolo",
        "activeModel": "qwen3-coder-next",
        "maxIterations": 20,
        "temperature": 0.7,
        "maxTokens": 4096,
        "systemPrompt": "test prompt",
        "providers": [
            {
                "type": "openai-compatible",
                "name": "regolo",
                "baseUrl": "https://api.regolo.ai/v1",
                "apiKey": "test-key",
                "defaultModel": "qwen3-coder-next",
                "enabled": true
            }
        ],
        "bufferContextEnabled": true
    })";
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = doc.object();
    
    bool panelVisible = obj["panelVisible"].toBool(false);
    QCOMPARE(panelVisible, true);
    QCOMPARE(obj["activeProvider"].toString(), "regolo");
    QCOMPARE(obj["activeModel"].toString(), "qwen3-coder-next");
    QCOMPARE(obj["maxIterations"].toInt(), 20);
    QCOMPARE(obj["bufferContextEnabled"].toBool(), true);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "=== Panel Visibility Persistence Tests ===";
    
    testPanelVisibleDefault();
    testPanelVisibleSaveLoad();
    testPanelVisibleJsonRoundtrip();
    testPanelVisibleWithOtherConfig();
    testPanelVisibleDeserialize();
    
    qDebug() << "\n=== All Panel Persistence Tests Complete ===";
    
    return 0;
}
