#include "agentconfigpage.h"
#include "../kateagentplugin.h"
#include "../configmanager.h"
#include "../openaiprovider.h"
#include "../agentloop.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <algorithm>

AgentConfigPage::AgentConfigPage(QWidget *parent, KateAgentPlugin *plugin)
    : KTextEditor::ConfigPage(parent)
    , m_plugin(plugin)
    , m_networkManager(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // API Key
    {
        auto *hl = new QHBoxLayout;
        auto label = new QLabel(i18n("API Key"));
        hl->addWidget(label);
        m_apiKeyEdit = new QLineEdit(this);
        m_apiKeyEdit->setEchoMode(QLineEdit::Password);
        hl->addWidget(m_apiKeyEdit);
        layout->addLayout(hl);
    }
    
    // Base URL
    {
        auto *hl = new QHBoxLayout;
        auto label = new QLabel(i18n("Base URL"));
        hl->addWidget(label);
        m_baseUrlEdit = new QLineEdit(this);
        m_baseUrlEdit->setText("http://localhost:11434/v1");
        hl->addWidget(m_baseUrlEdit);
        layout->addLayout(hl);
    }
    
    // Model Selection
    {
        auto *hl = new QHBoxLayout;
        auto label = new QLabel(i18n("Default Model for Sidebar:"));
        hl->addWidget(label);
        m_modelComboBox = new QComboBox(this);
        m_modelComboBox->setToolTip(i18n("This model will be used by default in the chat sidebar. "
                                        "You can change it anytime in the sidebar dropdown."));
        hl->addWidget(m_modelComboBox);
        layout->addLayout(hl);
        
        // Info label
        auto *infoLabel = new QLabel(i18n("ℹ️ The model list is fetched from the OpenAI provider at the Base URL above."));
        infoLabel->setWordWrap(true);
        infoLabel->setObjectName("agentConfigInfoLabel");
        layout->addWidget(infoLabel);
    }
    
    // System Prompt
    {
        auto *hl = new QHBoxLayout;
        auto label = new QLabel(i18n("System Prompt"));
        hl->addWidget(label);
        m_systemPromptEdit = new QTextEdit(this);
        m_systemPromptEdit->setMinimumHeight(100);
        hl->addWidget(m_systemPromptEdit);
        layout->addLayout(hl);
    }

    layout->addStretch();
    
    // Info/Error label
    m_infoLabel = new QLabel(this);
    m_infoLabel->setVisible(false);
    m_infoLabel->setWordWrap(true);
    layout->addWidget(m_infoLabel);
    
    setLayout(layout);
    
    loadSettings();
    connect(m_modelComboBox, &QComboBox::currentIndexChanged, this, &AgentConfigPage::changed);
    connect(m_systemPromptEdit, &QTextEdit::textChanged, this, &AgentConfigPage::changed);
    connect(m_apiKeyEdit, &QLineEdit::textEdited, this, &AgentConfigPage::changed);
    connect(m_baseUrlEdit, &QLineEdit::textEdited, this, &AgentConfigPage::changed);
}

void AgentConfigPage::fetchModelList()
{
    if (!m_networkManager) {
        m_networkManager = new QNetworkAccessManager(this);
    }
    
    QString baseUrl = m_baseUrlEdit->text().trimmed();
    if (!baseUrl.endsWith("/v1")) {
        baseUrl += "/v1";
    }
    
    QUrl url(baseUrl + "/models");
    QNetworkRequest request(url);
    
    auto *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            m_infoLabel->setVisible(false);
            
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("data") && jsonObj["data"].isArray()) {
                    QJsonArray modelsArray = jsonObj["data"].toArray();
                    QList<QJsonValue> modelsList;
                    for (const QJsonValue &value : modelsArray) {
                        modelsList.append(value);
                    }
                    std::sort(modelsList.begin(), modelsList.end(), [](const QJsonValue &a, const QJsonValue &b) {
                        return a.toObject()["id"].toString().toLower() < b.toObject()["id"].toString().toLower();
                    });
                    
                    // Use saved model if combo box is empty (first load), otherwise keep current selection
                    QString currentModel = m_modelComboBox->currentText();
                    QString modelToSelect = m_savedModel.isEmpty() ? currentModel : m_savedModel;
                    
                    int modelSelected = -1;
                    int idx = 0;
                    for (const QJsonValue &modelValue : modelsList) {
                        QJsonObject modelObj = modelValue.toObject();
                        if (modelObj.contains("id")) {
                            QString modelName = modelObj["id"].toString();
                            m_modelComboBox->addItem(modelName);
                            if (modelName == modelToSelect) {
                                modelSelected = idx;
                            }
                            idx++;
                        }
                    }
                    
                    if (modelSelected >= 0) {
                        m_modelComboBox->setCurrentIndex(modelSelected);
                    }
                }
            }
        } else {
            m_infoLabel->setText(i18n("Error fetching model list: %1", reply->errorString()));
            m_infoLabel->setVisible(true);
        }
        reply->deleteLater();
    });
    
    m_infoLabel->setText(i18n("Loading model list..."));
    m_infoLabel->setVisible(true);
}

