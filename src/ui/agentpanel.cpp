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
#include <QDockWidget>
#include <QRegularExpression>

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
    , m_threadStorage(new ThreadStorage(this))
    , m_chatCounter(0)
    , m_currentThreadId()
{
    qDebug() << "AgentPanel: Constructor starting...";
    setupUi();
    qDebug() << "AgentPanel: setupUi complete, m_tabs:" << (m_tabs ? "ok" : "NULL") << "m_inputBar:" << (m_inputBar ? "ok" : "NULL");
    
    if (m_threadStorage) {
        m_threadStorage->initialize();
    }

    connectSignals();
    qDebug() << "AgentPanel: connectSignals complete";

    loadExistingThreads();
    qDebug() << "AgentPanel: Constructor complete";
}

AgentPanel::~AgentPanel() = default;

void AgentPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_tabs = new CloseableTabWidget(this);
    m_tabs->setDocumentMode(true);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    
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
    QHBoxLayout *newChatLayout = new QHBoxLayout(newChatWidget);
    newChatLayout->setContentsMargins(4, 2, 4, 2);
    newChatLayout->setSpacing(0);
    
    QPushButton *newChatBtn = new QPushButton("+");
    newChatBtn->setFixedSize(24, 24);
    newChatBtn->setToolTip(i18n("New Chat"));
    newChatBtn->setStyleSheet("QPushButton { font-weight: bold; font-size: 16px; }");
    connect(newChatBtn, &QPushButton::clicked, this, &AgentPanel::onNewChat);
    
    // Hide sidebar button
    QPushButton *hideBtn = new QPushButton();
    hideBtn->setFixedSize(16, 16);
    hideBtn->setToolTip(i18n("Hide sidebar"));
    hideBtn->setStyleSheet("QPushButton { font-size: 10px; border: none; } QPushButton:hover { background-color: palette(highlight); color: palette(highlighted-text); border-radius: 2px; }");
    hideBtn->setText(QStringLiteral("\u2192"));  // Right arrow to indicate collapse
    connect(hideBtn, &QPushButton::clicked, this, [this]() {
        QWidget *parent = this;
        while (parent) {
            auto *dock = qobject_cast<QDockWidget*>(parent);
            if (dock) {
                dock->hide();
                return;
            }
            parent = parent->parentWidget();
        }
    });
    
    newChatLayout->addWidget(hideBtn);
    newChatLayout->addStretch();
    newChatLayout->addWidget(newChatBtn);
    
    mainLayout->addWidget(newChatWidget);
    mainLayout->addWidget(m_tabs, 1);
    
    m_inputBar = new InputBar(this);
    mainLayout->addWidget(m_inputBar);
    
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
        qWarning() << "AgentPanel: Cannot connect signals - m_inputBar or m_tabs is null";
        return;
    }
    
    connect(m_inputBar, &InputBar::sendMessage, this, &AgentPanel::onSendMessage);
    connect(m_inputBar, &InputBar::stopRequested, this, &AgentPanel::onStopRequested);
    connect(m_inputBar, &InputBar::modelChanged, this, &AgentPanel::onModelChanged);
    connect(m_inputBar, &InputBar::systemPromptChanged, this, &AgentPanel::onSystemPromptChanged);
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &AgentPanel::onTabCloseRequested);
    connect(m_tabs, &QTabWidget::currentChanged, this, &AgentPanel::onCurrentTabChanged);
    
    if (m_agent) {
        connect(m_agent, &AgentLoop::responseChunk, this, &AgentPanel::onResponseChunk);
        connect(m_agent, &AgentLoop::toolCallStarted, this, &AgentPanel::onToolCallStarted);
        connect(m_agent, &AgentLoop::toolCallCompleted, this, &AgentPanel::onToolCallCompleted);
        connect(m_agent, &AgentLoop::turnCompleted, this, &AgentPanel::onTurnCompleted);
        connect(m_agent, &AgentLoop::error, this, &AgentPanel::onError);
        connect(m_agent, &AgentLoop::runningChanged, this, &AgentPanel::onRunningChanged);
    } else {
        qWarning() << "AgentPanel: m_agent is NULL - cannot connect AgentLoop signals";
    }
    
    if (m_permissions) {
        connect(m_permissions, &PermissionManager::permissionRequested, 
                this, &AgentPanel::onPermissionRequested);
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
    
    m_tabs->setCurrentIndex(index);
    m_currentThreadId = threadId;
    
    qDebug() << "AgentPanel: Created new chat tab" << index << "with ID:" << threadId;
}

