#include "configmanager.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QStandardPaths>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_activeProvider(Defaults::ActiveProvider)
    , m_activeModel(Defaults::ActiveModel)
    , m_maxIterations(Defaults::MaxIterations)
    , m_temperature(Defaults::Temperature)
    , m_maxTokens(Defaults::MaxTokens)
{
    // No hardcoded providers - user must configure their own
    m_providers.clear();
    
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
    KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
    
    // Load basic settings
    m_activeProvider = group.readEntry("ActiveProvider", Defaults::ActiveProvider);
    m_activeModel = group.readEntry("ActiveModel", Defaults::ActiveModel);
    m_maxIterations = group.readEntry("MaxIterations", Defaults::MaxIterations);
    m_temperature = group.readEntry("Temperature", Defaults::Temperature);
    m_maxTokens = group.readEntry("MaxTokens", Defaults::MaxTokens);
    m_systemPrompt = group.readEntry("SystemPrompt", m_systemPrompt);
    m_panelVisible = group.readEntry("PanelVisible", false);
    m_bufferContextEnabled = group.readEntry("BufferContextEnabled", true);
    
    // Load providers
    m_providers.clear();
    int providerCount = group.readEntry("ProviderCount", 0);
    for (int i = 0; i < providerCount; i++) {
        KConfigGroup providerGroup(KSharedConfig::openConfig(), QStringLiteral("Provider/%1").arg(i));
        ProviderConfig cfg;
        cfg.type = providerGroup.readEntry("Type", QString());
        cfg.name = providerGroup.readEntry("Name", QString());
        cfg.baseUrl = providerGroup.readEntry("BaseUrl", QString());
        cfg.apiKey = providerGroup.readEntry("ApiKey", QString());
        cfg.defaultModel = providerGroup.readEntry("DefaultModel", QString());
        cfg.enabled = providerGroup.readEntry("Enabled", false);
        m_providers.push_back(cfg);
    }
}

void ConfigManager::save()
{
    KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
    
    // Save basic settings
    group.writeEntry("ActiveProvider", m_activeProvider);
    group.writeEntry("ActiveModel", m_activeModel);
    group.writeEntry("MaxIterations", m_maxIterations);
    group.writeEntry("Temperature", m_temperature);
    group.writeEntry("MaxTokens", m_maxTokens);
    group.writeEntry("SystemPrompt", m_systemPrompt);
    group.writeEntry("PanelVisible", m_panelVisible);
    group.writeEntry("BufferContextEnabled", m_bufferContextEnabled);
    
    // Save providers
    group.writeEntry("ProviderCount", static_cast<int>(m_providers.size()));
    for (size_t i = 0; i < m_providers.size(); i++) {
        KConfigGroup providerGroup(KSharedConfig::openConfig(), QStringLiteral("Provider/%1").arg(i));
        const auto &p = m_providers[i];
        providerGroup.writeEntry("Type", p.type);
        providerGroup.writeEntry("Name", p.name);
        providerGroup.writeEntry("BaseUrl", p.baseUrl);
        providerGroup.writeEntry("ApiKey", p.apiKey);
        providerGroup.writeEntry("DefaultModel", p.defaultModel);
        providerGroup.writeEntry("Enabled", p.enabled);
    }
    
    group.sync();
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
