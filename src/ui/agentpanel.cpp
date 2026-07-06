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
#include <QTimer>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QDate>
#include <QRegularExpression>
#include <QMessageBox>

AgentPanel::AgentPanel(AgentLoop *agent, ToolRegistry *registry,
                       LLMProvider *provider, ConfigManager *config,
                       PermissionManager *permissions, QWidget *parent)
    : QWidget(parent)
    , m_agent(agent)
    , m_registry(registry)
    , m_provider(provider)
    , m_config(config)
    , m_permissions(permissions)
    , m_tabs(nullptr)
    , m_inputBar(nullptr)
    , m_chatCounter(0)
    , m_currentThreadId()
{
    setupUi();
    
    connectSignals();
    
    loadExistingThreads();
}

AgentPanel::~AgentPanel() = default;

void AgentPanel::cacheTabs()
{
    m_tabHash.clear();
    for (int i = 0; i < m_tabs->count(); ++i) {
        m_tabHash.insert(m_tabs->tabToolTip(i), i);
    }
}

void AgentPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_tabs = new CloseableTabWidget(this);
    m_tabs->setDocumentMode(true);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Connect middle-click to close tab
    connect(m_tabs, &CloseableTabWidget::middleTabClicked, this, [this](int index) {
        closeChatTab(index);
    });
    
    // Connect tab double-click for renaming
    connect(m_tabs->tabBar(), &QTabBar::tabBarDoubleClicked, this, [this](int index) {
        renameChatTab(index);
    });
    
    createNewChatTab();
    
    QWidget *newChatWidget = new QWidget();
    newChatWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *newChatLayout = new QHBoxLayout(newChatWidget);
    newChatLayout->setContentsMargins(4, 2, 4, 2);
    newChatLayout->setSpacing(0);
    
    QPushButton *newChatBtn = new QPushButton("+");
    newChatBtn->setFixedSize(24, 24);
    newChatBtn->setToolTip(i18n("New Chat"));
    newChatBtn->setObjectName("agentNewChatButton");
    connect(newChatBtn, &QPushButton::clicked, this, &AgentPanel::onNewChat);
    newChatLayout->addStretch();
    
    newChatLayout->addWidget(newChatBtn);
    
    mainLayout->addWidget(newChatWidget);
    mainLayout->addWidget(m_tabs, 1);
    
    m_inputBar = new InputBar(this);
    mainLayout->addWidget(m_inputBar, 0);
    
    if (m_agent) {
        m_inputBar->setAgentLoop(m_agent);
    }
    
    if (m_provider) {
        QStringList models = m_provider->availableModels();
        if (!models.isEmpty()) {
            m_inputBar->setModels(models);
            
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
    if (!m_inputBar || !m_tabs) {
        // AgentPanel: Cannot connect signals - m_inputBar or m_tabs is null
        return;
    }
    
    connect(m_inputBar, &InputBar::sendMessage, this, &AgentPanel::onSendMessage);
    connect(m_inputBar, &InputBar::stopRequested, this, &AgentPanel::onStopRequested);
    connect(m_inputBar, &InputBar::modelChanged, this, &AgentPanel::onModelChanged);
    connect(m_inputBar, &InputBar::systemPromptChanged, this, &AgentPanel::onSystemPromptChanged);
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &AgentPanel::onTabCloseRequested);
    connect(m_tabs, &QTabWidget::currentChanged, this, &AgentPanel::onCurrentTabChanged);
    
    if (m_agent) {
        connect(m_agent, &AgentLoop::responseStarted, this, &AgentPanel::onResponseStarted);
        connect(m_agent, &AgentLoop::chunkReceived, this, &AgentPanel::onChunkReceived);
        connect(m_agent, &AgentLoop::toolCallStarted, this, &AgentPanel::onToolCallStarted);
        connect(m_agent, &AgentLoop::toolCallCompleted, this, &AgentPanel::onToolCallCompleted);
        connect(m_agent, &AgentLoop::turnCompleted, this, &AgentPanel::onTurnCompleted);
        connect(m_agent, &AgentLoop::error, this, &AgentPanel::onError);
        connect(m_agent, &AgentLoop::runningChanged, this, &AgentPanel::onRunningChanged);
    connect(m_agent, &AgentLoop::threadUpdated, this, &AgentPanel::onThreadUpdated);
    connect(m_agent, &AgentLoop::titleGenerated, this, &AgentPanel::onTitleGenerated);
    } else {
        // AgentPanel: m_agent is NULL - cannot connect AgentLoop signals
    }
    
    if (m_permissions) {
        connect(m_permissions, &PermissionManager::permissionRequested, 
                this, &AgentPanel::onPermissionRequested);
    }
    
    // Listen for settings changes from config page
    // This ensures the sidebar updates when model is changed in settings
    // Note: We need to connect to plugin's settingsChanged signal
    // This will be set up in the plugin itself
}