void AgentPanel::closeChatTab(int index)
{
    if (index < 0 || index >= m_tabs->count()) {
        return;
    }
    
    // Get the thread ID before removing the tab
    QString threadId = m_tabs->tabToolTip(index);
    
    // Don't save, just delete the thread
    if (!threadId.isEmpty() && m_threadStorage) {
        bool deleted = m_threadStorage->deleteThread(threadId);
        qDebug() << "AgentPanel: Deleting thread" << threadId << (deleted ? "success" : "failed");
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
    
    qDebug() << "AgentPanel: Closed chat tab" << index;
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
    
    if (m_agent) {
        QMap<QString, ConversationThread> threads = m_threadStorage->loadAllThreads();
        
        ConversationThread currentThread;
        if (threads.contains(m_currentThreadId)) {
            currentThread = threads[m_currentThreadId];
        } else {
            currentThread.id = m_currentThreadId;
        }
        currentThread.title = m_tabs->tabText(currentIndex);
        
        m_threadStorage->saveThread(currentThread);
        
        qDebug() << "AgentPanel: Saved thread" << m_currentThreadId;
    }
}

void AgentPanel::loadExistingThreads()
{
    QMap<QString, ConversationThread> threads = m_threadStorage->loadAllThreads();
    
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
    
    qDebug() << "AgentPanel: Loaded threads" << threads.size() << "updated counter to" << m_chatCounter;
    
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
            title = generateChatTitle(m_chatCounter);
        }
        
        ThreadView *threadView = new ThreadView(m_tabs);
        int index = m_tabs->addTab(threadView, title);
        m_tabs->setTabToolTip(index, thread.id);
        
        threadView->loadMessages(thread.messages);
        
        m_chatCounter++;
    }
    
    if (m_tabs->count() > 0) {
        m_tabs->setCurrentIndex(0);
        m_currentThreadId = m_tabs->tabToolTip(0);
    }
    
    qDebug() << "AgentPanel: Loaded" << threads.size() << "existing threads";
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
    if (!m_agent || message.trimmed().isEmpty() || m_currentThreadId.isEmpty()) {
        return;
    }
    
    // Ensure the thread has the current model selected in the dropdown
    if (m_provider && m_threadStorage) {
        QString currentModel = m_inputBar->currentModel();
        if (!currentModel.isEmpty()) {
            ConversationThread thread;
            thread.id = m_currentThreadId;
            if (m_threadStorage) {
                QMap<QString, ConversationThread> threads = m_threadStorage->loadAllThreads();
                if (threads.contains(m_currentThreadId)) {
                    thread = threads[m_currentThreadId];
                }
            }
            if (thread.currentModel != currentModel) {
                thread.currentModel = currentModel;
                m_threadStorage->saveThread(thread);
                qDebug() << "AgentPanel: Updated thread model to:" << currentModel;
            }
        }
    }
    
    m_hasUnsavedChanges = true;
    
    int currentIndex = m_tabs->currentIndex();
    QString currentTitle = m_tabs->tabText(currentIndex);
    if (currentTitle.startsWith("Chat ")) {
        QString newTitle = message.left(30).trimmed();
        if (newTitle.length() > 20) {
            newTitle = newTitle.left(20) + "...";
        }
        updateTabTitle(currentIndex, newTitle);
        saveCurrentThread();
    }
    
    m_agent->addUserMessage(m_currentThreadId, message);
    m_agent->executeTurn(m_currentThreadId);
    
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
    
    // Also update the current thread's model
    if (!m_currentThreadId.isEmpty() && m_threadStorage) {
        ConversationThread thread;
        thread.id = m_currentThreadId;
        if (m_threadStorage) {
            QMap<QString, ConversationThread> threads = m_threadStorage->loadAllThreads();
            if (threads.contains(m_currentThreadId)) {
                thread = threads[m_currentThreadId];
            }
        }
        thread.currentModel = model;
        // Save the updated thread (pass the whole object)
        m_threadStorage->saveThread(thread);
        qDebug() << "AgentPanel: Updated thread model to:" << model;
    }
}

void AgentPanel::onSystemPromptChanged(const QString &prompt)
{
    if (m_agent) {
        m_agent->setSystemPrompt(prompt);
    }
}

void AgentPanel::onResponseChunk(const QString &chunk)
{
    int currentIndex = m_tabs->currentIndex();
    if (currentIndex >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(currentIndex));
        if (threadView) {
            threadView->showStreamingChunk(chunk);
            m_hasUnsavedChanges = true;
        }
    }
}

void AgentPanel::onToolCallStarted(const QString &toolName, const QJsonObject &args)
{
    int currentIndex = m_tabs->currentIndex();
    if (currentIndex >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(currentIndex));
        if (threadView) {
            threadView->appendToolCall(toolName, args);
        }
    }
}

void AgentPanel::onToolCallCompleted(const QString &toolName, const QJsonObject &result)
{
    int currentIndex = m_tabs->currentIndex();
    if (currentIndex >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(currentIndex));
        if (threadView) {
            threadView->appendToolResult(toolName, result);
        }
    }
}

void AgentPanel::onTurnCompleted()
{
    int currentIndex = m_tabs->currentIndex();
    if (currentIndex >= 0) {
        ThreadView *threadView = qobject_cast<ThreadView*>(m_tabs->widget(currentIndex));
        if (threadView) {
            threadView->appendHtml("<hr>");
        }
    }
    
    saveCurrentThread();
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
    qDebug() << "AgentPanel: Permission requested for:" << toolName;
}

void AgentPanel::reloadModels()
{
    qDebug() << "AgentPanel: Reloading models from provider";
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
