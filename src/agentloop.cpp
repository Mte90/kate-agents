#include "agentloop.h"
#include "editorcontext.h"
#include "ghosttextprovider.h"
#include "threadjson.h"
#include <KTextEditor/MainWindow>
#include <KTextEditor/Document>
#include <QUuid>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QtConcurrent>
#include <QApplication>
#include <QKeyEvent>

QString systemPromptForProfile(AgentProfile profile)
{
    switch (profile) {
    case AgentProfile::Write:
        return "You are a coding assistant with full access to read, edit, and execute. You can modify files freely. Always show what you changed.";
    case AgentProfile::Ask:
        return "You are a coding assistant in read-only mode. Answer questions about the code but do NOT modify any files. You can only read and search.";
    case AgentProfile::Minimal:
        return "You are a brief coding assistant. Give concise answers. Minimize context usage.";
    default:
        return "You are a coding assistant.";
    }
}

AgentProfile stringToProfile(const QString &str)
{
    if (str == "Write") return AgentProfile::Write;
    if (str == "Ask") return AgentProfile::Ask;
    if (str == "Minimal") return AgentProfile::Minimal;
    return AgentProfile::Write;
}

QString profileToString(AgentProfile profile)
{
    switch (profile) {
    case AgentProfile::Write: return "Write";
    case AgentProfile::Ask: return "Ask";
    case AgentProfile::Minimal: return "Minimal";
    default: return "Write";
    }
}

AgentLoop::AgentLoop(LLMProvider *provider, ToolRegistry *registry, QObject *parent)
    : QObject(parent)
    , m_provider(provider)
    , m_registry(registry)
    , m_threadStorage(new ThreadStorage(this))
{
    if (m_threadStorage->initialize()) {
        m_threads = m_threadStorage->loadAllThreads();
    }
}

AgentLoop::~AgentLoop() = default;

void AgentLoop::setProvider(LLMProvider *provider)
{
    m_provider = provider;
}

void AgentLoop::setToolRegistry(ToolRegistry *registry)
{
    m_registry = registry;
}

void AgentLoop::setMainWindow(KTextEditor::MainWindow *mw)
{
    m_mainWindow = mw;
    
    // Initialize GhostTextProvider
    if (m_mainWindow) {
        m_ghostTextProvider = new GhostTextProvider(this);
        
        // Connect to view creation to register the provider
        connect(m_mainWindow, &KTextEditor::MainWindow::viewCreated, this,
                [this](KTextEditor::View *view) {
                    view->registerInlineNoteProvider(m_ghostTextProvider);
                    // Update project ID when a new view is created
                    updateProjectIdFromCurrentFile();
                });
        
        // Register for existing views
        auto views = m_mainWindow->views();
        for (KTextEditor::View *view : views) {
            view->registerInlineNoteProvider(m_ghostTextProvider);
        }
        
        // Update project ID from current file
        updateProjectIdFromCurrentFile();
    }
}

void AgentLoop::setSystemPrompt(const QString &prompt)
{
    m_systemPrompt = prompt;
}

ConversationThread AgentLoop::createThread(const QString &title)
{
    ConversationThread thread;
    thread.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    thread.createdAt = QDateTime::currentDateTime();
    thread.updatedAt = QDateTime::currentDateTime();
    thread.title = title;
    thread.isActive = true;
    
    if (!m_systemPrompt.isEmpty()) {
        LLMMessage systemMsg;
        systemMsg.role = "system";
        systemMsg.content = m_systemPrompt;
        thread.messages.push_back(systemMsg);
    }
    
    return thread;
}

void AgentLoop::addUserMessage(const QString &threadId, const QString &content, const QString &profile)
{
    if (!m_threads.contains(threadId)) {
        m_threads[threadId] = createThread();
        m_threads[threadId].id = threadId;  // Match UI key for storage lookup
        if (m_threadStorage) {
            m_threadStorage->saveThread(m_threads[threadId]);
        }
    }

    ConversationThread &thread = m_threads[threadId];

    LLMMessage userMsg;
    userMsg.role = "user";
    userMsg.content = content;
    userMsg.profile = profile;
    thread.messages.push_back(userMsg);

    thread.updatedAt = QDateTime::currentDateTime();

    if (m_threadStorage) {
        m_threadStorage->saveThread(thread);
    }
    
    // Emit signal to update UI - show user message immediately
    emit threadUpdated(threadId);
}

