#ifndef AGENTPANEL_H
#define AGENTPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QAction>

#include "threadview.h"
#include "inputbar.h"
#include "../threadstorage.h"

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
    void reloadModels();
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
    void onNewChat();
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);

private:
    void setupUi();
    void connectSignals();
    void createNewChatTab();
    void closeChatTab(int index);
    void updateTabTitle(int index, const QString &title);
    void saveCurrentThread();
    void loadExistingThreads();
    QString generateChatTitle(int chatNumber);
    
    AgentLoop *m_agent;
    ToolRegistry *m_registry;
    LLMProvider *m_provider;
    ConfigManager *m_config;
    PermissionManager *m_permissions;
    
    QTabWidget *m_tabs;
    InputBar *m_inputBar;
    
    ThreadStorage *m_threadStorage;
    int m_chatCounter;
    QString m_currentThreadId;
};

#endif
