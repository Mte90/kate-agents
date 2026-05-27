#ifndef AGENT_WORKER_H
#define AGENT_WORKER_H

#include <QObject>
#include <QThread>
#include <QPointer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QVector>
#include <functional>

#include "llmprovider.h"
#include "toolregistry.h"

namespace KateAgents {

/**
 * @brief AgentWorker handles all LLM and network operations in a separate thread
 * 
 * This class is designed to live in a QThread, completely separating network
 * operations from the UI thread to prevent any blocking or UI freezes.
 * 
 * Usage:
 * 1. Create AgentWorker
 * 2. Move to QThread: worker->moveToThread(workerThread);
 * 3. Connect signals/slots
 * 4. Start thread: workerThread->start();
 * 5. Send requests via sendRequest()
 * 6. Receive chunks via responseChunk() signal
 */
class AgentWorker : public QObject
{
    Q_OBJECT

public:
    explicit AgentWorker(QObject *parent = nullptr);
    ~AgentWorker() override;

    /**
     * @brief Set the LLM provider configuration
     * @param baseUrl API base URL
     * @param apiKey API key
     */
    void setProviderConfig(const QString &baseUrl, const QString &apiKey);

    /**
     * @brief Set tool registry for tool execution
     * @param registry ToolRegistry instance
     */
    void setToolRegistry(ToolRegistry *registry) { m_registry = registry; }

public slots:
    /**
     * @brief Send a streaming LLM request
     * @param messages List of LLM messages
     * @param tools List of available tools
     * @param model Model name to use
     * @param temperature Temperature parameter
     */
    void sendRequest(const std::vector<LLMMessage> &messages,
                     const std::vector<ToolDefinition> &tools,
                     const QString &model,
                     double temperature);

    /**
     * @brief Abort the current request
     */
    void abortRequest();

    /**
     * @brief Execute a tool call (called from main thread, runs in worker thread)
     * @param toolName Tool name
     * @param args Tool arguments
     * @return Tool result
     */
    QJsonObject executeTool(const QString &toolName, const QJsonObject &args);

signals:
    /**
     * @brief Emitted when a text chunk is received
     * @param chunk The text chunk
     */
    void responseChunk(const QString &chunk);

    /**
     * @brief Emitted when the final response is complete
     * @param response The complete response
     */
    void requestCompleted(const LLMResponse &response);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void error(const QString &error);

    /**
     * @brief Emitted when a tool call starts
     * @param toolName Tool name
     * @param args Tool arguments
     */
    void toolCallStarted(const QString &toolName, const QJsonObject &args);

    /**
     * @brief Emitted when a tool call completes
     * @param toolName Tool name
     * @param result Tool result
     */
    void toolCallCompleted(const QString &toolName, const QJsonObject &result);

private:
    /**
     * @brief Build the JSON request payload
     */
    QJsonDocument buildRequestPayload(const std::vector<LLMMessage> &messages,
                                      const std::vector<ToolDefinition> &tools,
                                      const QString &model,
                                      double temperature);

    /**
     * @brief Parse SSE response line by line
     */
    void parseSSEResponse(const QByteArray &responseData);

    QNetworkAccessManager *m_networkManager = nullptr;
    QPointer<QNetworkReply> m_currentReply;
    QPointer<ToolRegistry> m_registry;
    
    QString m_baseUrl;
    QString m_apiKey;
    int m_retryCount = 0;
    static constexpr int MAX_RETRIES = 3;
};

} // namespace KateAgents

#endif // AGENT_WORKER_H