// Method to update model from settings
void AgentPanel::updateModelFromSettings()
{
    QString newModel = m_config->getActiveModel();
    
    // Update the input bar model combo
    if (m_inputBar) {
        int idx = m_inputBar->findModel(newModel);
        if (idx >= 0) {
            m_inputBar->setCurrentModelIndex(idx);
        }
    }
}

void AgentPanel::createNewChatTab()
{
    m_chatCounter++;
    QString threadId = QString("chat_%1_%2").arg(QDate::currentDate().toString("yyyyMMdd")).arg(m_chatCounter);
    QString title = generateChatTitle(m_chatCounter);
    
    ThreadView *threadView = new ThreadView(m_tabs);
    int index = m_tabs->addTab(threadView, title);
    m_tabs->setTabToolTip(index, threadId);
    cacheTabs();
    connect(threadView, &ThreadView::deleteMessageRequested, this, &AgentPanel::onDeleteMessage);
    
    m_tabs->setCurrentIndex(index);
    m_currentThreadId = threadId;
    
}

void AgentPanel::closeChatTab(int index)
{
    if (index < 0 || index >= m_tabs->count()) {
        return;
    }
    

    
    // Get the thread ID before removing the tab
    QString threadId = m_tabs->tabToolTip(index);
    
    if (!threadId.isEmpty()) {
        if (m_threadStorage) {
            m_threadStorage->deleteThread(threadId);
        }
        if (m_agent) {
            m_agent->deleteThread(threadId);
        }
    }
    
    QWidget *widget = m_tabs->widget(index);
    m_tabs->removeTab(index);
    delete widget;
    
    if (m_tabs->count() > 0) {
        int newIndex = qMax(0, index - 1);
        m_tabs->setCurrentIndex(newIndex);
        m_currentThreadId = m_tabs->tabToolTip(newIndex);
    } else {
        createNewChatTab();
    }
    cacheTabs();
    
}

void AgentPanel::updateTabTitle(int index, const QString &title)
{
    if (index >= 0 && index < m_tabs->count()) {
        m_tabs->setTabText(index, title);
    }
}

void AgentPanel::renameChatTab(int index)
{
    if (index < 0 || index >= m_tabs->count()) {
        return;
    }
    
    QString currentTitle = m_tabs->tabText(index);
    bool ok = false;
    QString newTitle = QInputDialog::getText(
        this,
        i18n("Rename Chat"),
        i18n("Enter a new name for this chat:"),
        QLineEdit::Normal,
        currentTitle,
        &ok
    );
    
    if (ok && !newTitle.trimmed().isEmpty()) {
        updateTabTitle(index, newTitle.trimmed());
        
        // Update the thread title in storage
        m_hasUnsavedChanges = true;
        saveCurrentThread();
    }
}

