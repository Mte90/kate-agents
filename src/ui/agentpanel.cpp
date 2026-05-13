#include "agentpanel.h"
#include "../agentloop.h"
#include "../toolregistry.h"
#include "../llmprovider.h"
#include "../configmanager.h"
#include "../permissionmanager.h"
#include <KLocalizedString>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QDebug>
#include <QJsonObject>

AgentPanel::AgentPanel(AgentLoop *agent, ToolRegistry *registry,
                       LLMProvider *provider, ConfigManager *config,
                       PermissionManager *permissions, QWidget *parent)
    : QWidget(parent)
    , m_agent(agent)
    , m_registry(registry)
    , m_provider(provider)
    , m_config(config)
    , m_permissions(permissions)
{
    setupUi();
    connectSignals();
}

AgentPanel::~AgentPanel() = default;

void AgentPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_tabs = new QTabWidget(this);
    m_tabs->setDocumentMode(true);
    
    m_threadView = new ThreadView(m_tabs);
    m_tabs->addTab(m_threadView, i18n("Chat"));
    
    mainLayout->addWidget(m_tabs, 1);
    
    m_inputBar = new InputBar(this);
    mainLayout->addWidget(m_inputBar);
    
    // Pass AgentLoop reference to InputBar for Tab key handling
    if (m_agent) {
        m_inputBar->setAgentLoop(m_agent);
    }
    
    // Load models from provider
    if (m_provider) {
        QStringList models = m_provider->availableModels();
        if (!models.isEmpty()) {
            m_inputBar->setModels(models);
            
            // Load default model from settings
            KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
            QString defaultModel = group.readEntry("Model", QString());
            
            if (!defaultModel.isEmpty() && models.contains(defaultModel)) {
                m_inputBar->setCurrentModel(defaultModel);
            }
        }
    }
}

void AgentPanel::connectSignals()
{
    connect(m_inputBar, &InputBar::sendMessage, this, &AgentPanel::onSendMessage);
    connect(m_inputBar, &InputBar::stopRequested, this, &AgentPanel::onStopRequested);
    connect(m_inputBar, &InputBar::modelChanged, this, &AgentPanel::onModelChanged);
    connect(m_inputBar, &InputBar::systemPromptChanged, this, &AgentPanel::onSystemPromptChanged);
    
    if (m_agent) {
        connect(m_agent, &AgentLoop::responseChunk, this, &AgentPanel::onResponseChunk);
        connect(m_agent, &AgentLoop::toolCallStarted, this, &AgentPanel::onToolCallStarted);
        connect(m_agent, &AgentLoop::toolCallCompleted, this, &AgentPanel::onToolCallCompleted);
        connect(m_agent, &AgentLoop::turnCompleted, this, &AgentPanel::onTurnCompleted);
        connect(m_agent, &AgentLoop::error, this, &AgentPanel::onError);
        connect(m_agent, &AgentLoop::runningChanged, this, &AgentPanel::onRunningChanged);
    }
    
    if (m_permissions) {
        connect(m_permissions, &PermissionManager::permissionRequested, 
                this, &AgentPanel::onPermissionRequested);
    }
}

QAction *AgentPanel::createAction()
{
    QAction *action = new QAction(i18n("Kate Agent"), this);
    return action;
}

void AgentPanel::onSendMessage(const QString &message)
{
    if (!m_agent || message.trimmed().isEmpty()) return;
    
    static QString currentThreadId = "default_thread";
    
    m_agent->addUserMessage(currentThreadId, message);
    m_agent->executeTurn(currentThreadId);
    
    m_inputBar->clear();
    
    qDebug() << "AgentPanel: Sending message to agent:" << message;
}

void AgentPanel::onStopRequested()
{
    if (m_agent) {
        m_agent->abort();
    }
}

void AgentPanel::onModelChanged(const QString &model)
{
    m_config->setActiveModel(model);
    qDebug() << "AgentPanel: Model changed to:" << model;
}

void AgentPanel::onSystemPromptChanged(const QString &prompt)
{
    if (m_agent) {
        m_agent->setSystemPrompt(prompt);
    }
}

void AgentPanel::onResponseChunk(const QString &chunk)
{
    m_threadView->showStreamingChunk(chunk);
}

void AgentPanel::onToolCallStarted(const QString &toolName, const QJsonObject &args)
{
    m_threadView->appendToolCall(toolName, args);
}

void AgentPanel::onToolCallCompleted(const QString &toolName, const QJsonObject &result)
{
    m_threadView->appendToolResult(toolName, result);
}

void AgentPanel::onTurnCompleted()
{
    m_threadView->appendHtml("<hr>");
}

void AgentPanel::onError(const QString &error)
{
    m_threadView->appendHtml(QString("<div class='tool-result error'>%1</div>").arg(i18n("Error: %1").arg(error)));
}

void AgentPanel::onRunningChanged(bool running)
{
    m_inputBar->setRunningState(running);
}

void AgentPanel::onPermissionRequested(const QString &toolName)
{
    qDebug() << "AgentPanel: Permission requested for:" << toolName;
}

void AgentPanel::reloadModels()
{
    qDebug() << "AgentPanel: Reloading models from provider";
    if (m_provider) {
        QStringList models = m_provider->availableModels();
        if (!models.isEmpty()) {
            m_inputBar->setModels(models);
            
            // Reload default model from settings
            KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
            QString defaultModel = group.readEntry("Model", QString());
            
            if (!defaultModel.isEmpty() && models.contains(defaultModel)) {
                m_inputBar->setCurrentModel(defaultModel);
            }
        }
    }
}
