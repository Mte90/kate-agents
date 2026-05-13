#include "openaiprovider.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QBuffer>
#include <QDebug>
#include <QtConcurrent>
#include <QEventLoop>
#include <QByteArray>
#include <QRegularExpression>
#include <KLocalizedString>

OpenAIProvider::OpenAIProvider(const QString &baseUrl, const QString &apiKey, QObject *parent)
    : LLMProvider(parent)
    , m_baseUrl(baseUrl)
    , m_apiKey(apiKey)
    , m_nam(new QNetworkAccessManager(this))
{
}

OpenAIProvider::~OpenAIProvider() = default;

bool OpenAIProvider::isAvailable()
{
    return !m_baseUrl.isEmpty() && !m_apiKey.isEmpty();
}

QStringList OpenAIProvider::availableModels()
{
    // Fetch models from the API (synchronous for now)
    // In a real implementation, this should be async
    if (m_baseUrl.isEmpty() || m_apiKey.isEmpty()) {
        return QStringList() << "qwen3-coder-next";
    }
    
    QUrl modelsUrl(m_baseUrl + "/models");
    QNetworkRequest request(modelsUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!m_apiKey.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());
    }
    
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(request);
    
    QObject::connect(reply, &QNetworkReply::finished, &loop, [&loop, &reply]() {
        reply->deleteLater();
        loop.quit();
    });
    
    // Wait for response (timeout after 5 seconds)
    QTimer::singleShot(5000, &loop, [&loop, &reply]() {
        if (reply->isOpen()) {
            reply->abort();
            loop.quit();
        }
    });
    
    loop.exec();
    
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();
        QJsonArray modelsArray = obj["data"].toArray();
        
        QStringList models;
        for (const QJsonValue &val : modelsArray) {
            QJsonObject modelObj = val.toObject();
            QString modelId = modelObj["id"].toString();
            if (!modelId.isEmpty()) {
                models << modelId;
            }
        }
        
        return models.isEmpty() ? QStringList() << "qwen3-coder-next" : models;
    }
    
    return QStringList() << "qwen3-coder-next";
}

QFuture<LLMResponse> OpenAIProvider::chat(
    const std::vector<LLMMessage> &messages,
    const std::vector<ToolDefinition> &tools,
    const QString &model,
    double temperature,
    int maxTokens)
{
    return QtConcurrent::run([this, messages, tools, model, temperature, maxTokens]() {
        QEventLoop loop;
        LLMResponse response;
        QString error;
        
        QJsonObject json;
        json["model"] = model;
        json["temperature"] = temperature;
        json["max_tokens"] = maxTokens;
        
        QJsonArray msgArray;
        for (const auto &msg : messages) {
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
            for (const auto &tool : tools) {
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
        
        QUrl url(m_baseUrl + "/chat/completions");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());
        
        QNetworkReply *reply = m_nam->post(request, QJsonDocument(json).toJson());
        
        QObject::connect(reply, &QNetworkReply::finished, [&]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject root = doc.object();
                
                if (root.contains("choices") && root["choices"].isArray()) {
                    QJsonArray choices = root["choices"].toArray();
                    if (!choices.isEmpty()) {
                        QJsonObject choice = choices.at(0).toObject();
                        QJsonObject message = choice["message"].toObject();
                        
                        response.content = message["content"].toString();
                        response.finishReason = choice["finish_reason"].toString();
                        
                        if (message.contains("tool_calls") && message["tool_calls"].isArray()) {
                            QJsonArray tcArray = message["tool_calls"].toArray();
                            for (const auto &tc : tcArray) {
                                QJsonObject tcObj = tc.toObject();
                                ToolCall call;
                                call.id = tcObj["id"].toString();
                                call.name = tcObj["function"].toObject()["name"].toString();
                                call.arguments = tcObj["function"].toObject()["arguments"].toObject();
                                response.toolCalls.push_back(call);
                            }
                        }
                    }
                }
            } else {
                int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                if (statusCode == 401) {
                    error = i18n("Authentication failed: Invalid API key or missing authentication");
                } else if (statusCode == 403) {
                    error = i18n("Access forbidden: Check your API key and permissions");
                } else if (statusCode == 404) {
                    error = i18n("Endpoint not found: Check your base URL");
                } else {
                    error = reply->errorString();
                }
            }
            reply->deleteLater();
            loop.quit();
        });
        
        loop.exec();
        
        if (!error.isEmpty()) {
            qWarning() << "OpenAIProvider chat error:" << error;
        }
        
        return response;
    });
}

void OpenAIProvider::chatStream(
    const std::vector<LLMMessage> &messages,
    const std::vector<ToolDefinition> &tools,
    const QString &model,
    std::function<void(const QString &chunk)> onChunk,
    std::function<void(const LLMResponse &final)> onDone,
    std::function<void(const QString &error)> onError,
    double temperature,
    int maxTokens)
{
    // Non-streaming fallback - call chat and emit callbacks
    (void)QtConcurrent::run([this, messages, tools, model, temperature, maxTokens, onChunk, onDone, onError]() {
        QEventLoop loop;
        LLMResponse response;
        QString err;
        
        QJsonObject json;
        json["model"] = model;
        json["temperature"] = temperature;
        json["max_tokens"] = maxTokens;
        
        QJsonArray msgArray;
        for (const auto &msg : messages) {
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
            for (const auto &tool : tools) {
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
        
        QUrl url(m_baseUrl + "/chat/completions");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());
        
        QNetworkReply *reply = m_nam->post(request, QJsonDocument(json).toJson());
        
        QObject::connect(reply, &QNetworkReply::finished, [&]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject root = doc.object();
                
                if (root.contains("choices") && root["choices"].isArray()) {
                    QJsonArray choices = root["choices"].toArray();
                    if (!choices.isEmpty()) {
                        QJsonObject choice = choices.at(0).toObject();
                        QJsonObject message = choice["message"].toObject();
                        
                        response.content = message["content"].toString();
                        response.finishReason = choice["finish_reason"].toString();
                        
                        if (message.contains("tool_calls") && message["tool_calls"].isArray()) {
                            QJsonArray tcArray = message["tool_calls"].toArray();
                            for (const auto &tc : tcArray) {
                                QJsonObject tcObj = tc.toObject();
                                ToolCall call;
                                call.id = tcObj["id"].toString();
                                call.name = tcObj["function"].toObject()["name"].toString();
                                call.arguments = tcObj["function"].toObject()["arguments"].toObject();
                                response.toolCalls.push_back(call);
                            }
                        }
                    }
                }
            } else {
                err = reply->errorString();
            }
            reply->deleteLater();
            loop.quit();
        });
        
        loop.exec();
        
        if (!err.isEmpty()) {
            QMetaObject::invokeMethod(this, [onError, err]() {
                onError(err);
            }, Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, [onDone, response]() {
                onDone(response);
            }, Qt::QueuedConnection);
        }
    });
}
