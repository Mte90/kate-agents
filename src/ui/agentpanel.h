#ifndef AGENTPANEL_H
#define AGENTPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QTabBar>
#include <QAction>
#include <QMouseEvent>

#include "threadview.h"
#include "inputbar.h"
#include "../threadstorage.h"

class AgentLoop;
class ToolRegistry;
class LLMProvider;
class ConfigManager;
class PermissionManager;
class InputBar;
class ThreadView;
class ConversationThread;

// Custom TabBar to handle middle-click (wheel click) to close tabs
class CloseableTabBar : public QTabBar {
    Q_OBJECT
public:
    explicit CloseableTabBar(QWidget *parent = nullptr) : QTabBar(parent) {}

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::MiddleButton) {
            int index = tabAt(event->pos());
            if (index >= 0) {
                emit middleTabClicked(index);
            }
        }
        QTabBar::mousePressEvent(event);
    }

signals:
    void middleTabClicked(int index);
};

// Custom TabWidget that uses CloseableTabBar
class CloseableTabWidget : public QTabWidget {
    Q_OBJECT
public:
    explicit CloseableTabWidget(QWidget *parent = nullptr) : QTabWidget(parent) {}

signals:
    void middleTabClicked(int index);

protected:
    void mousePressEvent(QMouseEvent *event) override {
        // Delegate to tab bar for middle-click handling
        if (event->button() == Qt::MiddleButton) {
            auto *bar = tabBar();
            if (bar) {
                // Map the position to the tab bar's coordinate system
                QPoint barPos = bar->mapFromParent(event->pos());
                int index = bar->tabAt(barPos);
                if (index >= 0) {
                    emit middleTabClicked(index);
                    event->accept();
                    return;
                }
            }
        }
        QTabWidget::mousePressEvent(event);
    }
};

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
    void onThreadUpdated(const QString &threadId);
    void onError(const QString &error);
    void onRunningChanged(bool running);
    void onPermissionRequested(const QString &toolName);
    void onNewChat();
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);
    void updateModelFromSettings();
    void renameChatTab(int index);

private:
    // Methods
    void setupUi();
    void connectSignals();
    void createNewChatTab();
    void closeChatTab(int index);
    void updateTabTitle(int index, const QString &title);
    void saveCurrentThread();
    void loadExistingThreads();
    QString generateChatTitle(int chatNumber);
    
    // Member variables (order must match constructor initialization list)
    AgentLoop *m_agent;
    ToolRegistry *m_registry;
    LLMProvider *m_provider;
    ConfigManager *m_config;
    PermissionManager *m_permissions;
    CloseableTabWidget *m_tabs = nullptr;
    InputBar *m_inputBar;
    ThreadStorage *m_threadStorage;
    int m_chatCounter;
    QString m_activeThreadId;  // Thread currently receiving responses
    QString m_currentThreadId;
    bool m_hasUnsavedChanges = false;  // Track if current thread has unsaved changes
};

#endif