void AgentLoop::executeTurn(const QString &threadId, const QString &model)
{
    // Check if thread exists
    if (!m_threads.contains(threadId)) {
        emit error(QString("Thread not found: %1").arg(threadId));
        return;
    }

    // Check if provider is set
    if (!m_provider) {
        emit error("No LLM provider set");
        return;
    }

    m_isRunning = true;
    m_currentThreadId = threadId;
    m_currentIteration = 0;
    m_iterationCount = 0;
    emit runningChanged(true);

    // Use provided model, or fall back to thread's model, or use first available
    QString modelToUse = model;
    if (modelToUse.isEmpty()) {
        modelToUse = m_threads[threadId].currentModel;
    }
    if (modelToUse.isEmpty()) {
        QStringList models = m_provider->availableModels();
        if (!models.isEmpty()) {
            modelToUse = models.first();
        } else {
            emit error("No models available");
            m_isRunning = false;
            emit runningChanged(false);
            return;
        }
    }


    // Start the iterative loop - call LLM for the first time
    callLLMInternal(threadId, modelToUse);
}

void AgentLoop::callLLM(const QString &threadId, const QString &model)
{
    // Increment iteration count
    ++m_iterationCount;

    // Build the request from thread messages
    buildRequest(threadId);

    // Call the LLM with streaming
    m_provider->chatStream(
        m_currentRequest,
        m_currentTools,
        model,
        // onChunk - emit each chunk as it arrives
        [this](const QString &chunk) {
            emit responseChunk(chunk);
        },
        // onDone - handle final response
        [this, threadId, model](const LLMResponse &final) {
            // Add assistant message to thread
            if (!final.content.isEmpty()) {
                LLMMessage assistantMsg;
                assistantMsg.role = "assistant";
                assistantMsg.content = final.content;
                m_threads[threadId].messages.push_back(assistantMsg);
            }
            // Check for tool calls
            if (!final.toolCalls.empty()) {
                // Check iteration limit
                if (m_iterationCount >= m_maxIterations) {
                    emit error(QString("Max iterations (%1) reached").arg(m_maxIterations));
                    m_isRunning = false;
                    emit runningChanged(false);
                    emit turnCompleted(threadId);
                    return;
                }

                // Handle tool calls - they will trigger another LLM call
                handleToolCalls(final.toolCalls, threadId);
                
                // After handling tool calls, call LLM again if within iteration limit
                if (m_iterationCount < m_maxIterations) {
                    callLLM(threadId, model);
                } else {
                    emit turnCompleted(threadId);
                    m_isRunning = false;
                    emit runningChanged(false);
                }
            } else {
                // No more tool calls, turn is complete
                if (m_threadStorage && m_threads.contains(threadId)) {
                    m_threadStorage->saveThread(m_threads[threadId]);
                }
                emit turnCompleted(threadId);
                m_isRunning = false;
                emit runningChanged(false);
            }
        },
        // onError - handle errors
        [this](const QString &errMsg) {
            emit error(QString("LLM streaming error: %1").arg(errMsg));
            m_isRunning = false;
            emit runningChanged(false);
        }
    );
}

