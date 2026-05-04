#ifndef AGENTPANEL_H
#define AGENTPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QAction>

#include "threadview.h"
#include "inputbar.h"

class AgentLoop;
class ToolRegistry;
class LLMProvider;
class ConfigManager;
class PermissionManager;

class AgentPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AgentPanel(AgentLoop *agent, ToolRegistry *registry, 
                        LLMProvider *provider, ConfigManager *config,
                        PermissionManager *permissions,
                        QWidget *parent = nullptr);
    ~AgentPanel() override;

    QAction *createAction();

public slots:
    void onSendMessage(const QString &message);
    void onStopRequested();
    void onModelChanged(const QString &model);

private slots:
    void onResponseChunk(const QString &chunk);
    void onSystemPromptChanged(const QString &prompt);
    void onToolCallStarted(const QString &toolName, const QJsonObject &args);
    void onToolCallCompleted(const QString &toolName, const QJsonObject &result);
    void onTurnCompleted();
    void onError(const QString &error);
    void onRunningChanged(bool running);
    void onPermissionRequested(const QString &toolName);

private:
    void setupUi();
    void connectSignals();
    
    AgentLoop *m_agent;
    ToolRegistry *m_registry;
    LLMProvider *m_provider;
    ConfigManager *m_config;
    PermissionManager *m_permissions;
    
    ThreadView *m_threadView;
    InputBar *m_inputBar;
    QTabWidget *m_tabs;
};

#endif
