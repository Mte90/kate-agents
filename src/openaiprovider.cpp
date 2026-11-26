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
#include <QByteArray>
#include <QRegularExpression>
#include <KLocalizedString>
#include <QtConcurrent>
#include <KConfigGroup>
#include <KSharedConfig>


OpenAIProvider::OpenAIProvider(const QString &baseUrl, const QString &apiKey, QObject *parent)
    : LLMProvider(parent)
    , m_baseUrl(baseUrl)
    , m_apiKey(apiKey)
    , m_nam(new QNetworkAccessManager(this))
    , m_rateLimitSemaphore(2)  // Allow max 2 concurrent requests
{
}

OpenAIProvider::~OpenAIProvider() = default;

bool OpenAIProvider::isAvailable()
{
    return !m_baseUrl.isEmpty() && !m_apiKey.isEmpty();
}

QStringList OpenAIProvider::availableModels()
{

    KConfigGroup group(KSharedConfig::openConfig(), "KateAgent");
    
    QStringList models = group.readEntry("Models", QStringList());

    return models;

/*
// Old approach that did not work
    // Fetch models from the API (synchronous for now)
    // In a real implementation, this should be async
    if (m_baseUrl.isEmpty() || m_apiKey.isEmpty()) {
        return QStringList();  // Empty - user must configure
    }

    if (!m_baseUrl.endsWith("/v1")) {
        m_baseUrl += "/v1";
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
    
    // Wait for response (timeout after 10 seconds)
    QTimer::singleShot(10000, &loop, [&loop, &reply]() {
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
        
        return models;  // Return empty if no models from API
    }
    
    return QStringList();  // Empty - user must configure
*/
}

QFuture<LLMResponse> OpenAIProvider::chat(
    const std::vector<LLMMessage> &messages,
    const std::vector<ToolDefinition> &tools,
    const QString &model,
    double temperature
)
{
    Q_UNUSED(temperature);
    return QtConcurrent::run([this, messages = messages, tools = tools, model]() {
        LLMResponse result;
        chatStream(messages, tools, model,
            [](const QString &) {},
            [&result](const LLMResponse &final) {
                result = final;
            },
            [&result](const QString &error) {
                result.error = error;
            }
        );
        return result;
    });
}

QJsonArray OpenAIProvider::buildToolsJson(const std::vector<ToolDefinition> &tools)
{
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
    return toolsArray;
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
        json["tools"] = buildToolsJson(tools);
    }

    QString base = m_baseUrl.trimmed();
    while (base.endsWith('/')) {
        base.chop(1);
    }

    constexpr auto kCompletionsSuffix = QLatin1String("/chat/completions");

    if (base.endsWith(kCompletionsSuffix)) {
        base.chop(kCompletionsSuffix.size());
        while (base.endsWith('/')) {
            base.chop(1);
        }
    }
    QUrl url(base + kCompletionsSuffix);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QByteArray payload = QJsonDocument(json).toJson();

    m_rateLimitSemaphore.acquire();

    QNetworkReply *reply = m_nam->post(request, payload);
    m_currentReply = reply;

    struct StreamBuffers {
        QString responseText;
        QString reasoningText;
    };
    auto buffers = std::make_shared<StreamBuffers>();
    QPointer<OpenAIProvider> safeThis(this);

    m_lineBuffer.clear();

    bool readyReadConnected = QObject::connect(reply, &QNetworkReply::readyRead, [safeThis, buffers, onChunk, reply]() {
        if (reply->error() != QNetworkReply::NoError) return;
        if (!safeThis) return;

        safeThis->m_lineBuffer += reply->readAll();

        while (true) {
            int pos = safeThis->m_lineBuffer.indexOf('\n');
            if (pos == -1) break;

            QByteArray line = safeThis->m_lineBuffer.left(pos);
            safeThis->m_lineBuffer = safeThis->m_lineBuffer.mid(pos + 1);

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
                        QJsonObject choice = choices.at(0).toObject();
                        QJsonObject delta = choice["delta"].toObject();
                        QString content = delta["content"].toString();
                        QString reasoningContent = delta["reasoning_content"].toString();

                        if (!reasoningContent.isEmpty()) {
                            buffers->reasoningText += reasoningContent;
                        }
                        if (!content.isEmpty()) {
                            buffers->responseText += content;
                            if (safeThis && onChunk) onChunk(content);
                        }
                    }
                }
            }
        }
    });

    if (!readyReadConnected) {
        onError("Failed to connect readyRead signal");
        reply->deleteLater();
        m_rateLimitSemaphore.release();
        return;
    }
    QObject::connect(reply, &QNetworkReply::finished, [safeThis, reply, buffers, onChunk, onDone, onError]() {
        if (!safeThis) {
            reply->deleteLater();
            return;
        }

        safeThis->m_rateLimitSemaphore.release();
        safeThis->m_currentReply = nullptr;
        safeThis->m_retryCount = 0;

        // Process remaining buffered data before clearing
        if (!safeThis->m_lineBuffer.isEmpty()) {
            const QList<QByteArray> lines = safeThis->m_lineBuffer.split('\n');
            safeThis->m_lineBuffer.clear();
            for (const QByteArray &line : lines) {
                if (line.startsWith("data: ")) {
                    const QByteArray jsonData = line.mid(6);
                    if (jsonData.trimmed() == "[DONE]") continue;
                    QJsonParseError parseError;
                    const QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
                    if (parseError.error != QJsonParseError::NoError) continue;
                    const QJsonObject root = doc.object();
                    if (root.contains("choices") && root["choices"].isArray()) {
                        const QJsonArray choices = root["choices"].toArray();
                        if (!choices.isEmpty()) {
                            const QJsonObject delta = choices.at(0).toObject()["delta"].toObject();
                            const QString content = delta["content"].toString();
                            if (!content.isEmpty() && onChunk) onChunk(content);
                        }
                    }
                }
            }
        }

        if (reply->error() == QNetworkReply::NoError) {
            LLMResponse response;
            response.content = buffers->responseText;
            response.thinking = buffers->reasoningText;
            if (onDone) onDone(response);
        } else {
            if (onError) onError(reply->errorString());
        }
        reply->deleteLater();
    });}

void OpenAIProvider::abort()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply = nullptr;
    }
}