void AgentPanel::saveCurrentThread()
{
    int currentIndex = m_tabs->currentIndex();
    if (currentIndex < 0) {
        return;
    }
    
    ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(currentIndex));
    if (!threadView || m_currentThreadId.isEmpty()) {
        return;
    }
    
    // Don't save if thread has no messages (empty chat)
    if (threadView->document()->isEmpty()) {
        return;
    }
    
    if (m_agent) {
        // Use AgentLoop as the single source of truth
        m_agent->saveCurrentThread();
    }
}

void AgentPanel::loadExistingThreads()
{
    // Use AgentLoop as the single source of truth
    QMap<QString, ConversationThread> threads;
    if (m_agent) {
        threads = m_agent->getThreads();
    }
    
    // Track max counter from existing threads
    QRegularExpression counterPattern("chat_\\d+_(\\d+)");
    for (auto it = threads.begin(); it != threads.end(); ++it) {
        // Parse existing thread IDs to find the max counter
        QString id = it.key();
        QRegularExpressionMatch match = counterPattern.match(id);
        if (match.hasCaptured(0)) {
            int counter = match.captured(1).toInt();
            m_chatCounter = qMax(m_chatCounter, counter);
        }
    }
    
    
    if (threads.isEmpty()) {
        return;
    }
    
    // Clear all existing tabs before loading saved threads
    while (m_tabs->count() > 0) {
        QWidget *widget = m_tabs->widget(0);
        m_tabs->removeTab(0);
        delete widget;
    }
    
    for (auto it = threads.begin(); it != threads.end(); ++it) {
        const ConversationThread &thread = it.value();
        
        // Use thread title if available, otherwise generate a default title
        QString title = thread.title;
        if (title.trimmed().isEmpty()) {
            m_chatCounter++;
            title = generateChatTitle(m_chatCounter);
        }
        
        ThreadView *threadView = new ThreadView(m_tabs);
        int index = m_tabs->addTab(threadView, title);
        m_tabs->setTabToolTip(index, thread.id);
        
        connect(threadView, &ThreadView::deleteMessageRequested, this, &AgentPanel::onDeleteMessage);
        
        threadView->loadMessages(thread.messages);
    }
    
    if (m_tabs->count() > 0) {
        m_tabs->setCurrentIndex(0);
        m_currentThreadId = m_tabs->tabToolTip(0);
    }
    cacheTabs();
}

QString AgentPanel::generateChatTitle(int chatNumber)
{
    return QString("Chat %1").arg(chatNumber);
}

void AgentPanel::onNewChat()
{
    createNewChatTab();
}

void AgentPanel::onTabCloseRequested(int index)
{
    closeChatTab(index);
}

void AgentPanel::onDeleteMessage(int messageId)
{
    if (m_currentThreadId.isEmpty() || !m_agent) {
        return;
    }
    
    auto threads = m_agent->getThreads();
    if (!threads.contains(m_currentThreadId)) {
        return;
    }
    
    auto &messages = threads[m_currentThreadId].messages;
    if (messages.isEmpty()) {
        return;
    }
    
    // Use the provided messageId as the index
    if (messageId < 0 || messageId >= messages.size()) {
        return;
    }
    
    m_agent->deleteMessage(m_currentThreadId, messageId);
    
    if (m_tabs->count() > 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->currentWidget());
        if (threadView) {
            // Reload messages to reflect the deletion
            threadView->loadMessages(m_agent->getThreads()[m_currentThreadId].messages);
        }
    }
}

void AgentPanel::onCurrentTabChanged(int index)
{
    if (index >= 0 && index < m_tabs->count()) {
        // Save only if we're switching away from a tab with unsaved changes
        if (m_hasUnsavedChanges) {
            saveCurrentThread();
            m_hasUnsavedChanges = false;
        }
        
        m_currentThreadId = m_tabs->tabToolTip(index);
    }
}

QAction *AgentPanel::createAction()
{
    QAction *action = new QAction(i18n("Kate Agent"), this);
    return action;
}

