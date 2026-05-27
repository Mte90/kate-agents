#include "openaiprovider.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QBuffer>
#include <QDebug>
#include <QEventLoop>
#include <QPromise>
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
    
    QEventLoop loop;
    QNetworkReply *reply = m_nam->get(request);
    
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
    double temperature)
{
    QPromise<LLMResponse> *promise = new QPromise<LLMResponse>();
    QFuture<LLMResponse> future = promise->future();
    
    QJsonObject json;
    json["model"] = model;
    json["temperature"] = temperature;
    
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
    
    QString jsonStr = QJsonDocument(json).toJson(QJsonDocument::Indented);
    
    QNetworkReply *reply = m_nam->post(request, QJsonDocument(json).toJson());
    m_currentReply = reply;
    
    QObject::connect(reply, &QNetworkReply::finished, [this, promise, reply]() {
        m_currentReply = nullptr;
        
        if (reply->error() == QNetworkReply::NoError) {
QByteArray responseData = reply->readAll();
            LLMResponse response;
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
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
            promise->addResult(response);
        } else {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QString error;
            if (statusCode == 401) {
                error = i18n("Authentication failed: Invalid API key or missing authentication");
            } else if (statusCode == 403) {
                error = i18n("Access forbidden: Check your API key and permissions");
            } else if (statusCode == 404) {
                error = i18n("Endpoint not found: Check your base URL");
            } else {
                error = reply->errorString();
            }
            // OpenAIProvider chat error: << error
            LLMResponse errResponse;
            errResponse.content = error;
            errResponse.finishReason = "error";
            promise->addResult(errResponse);
        }
        reply->deleteLater();
        delete promise;
    });
    
    return future;
}

void OpenAIProvider::chatStream(
    const std::vector<LLMMessage> &messages,
    const std::vector<ToolDefinition> &tools,
    const QString &model,
    std::function<void(const QString &chunk)> onChunk,
    std::function<void(const LLMResponse &final)> onDone,
    std::function<void(const QString &error)> onError)
{
    
    QJsonObject json;
    json["model"] = model;
    json["stream"] = true;
    
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
    
    QByteArray payload = QJsonDocument(json).toJson();
    
    QNetworkReply *reply = m_nam->post(request, payload);
    m_currentReply = reply;
    
    QObject::connect(reply, &QNetworkReply::finished, [this, reply, request, payload, onChunk, onDone, onError]() {
        m_currentReply = nullptr;
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            if (responseData.isEmpty()) {
                onError("Empty response from API");
                reply->deleteLater();
                return;
            }
            
            // Parse SSE response line by line
            QString responseText;
            QList<QByteArray> lines = responseData.split('\n');
            for (const QByteArray &line : lines) {
                if (line.startsWith("data: ")) {
                    QByteArray jsonData = line.mid(6);
                    if (jsonData.trimmed() == "[DONE]") {
                        continue;
                    }
                    QJsonParseError parseError;
                    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
                    if (parseError.error != QJsonParseError::NoError) {
                        continue;
                    }
                    QJsonObject root = doc.object();
                    if (root.contains("choices") && root["choices"].isArray()) {
                        QJsonArray choices = root["choices"].toArray();
                        if (!choices.isEmpty()) {
                            QJsonObject choice = choices.at(0).toObject();
                            QJsonObject delta = choice["delta"].toObject();
                            QString content = delta["content"].toString();
                            if (!content.isEmpty()) {
                                responseText += content;
                                if (onChunk) {
                                    onChunk(content);
                                }
                            }
                        }
                    }
                }
            }
            LLMResponse response;
            response.content = responseText;
            onDone(response);
        } else {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            
            // Check if we should retry (429 or 500 errors, max 3 retries)
            bool shouldRetry = (statusCode == 429 || statusCode == 500) && m_retryCount < MAX_RETRIES;
            
            if (shouldRetry) {
                m_retryCount++;
                int delay = 2000 * m_retryCount;
                
                QNetworkRequest retryRequest = request;
                QByteArray retryPayload = payload;
                auto retryOnChunk = onChunk;
                auto retryOnDone = onDone;
                auto retryOnError = onError;
                
                QTimer::singleShot(delay, this, [this, retryRequest, retryPayload, retryOnChunk, retryOnDone, retryOnError]() {
                    QNetworkReply *retryReply = m_nam->post(retryRequest, retryPayload);
                    m_currentReply = retryReply;
                    
                    QObject::connect(retryReply, &QNetworkReply::finished, [this, retryReply, retryOnChunk, retryOnDone, retryOnError]() {
                        m_currentReply = nullptr;
                        m_retryCount = 0;
                        if (retryReply->error() == QNetworkReply::NoError) {
                            QByteArray responseData = retryReply->readAll();
                            QString responseText;
                            QList<QByteArray> lines = responseData.split('\n');
                            for (const QByteArray &line : lines) {
                                if (line.startsWith("data: ")) {
                                    QByteArray jsonData = line.mid(6);
                                    if (jsonData.trimmed() == "[DONE]") continue;
                                    QJsonParseError parseError;
                                    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
                                    if (parseError.error != QJsonParseError::NoError) continue;
                                    QJsonObject root = doc.object();
                                    if (root.contains("choices") && root["choices"].isArray()) {
                                        QJsonArray choices = root["choices"].toArray();
                                        if (!choices.isEmpty()) {
                                            QJsonObject delta = choices.at(0).toObject()["delta"].toObject();
                                            QString content = delta["content"].toString();
                                            if (!content.isEmpty()) {
                                                responseText += content;
                                                if (retryOnChunk) retryOnChunk(content);
                                            }
                                        }
                                    }
                                }
                            }
                            LLMResponse response;
                            response.content = responseText;
                            retryOnDone(response);
                        } else {
                            retryOnError(retryReply->errorString());
                        }
                        retryReply->deleteLater();
                    });
                });
                
                reply->deleteLater();
                return;
            }
            
            m_retryCount = 0; // Reset on non-retryable error
            QString err = reply->errorString();
            QString errorWithUrl = QString("[%1] %2").arg(m_baseUrl).arg(err);
            onError(errorWithUrl);
        }
        reply->deleteLater();
    });
}

void OpenAIProvider::abort()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply = nullptr;
    }
}
