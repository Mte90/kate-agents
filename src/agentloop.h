#ifndef AGENTLOOP_H
#define AGENTLOOP_H

#include "llmprovider.h"
#include "toolregistry.h"
#include "threadstorage.h"
#include <QObject>
#include <vector>
#include <QString>
#include <QMap>
#include <functional>
#include <QPointer>
#include <QMutex>
#include <QMetaEnum>

class GhostTextProvider;

enum class AgentProfile { Write, Ask, Minimal };
QString systemPromptForProfile(AgentProfile profile);
AgentProfile stringToProfile(const QString &str);
QString profileToString(AgentProfile profile);

namespace KTextEditor {
class MainWindow;
}

class AgentLoop : public QObject
{
    Q_OBJECT

public:
    explicit AgentLoop(LLMProvider *provider, ToolRegistry *registry, QObject *parent = nullptr);
    ~AgentLoop() override;

    void setProvider(LLMProvider *provider);
    void setToolRegistry(ToolRegistry *registry);
    void setMainWindow(KTextEditor::MainWindow *mw);
    void showGhostText(const QString &suggestion, int line, int column);
    void clearGhostText();
    void acceptGhostText();
    bool hasGhostText() const;

    ConversationThread createThread(const QString &title = QString());
    void addUserMessage(const QString &threadId, const QString &content, const QString &profile = QString());
    void executeTurn(const QString &threadId, const QString &model = QString());

    void setSystemPrompt(const QString &prompt);
    void setMaxIterations(int max) { m_maxIterations = max; }

    bool isRunning() const { return m_isRunning; }
    void abort();
    void saveAllThreads();

signals:
    void responseChunk(const QString &chunk);
    void toolCallStarted(const QString &toolName, const QJsonObject &args);
    void toolCallCompleted(const QString &toolName, const QJsonObject &result);
    void turnCompleted(const QString &threadId);
    void error(const QString &error);
    void runningChanged(bool running);
    void threadUpdated(const QString &threadId);

private:
    void buildRequest(const QString &threadId);
    void handleToolCalls(const std::vector<ToolCall> &toolCalls, const QString &threadId);
    void addToolResult(const QString &threadId, const QString &toolCallId, const QString &toolName, const QJsonObject &result);
    void callLLM(const QString &threadId, const QString &model);
    void callLLMInternal(const QString &threadId, const QString &model);
    QString generateTitle(const QString &firstMessage);
    void updateProjectIdFromCurrentFile();

    LLMProvider *m_provider = nullptr;
    ToolRegistry *m_registry = nullptr;
    ThreadStorage *m_threadStorage = nullptr;
    KTextEditor::MainWindow *m_mainWindow = nullptr;
    QString m_systemPrompt;
    int m_maxIterations = 20;
    int m_iterationCount = 0;
    int m_currentIteration = 0;
    bool m_isRunning = false;
    QString m_currentThreadId;
    QMap<QString, ConversationThread> m_threads;
    std::vector<LLMMessage> m_currentRequest;
    std::vector<ToolDefinition> m_currentTools;
    GhostTextProvider *m_ghostTextProvider = nullptr;
};

#endif