void AgentPanel::onSendMessage(const QString &message)
{
    // Re-entrancy guard: prevent sending while agent is running
    if (m_agent && m_agent->isRunning()) {
        // Agent is already running, ignoring send request
        return;
    }
    
    if (!m_agent || !m_inputBar || message.trimmed().isEmpty() || m_currentThreadId.isEmpty()) {
        return;
    }
    
    // Ensure the thread has the current model selected in the dropdown
    if (m_provider && m_agent) {
        QString currentModel = m_inputBar->currentModel();
        if (!currentModel.isEmpty()) {
            // Get thread from AgentLoop (single source of truth)
            auto threads = m_agent->getThreads();
            ConversationThread thread;
            if (threads.contains(m_currentThreadId)) {
                thread = threads[m_currentThreadId];
            } else {
                thread.id = m_currentThreadId;
            }
            if (thread.currentModel != currentModel) {
                thread.currentModel = currentModel;
                m_agent->setCurrentThread(thread);
            }
        }
    }
    
    m_hasUnsavedChanges = true;
    
    QString currentProfile = m_inputBar->currentProfile();
    m_agent->addUserMessage(m_currentThreadId, message, currentProfile);
    
    // addUserMessage emits threadUpdated → renderThread → user message is already shown in UI.
    // Set active thread for response streaming and get the current view.
    int currentIdx = m_tabs->currentIndex();
    ThreadView *curView = qobject_cast<ThreadView*>(m_tabs->widget(currentIdx));
    
    m_activeThreadId = m_currentThreadId;
    QString currentModel = m_inputBar->currentModel();
    if (curView && !currentModel.isEmpty()) {
        curView->setStreamingModel(currentModel);
    }
    m_agent->executeTurn(m_currentThreadId, currentModel);  // Pass model explicitly
    
    m_inputBar->clear();
    
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
    
    // Also update the current thread's model via AgentLoop
    if (!m_currentThreadId.isEmpty() && m_agent) {
        auto threads = m_agent->getThreads();
        ConversationThread thread;
        if (threads.contains(m_currentThreadId)) {
            thread = threads[m_currentThreadId];
        } else {
            thread.id = m_currentThreadId;
        }
        thread.currentModel = model;
        m_agent->setCurrentThread(thread);
    }
}

void AgentPanel::onSystemPromptChanged(const QString &prompt)
{
    if (m_agent) {
        m_agent->setSystemPrompt(prompt);
    }
}

void AgentPanel::onResponseStarted()
{
    if (m_activeThreadId.isEmpty()) {
        return;
    }
    int index = m_tabHash.value(m_activeThreadId, -1);
    if (index >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(index));
        if (threadView) {
            threadView->resetStreaming();
        }
    }
}

void AgentPanel::onChunkReceived(const QString &threadId, const QString &chunk, bool isThinking)
{
    if (isThinking) {
        return;
    }

    int index = m_tabHash.value(threadId, -1);
    if (index >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(index));
        if (threadView) {
            threadView->showStreamingChunk(chunk);
            m_hasUnsavedChanges = true;
        }
    }
}

void AgentPanel::onToolCallStarted(const QString &toolName, const QJsonObject &args)
{
    if (m_activeThreadId.isEmpty()) {
        return;
    }
    int index = m_tabHash.value(m_activeThreadId, -1);
    if (index >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(index));
        if (threadView) {
            threadView->appendToolCall(toolName, args);
        }
    }
}

void AgentPanel::onToolCallCompleted(const QString &toolName, const QJsonObject &result)
{
    if (m_activeThreadId.isEmpty()) {
        return;
    }
    int index = m_tabHash.value(m_activeThreadId, -1);
    if (index >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(index));
        if (threadView) {
            threadView->appendToolResult(toolName, result);
        }
    }
}

