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
        auto label = new QLabel(i18n("Model"));
        hl->addWidget(label);
        m_modelComboBox = new QComboBox(this);
        hl->addWidget(m_modelComboBox);
        layout->addLayout(hl);
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

    // Buffer Context
    m_bufferContextCheckbox = new QCheckBox(i18n("Include editor context in prompts"), this);
    layout->addWidget(m_bufferContextCheckbox);
    connect(m_bufferContextCheckbox, &QCheckBox::checkStateChanged, this, &AgentConfigPage::changed);

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
                    
                    QString currentModel = m_modelComboBox->currentText();
                    m_modelComboBox->clear();
                    
                    int modelSelected = -1;
                    int idx = 0;
                    for (const QJsonValue &modelValue : modelsList) {
                        QJsonObject modelObj = modelValue.toObject();
                        if (modelObj.contains("id")) {
                            QString modelName = modelObj["id"].toString();
                            m_modelComboBox->addItem(modelName);
                            if (modelName == currentModel) {
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
    return i18n("Kate Agent");
}

QString AgentConfigPage::fullName() const
{
    return i18n("Kate Agent Settings");
}

QIcon AgentConfigPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("preferences-system"));
}

void AgentConfigPage::apply()
{
    KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
    group.writeEntry("ApiKey", m_apiKeyEdit->text());
    group.writeEntry("BaseUrl", m_baseUrlEdit->text());
    group.writeEntry("Model", m_modelComboBox->currentText());
    group.writeEntry("SystemPrompt", m_systemPromptEdit->toPlainText());
    group.writeEntry("BufferContextEnabled", m_bufferContextCheckbox->isChecked());
    group.sync();

    if (m_plugin) {
        m_plugin->m_config->load();
        auto providerCfg = m_plugin->m_config->getProviderConfig(m_plugin->m_config->getActiveProvider());
        delete m_plugin->m_provider;
        m_plugin->m_provider = new OpenAIProvider(providerCfg.baseUrl, providerCfg.apiKey, m_plugin);
        m_plugin->m_agentLoop->setProvider(m_plugin->m_provider);
    }
}

void AgentConfigPage::defaults()
{
    m_apiKeyEdit->setText("");
    m_baseUrlEdit->setText("http://localhost:11434/v1");
    m_modelComboBox->setCurrentIndex(0);
    m_systemPromptEdit->setPlainText("You are a helpful coding assistant. Provide concise, accurate code and explanations.");
    m_bufferContextCheckbox->setChecked(true);
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
    QString model = group.readEntry("Model", QString());
    QString systemPrompt = group.readEntry("SystemPrompt", QString());
    bool bufferContextEnabled = group.readEntry("BufferContextEnabled", true);

    if (systemPrompt.isEmpty()) {
        defaults();
    } else {
        m_apiKeyEdit->setText(apiKey);
        m_baseUrlEdit->setText(baseUrl);
        m_systemPromptEdit->setPlainText(systemPrompt);
    }
    m_bufferContextCheckbox->setChecked(bufferContextEnabled);
    
    fetchModelList();
}

void AgentConfigPage::changed()
{
    updateSettings();
}

void AgentConfigPage::updateSettings()
{
    // Emitting the base class signal for config changes
    KTextEditor::ConfigPage::changed();
}

#include "agentconfigpage.moc"