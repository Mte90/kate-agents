#include "agent-engine.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QDebug>
#include <QTimer>

namespace KateAgents {

AgentEngine::AgentEngine(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_retryCount(0)
{
}

AgentEngine::~AgentEngine()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

void AgentEngine::generateResponse(const std::vector<LLMMessage>& messages, const std::vector<ToolDefinition>& tools, const QString& model, double temperature, QObject* receiver, const char* slot)
{
    QJsonObject json;
    json["model"] = model;
    json["temperature"] = temperature;
    
    QJsonArray msgArray;
    for (const auto& msg : messages) {
        QJsonObject msgObj;
        msgObj["role"] = msg.role;
        msgObj["content"] = msg.content;
        if (!msg.toolCallId.isEmpty()) {
            msgObj["tool_call_id"] = msg.toolCallId;
        }
        msgArray.append(msgObj);
    }
    json["messages"] = msgArray;
    
    if (!tools.empty()) {
        QJsonArray toolsArray;
        for (const auto& tool : tools) {
            QJsonObject toolObj;
            toolObj["type"] = tool.type;
            QJsonObject funcObj;
            funcObj["name"] = tool.function.name;
            funcObj["description"] = tool.function.description;
            funcObj["parameters"] = tool.function.parameters;
            toolObj["function"] = funcObj;
            toolsArray.append(toolObj);
        }
        json["tools"] = toolsArray;
    }
    
    // Emit signal to trigger slot
    if (receiver && slot) {
        QMetaObject::invokeMethod(receiver, slot, Q_ARG(QString, "Response generation not fully implemented"));
    }
}

void AgentEngine::chatStream(
    const std::vector<LLMMessage>& messages,
    const std::vector<ToolDefinition>& tools,
    const QString& model,
    std::function<void(const QString& chunk)> onChunk,
    std::function<void(const LLMResponse& final)> onDone,
    std::function<void(const QString& error)> onError)
{
    QJsonObject json;
    json["model"] = model;
    json["stream"] = true;
    
    QJsonArray msgArray;
    for (const auto& msg : messages) {
        QJsonObject msgObj;
        msgObj["role"] = msg.role;
        msgObj["content"] = msg.content;
        if (!msg.toolCallId.isEmpty()) {
            msgObj["tool_call_id"] = msg.toolCallId;
        }
        msgArray.append(msgObj);
    }
    json["messages"] = msgArray;
    
    if (!tools.empty()) {
        QJsonArray toolsArray;
        for (const auto& tool : tools) {
            QJsonObject toolObj;
            toolObj["type"] = tool.type;
            QJsonObject funcObj;
            funcObj["name"] = tool.function.name;
            funcObj["description"] = tool.function.description;
            funcObj["parameters"] = tool.function.parameters;
            toolObj["function"] = funcObj;
            toolsArray.append(toolObj);
        }
        json["tools"] = toolsArray;
    }
    
    // Streaming implementation pending:
    // - Configure API endpoint URL
    // - Parse SSE events line-by-line (see OpenAIProvider::chatStream)
    // - Emit responseChunkReceived/generationComplete signals
}

void AgentEngine::cancelGeneration()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

} // namespace KateAgents