void AgentLoop::callLLMInternal(const QString &threadId, const QString &model)
{
    
    // Build the request from thread messages
    buildRequest(threadId);

    // Call the LLM with streaming
    m_provider->chatStream(
        m_currentRequest,
        m_currentTools,
        model,
        // onChunk - emit each chunk as it arrives
        [this](const QString &chunk) {
            emit responseChunk(chunk);
        },
        // onDone - handle final response - recursive pattern
        [this, threadId, model](const LLMResponse &final) {
            // Add assistant message to thread
            if (!final.content.isEmpty()) {
                LLMMessage assistantMsg;
                assistantMsg.role = "assistant";
                assistantMsg.content = final.content;
                m_threads[threadId].messages.push_back(assistantMsg);
            }
            // Check for tool calls
            if (!final.toolCalls.empty()) {
                // Check iteration limit using m_currentIteration
                if (m_currentIteration >= m_maxIterations) {
                    emit error(QString("Max iterations (%1) reached").arg(m_maxIterations));
                    m_isRunning = false;
                    emit runningChanged(false);
                    emit turnCompleted(threadId);
                    m_currentIteration = 0; // Reset for next turn
                    return;
                }

                // Handle tool calls
                handleToolCalls(final.toolCalls, threadId);
                
                // Increment iteration and recursively call executeTurn to continue loop
                m_currentIteration++;
                if (m_currentIteration < m_maxIterations) {
                    executeTurn(threadId);
                } else {
                    // Max iterations reached
                    m_isRunning = false;
                    emit runningChanged(false);
                    emit turnCompleted(threadId);
                    m_currentIteration = 0; // Reset for next turn
                }
            } else {
                // No more tool calls, turn is complete
                if (m_threadStorage && m_threads.contains(threadId)) {
                    m_threadStorage->saveThread(m_threads[threadId]);
                }
                emit turnCompleted(threadId);
                m_isRunning = false;
                emit runningChanged(false);
                m_currentIteration = 0; // Reset for next turn
            }
        },
        // onError - handle errors
        [this](const QString &errMsg) {
            emit error(QString("LLM streaming error: %1").arg(errMsg));
            m_isRunning = false;
            emit runningChanged(false);
            m_currentIteration = 0; // Reset for next turn
        }
    );
}

void AgentLoop::abort()
{
    m_isRunning = false;
    emit runningChanged(false);
}

void AgentLoop::saveAllThreads()
{
    if (m_threadStorage) {
        for (auto it = m_threads.constBegin(); it != m_threads.constEnd(); ++it) {
            m_threadStorage->saveThread(it.value());
        }
    }
}

void AgentLoop::buildRequest(const QString &threadId)
{
    constexpr int maxMessages = 100;

    auto it = m_threads.find(threadId);
    if (it == m_threads.end()) {
        emit error(QString("Thread not found: %1").arg(threadId));
        return;
    }

    ConversationThread &thread = *it;
    size_t totalMessages = thread.messages.size();
    size_t startIndex = 0;

    bool hasSystemPrompt = !thread.messages.empty() && thread.messages[0].role == "system";

    if (totalMessages > static_cast<size_t>(maxMessages)) {
        if (hasSystemPrompt) {
            startIndex = totalMessages - (maxMessages - 1);
        } else {
            startIndex = totalMessages - maxMessages;
        }
    }

    m_currentRequest.clear();
    
    // Inject EditorContext as system message if file is open and window is valid
    if (m_mainWindow != nullptr) {
        EditorContext ctx;
        ctx.capture(m_mainWindow);
        QString contextChunk = ctx.toSystemPromptChunk();
        if (!contextChunk.isEmpty()) {
            // Ensure system prompt exists
            bool hasSystem = m_currentRequest.empty() || m_currentRequest[0].role != "system";
            if (hasSystem) {
                LLMMessage systemMsg;
                systemMsg.role = "system";
                systemMsg.content = "Editor context injected below.";
                m_currentRequest.insert(m_currentRequest.begin(), systemMsg);
            }
            // Find system prompt and inject context after it
            for (auto &msg : m_currentRequest) {
                if (msg.role == "system") {
                    msg.content += "\n\n# Current Editor Context:\n" + contextChunk;
                    break;
                }
            }
        }
    }
    
    for (size_t i = startIndex; i < totalMessages; ++i) {
        m_currentRequest.push_back(thread.messages[i]);
    }

    m_currentTools = m_registry ? m_registry->getToolDefinitions() : std::vector<ToolDefinition>();
}