void AgentPanel::onTurnCompleted()
{
    if (m_activeThreadId.isEmpty()) {
        return;
    }
    
    QString thinking;
    QString content;
    
    if (m_agent) {
        auto &threads = m_agent->getThreads();
        if (threads.contains(m_activeThreadId)) {
            auto &messages = threads[m_activeThreadId].messages;
            if (!messages.isEmpty()) {
                auto &lastMsg = messages.last();
                thinking = lastMsg.thinking;
                content = lastMsg.content;
            }
        }
    }
    
    int index = m_tabHash.value(m_activeThreadId, -1);
    if (index >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(index));
        if (threadView) {
            threadView->endStreaming();
        }
    }
    
    saveCurrentThread();
    
    // Trigger async LLM title generation if tab still has default title
    int currentIdx = m_tabs->currentIndex();
    if (m_agent && currentIdx >= 0) {
        QString currentTitle = m_tabs->tabText(currentIdx);
        if (currentTitle.startsWith("Chat ") && !m_pendingTitleTabs.contains(m_currentThreadId)) {
            m_pendingTitleTabs.insert(m_currentThreadId);
            m_agent->generateTitleFromMessages(m_currentThreadId);
        }
    }
}

void AgentPanel::onTitleGenerated(const QString &threadId, const QString &title)
{
    m_pendingTitleTabs.remove(threadId);

    int index = m_tabHash.value(threadId, -1);
    if (index >= 0) {
        updateTabTitle(index, title);
        // Update thread title via AgentLoop (single source of truth)
        if (m_agent) {
            auto threads = m_agent->getThreads();
            if (threads.contains(threadId)) {
                threads[threadId].title = title;
                m_agent->setCurrentThread(threads[threadId]);
            }
        }
    }
}

void AgentPanel::onError(const QString &error)
{
    int currentIndex = m_tabs->currentIndex();
    if (currentIndex >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(currentIndex));
        if (threadView) {
            threadView->appendHtml(QString("<div class='tool-result error'>%1</div>").arg(i18n("Error: %1").arg(error)));
            threadView->ensureCursorVisible();  // Scroll to show the error
        }
    }
}

void AgentPanel::onRunningChanged(bool running)
{
    m_inputBar->setRunningState(running);
}

void AgentPanel::onPermissionRequested(const QString &toolName)
{
    if (toolName.isEmpty() || !m_permissions) {
        return;
    }
    
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Tool Permission Required"));
    msgBox.setText(tr("Tool '%1' wants to be executed. Allow?").arg(toolName));
    
    QPushButton* allowBtn = msgBox.addButton(tr("Allow (once)"), QMessageBox::ActionRole);
    QPushButton* alwaysBtn = msgBox.addButton(tr("Always Allow"), QMessageBox::ActionRole);
    QPushButton* denyBtn = msgBox.addButton(tr("Deny"), QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == allowBtn) {
        m_permissions->grantPermission(toolName);
    } else if (msgBox.clickedButton() == alwaysBtn) {
        m_permissions->grantPermission(toolName);
        m_permissions->setToolPolicy(toolName, PermissionPolicy::Allow);
    } else if (msgBox.clickedButton() == denyBtn) {
        m_permissions->denyPermission(toolName);
    }
    // If dialog closed without button click (ESC), deny permission
}

void AgentPanel::reloadModels()
{
    if (m_provider) {
        QStringList models = m_provider->availableModels();
        if (!models.isEmpty()) {
            m_inputBar->setModels(models);
            
            KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
            QString defaultModel = group.readEntry("Model", QString());
            
            if (!defaultModel.isEmpty() && models.contains(defaultModel)) {
                m_inputBar->setCurrentModel(defaultModel);
            }
        }
    }
}


void AgentPanel::onThreadUpdated(const QString &threadId)
{
    if (!m_agent || threadId.isEmpty()) {
        return;
    }
    
    // Get thread from AgentLoop (single source of truth)
    auto threads = m_agent->getThreads();
    if (!threads.contains(threadId)) {
        return;
    }
    ConversationThread thread = threads[threadId];
    int index = m_tabHash.value(threadId, -1);
    if (index >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(index));
        if (threadView) {
            threadView->renderThread(thread.messages);
        }
    }
}
