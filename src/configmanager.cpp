#include "configmanager.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_activeProvider("regolo")
    , m_activeModel("qwen3-coder-next")
    , m_maxIterations(20)
    , m_temperature(0.7)
    , m_maxTokens(4096)
{
    std::vector<ProviderConfig> providers;
    ProviderConfig regolo;
    regolo.type = "openai-compatible";
    regolo.name = "regolo";
    regolo.baseUrl = "https://api.regolo.ai/v1";
    regolo.apiKey = "";
    regolo.defaultModel = "qwen3-coder-next";
    regolo.enabled = true;
    providers.push_back(regolo);
    m_providers = providers;
    
    m_systemPrompt = R"(
You are an AI coding assistant integrated in the Kate text editor for KDE.
You can read, search, and modify files in the user's project.

IMPORTANT RULES:
1. Always use tools to read files before editing them - never guess file contents.
2. When editing, use edit_file with the EXACT text to replace. Include enough context for a unique match.
3. After making changes, run diagnostics to verify no errors were introduced.
4. For multi-file changes, read all affected files first, then edit them one by one.
5. When running terminal commands, prefer short commands with clear output.
6. If a tool call fails, analyze the error and try a different approach.
7. Be concise in your responses. Focus on the actual changes needed.
8. Always explain what you're about to do before doing it.
)";
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::load()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);
    QFile file(configPath + "/config.json");

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonObject obj = doc.object();
        m_activeProvider = obj["activeProvider"].toString("regolo");
        m_activeModel = obj["activeModel"].toString("qwen3-coder-next");
        m_maxIterations = obj["maxIterations"].toInt(20);
        m_temperature = obj["temperature"].toDouble(0.7);
        m_maxTokens = obj["maxTokens"].toInt(4096);
        m_systemPrompt = obj["systemPrompt"].toString(m_systemPrompt);
        
        // Load panel visibility state
        m_panelVisible = obj["panelVisible"].toBool(false);

        m_providers.clear();
        QJsonArray providers = obj["providers"].toArray();
        for (const QJsonValue &val : providers) {
            QJsonObject p = val.toObject();
            ProviderConfig cfg;
            cfg.type = p["type"].toString();
            cfg.name = p["name"].toString();
            cfg.baseUrl = p["baseUrl"].toString();
            cfg.apiKey = p["apiKey"].toString();
            cfg.defaultModel = p["defaultModel"].toString();
            cfg.enabled = p["enabled"].toBool(false);
            m_providers.push_back(cfg);
        }
    }

    // Override with KDE config (kateagentrc) if present
    KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
    QString kdeApiKey = group.readEntry("ApiKey", QString());
    QString kdeBaseUrl = group.readEntry("BaseUrl", QString());
    QString kdeModel = group.readEntry("Model", QString());
    QString kdeSystemPrompt = group.readEntry("SystemPrompt", QString());
    m_bufferContextEnabled = group.readEntry("BufferContextEnabled", true);

    if (!kdeBaseUrl.isEmpty()) {
        for (auto &p : m_providers) {
            if (p.name == m_activeProvider) {
                if (!kdeBaseUrl.isEmpty()) p.baseUrl = kdeBaseUrl;
                if (!kdeApiKey.isEmpty()) p.apiKey = kdeApiKey;
                if (!kdeModel.isEmpty()) p.defaultModel = kdeModel;
                break;
            }
        }
        if (!kdeModel.isEmpty()) m_activeModel = kdeModel;
    }
    if (!kdeSystemPrompt.isEmpty()) m_systemPrompt = kdeSystemPrompt;
}

void ConfigManager::save()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configPath);
    QFile file(configPath + "/config.json");
    
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    
    QJsonObject obj;
    obj["activeProvider"] = m_activeProvider;
    obj["activeModel"] = m_activeModel;
    obj["maxIterations"] = m_maxIterations;
    obj["temperature"] = m_temperature;
    obj["maxTokens"] = m_maxTokens;
    obj["systemPrompt"] = m_systemPrompt;
    obj["panelVisible"] = m_panelVisible;
    
    QJsonArray providers;
    for (const ProviderConfig &p : m_providers) {
        QJsonObject pobj;
        pobj["type"] = p.type;
        pobj["name"] = p.name;
        pobj["baseUrl"] = p.baseUrl;
        pobj["apiKey"] = p.apiKey;
        pobj["defaultModel"] = p.defaultModel;
        pobj["enabled"] = p.enabled;
        providers.append(pobj);
    }
    obj["providers"] = providers;
    
    file.write(QJsonDocument(obj).toJson());
    file.close();
}

ProviderConfig ConfigManager::getProviderConfig(const QString &name) const
{
    for (const ProviderConfig &p : m_providers) {
        if (p.name == name) {
            return p;
        }
    }
    return ProviderConfig();
}

void ConfigManager::setProviderConfig(const QString &name, const ProviderConfig &config)
{
    for (size_t i = 0; i < m_providers.size(); i++) {
        if (m_providers[i].name == name) {
            m_providers[i] = config;
            return;
        }
    }
    m_providers.push_back(config);
}