QString AgentConfigPage::name() const
{
    QString result = i18n("Kate Agent");
    return result;
}

QString AgentConfigPage::fullName() const
{
    QString result = i18n("Kate Agent Settings");
    return result;
}

QIcon AgentConfigPage::icon() const
{
    QIcon result = QIcon::fromTheme(QStringLiteral("preferences-system"));
    return result;
}

void AgentConfigPage::apply()
{
    KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
    group.writeEntry("ApiKey", m_apiKeyEdit->text());
    group.writeEntry("BaseUrl", m_baseUrlEdit->text());
    group.writeEntry("Model", m_modelComboBox->currentText());
    group.writeEntry("SystemPrompt", m_systemPromptEdit->toPlainText());
    group.sync();

    // Reload config and update provider
    if (m_plugin && m_plugin->m_config && m_plugin->m_provider) {
        m_plugin->m_config->load();
        auto providerCfg = m_plugin->m_config->getProviderConfig(m_plugin->m_config->getActiveProvider());
        
        if (auto *openaiProvider = qobject_cast<OpenAIProvider*>(m_plugin->m_provider)) {
            openaiProvider->setBaseUrl(providerCfg.baseUrl);
            openaiProvider->setApiKey(providerCfg.apiKey);
        }
        
        emit m_plugin->settingsChanged();
    }
}

void AgentConfigPage::defaults()
{
    m_apiKeyEdit->setText("");
    m_baseUrlEdit->setText("http://localhost:11434/v1");
    if (m_modelComboBox->count() > 0) {
        m_modelComboBox->setCurrentIndex(0);
    }
    m_systemPromptEdit->setPlainText("You are a helpful coding assistant. Provide concise, accurate code and explanations.");
}

void AgentConfigPage::reset()
{
    loadSettings();
}

void AgentConfigPage::loadSettings()
{
    KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
    
    QString apiKey = group.readEntry("ApiKey", QString());
    QString baseUrl = group.readEntry("BaseUrl", "http://localhost:11434/v1");
    m_savedModel = group.readEntry("Model", QString());  // Salva il modello da selezionare dopo
    QString systemPrompt = group.readEntry("SystemPrompt", QString());

    if (systemPrompt.isEmpty()) {
        defaults();
    } else {
        m_apiKeyEdit->setText(apiKey);
        m_baseUrlEdit->setText(baseUrl);
        m_systemPromptEdit->setPlainText(systemPrompt);
    }
    
    fetchModelList();
}

void AgentConfigPage::changed()
{
    updateSettings();
    
    // Apply changes to provider immediately
    if (m_plugin && m_plugin->m_config && m_plugin->m_provider) {
        m_plugin->m_config->load();
        auto providerCfg = m_plugin->m_config->getProviderConfig(m_plugin->m_config->getActiveProvider());
        
        if (auto *openaiProvider = qobject_cast<OpenAIProvider*>(m_plugin->m_provider)) {
            openaiProvider->setBaseUrl(providerCfg.baseUrl);
            openaiProvider->setApiKey(providerCfg.apiKey);
        }
    }
}

void AgentConfigPage::updateSettings()
{
    // Emitting the base class signal for config changes
    KTextEditor::ConfigPage::changed();
}