void AgentLoop::handleToolCalls(const std::vector<ToolCall> &toolCalls, const QString &threadId)
{
    if (toolCalls.empty()) {
        return;
    }
    
    if (!m_registry) {
        emit error("Tool registry not set");
        return;
    }
    
    // Create shared container for results (thread-safe)
    QMutex resultsMutex;
    QVector<QPair<QString, QJsonObject>> indexedResults;
    indexedResults.resize(toolCalls.size());
    
    // Launch parallel execution for all tool calls
    for (size_t i = 0; i < toolCalls.size(); ++i) {
        const ToolCall &toolCall = toolCalls[i];
        const QString &toolCallId = toolCall.id;
        const QString &toolName = toolCall.name;
        const QJsonObject &args = toolCall.arguments;
        
        // Use QtConcurrent to execute tool calls in parallel
        QFuture<QJsonObject> future = QtConcurrent::run(
            [&, i]() -> QJsonObject {
                // Emit start signal (all start signals fire immediately)
                emit toolCallStarted(toolName, args);
                
                // Execute tool (already thread-safe per toolregistry.cpp)
                QJsonObject result = m_registry->executeTool(toolName, args);
                
                // Store result in our indexed vector
                QMutexLocker locker(&resultsMutex);
                indexedResults[i] = {toolCallId, result};
                
                return result;
            }
        );
        
        // Wait for all futures to complete before emitting completion signals
        future.waitForFinished();
    }
    
    // Emit completion signals and add results in original order
    for (int i = 0; i < indexedResults.size(); ++i) {
        const auto &item = indexedResults[i];
        emit toolCallCompleted(item.first, item.second);
        addToolResult(threadId, item.first, "", item.second);
    }
}
void AgentLoop::addToolResult(const QString &threadId, const QString &toolCallId, const QString &toolName, const QJsonObject &result)
{
    Q_UNUSED(toolName);
    
    // Check if thread exists
    if (!m_threads.contains(threadId)) {
        emit error(QString("Thread not found: %1").arg(threadId));
        return;
    }

    ConversationThread &thread = m_threads[threadId];

    // Create tool message
    LLMMessage toolMsg;
    toolMsg.role = "tool";
    toolMsg.toolCallId = toolCallId;
    toolMsg.content = result.isEmpty() ? QString() : QJsonDocument(result).toJson(QJsonDocument::Compact);
    thread.messages.push_back(toolMsg);

    // Update timestamp
    thread.updatedAt = QDateTime::currentDateTime();
}

QString AgentLoop::generateTitle(const QString &firstMessage)
{
    if (firstMessage.length() > 50) {
        return firstMessage.left(47) + "...";
    }
    return firstMessage;
}

void AgentLoop::showGhostText(const QString &suggestion, int line, int column)
{
    if (m_ghostTextProvider) {
        m_ghostTextProvider->setSuggestion(suggestion, line, column);
    }
}

void AgentLoop::clearGhostText()
{
    if (m_ghostTextProvider) {
        m_ghostTextProvider->clearSuggestion();
    }
}

void AgentLoop::acceptGhostText()
{
    if (m_ghostTextProvider && m_ghostTextProvider->hasSuggestion()) {
        QString suggestion = m_ghostTextProvider->suggestion();
        m_ghostTextProvider->clearSuggestion();
        
        if (m_mainWindow) {
            auto views = m_mainWindow->views();
            if (!views.isEmpty()) {
                KTextEditor::View *view = views.first();
                // Use key press simulation to accept ghost text
                view->setFocus();
                // Simulate Tab key press to accept suggestion
                QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
                QApplication::sendEvent(view, &tabEvent);
            }
        }
    }
}

bool AgentLoop::hasGhostText() const
{
    return m_ghostTextProvider && m_ghostTextProvider->hasSuggestion();
}

void AgentLoop::updateProjectIdFromCurrentFile()
{
    if (!m_mainWindow) {
        return;
    }
    
    // Get active view
    auto views = m_mainWindow->views();
    if (views.isEmpty()) {
        return;
    }
    
    KTextEditor::View *activeView = m_mainWindow->activeView();
    if (!activeView) {
        activeView = views.first();
    }
    
    // Get document and file path
    KTextEditor::Document *doc = activeView->document();
    QString filePath = doc->url().toLocalFile();
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Update project ID based on the file's git repo
    ThreadJsonStorage::setCurrentProjectIdFromFile(filePath);
}
