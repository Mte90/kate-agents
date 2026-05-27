#ifndef AGENTENGINE_H
#define AGENTENGINE_H

#include "llmprovider.h"
#include <QNetworkAccessManager>
#include <QTimer>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFuture>
#include <QPromise>
#include <QEventLoop>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <functional>
#include <vector>
#include <memory>

class AgentEngine : public LLMProvider
{
    Q_OBJECT

public:
    explicit AgentEngine(const QString &baseUrl, const QString &apiKey, QObject *parent = nullptr);
    ~AgentEngine() override = default;

    QString name() const override { return m_name; }
    void setName(const QString &name) { m_name = name; }
    
    bool isAvailable() override;
    QStringList availableModels() override;

    QFuture<LLMResponse> chat(
        const std::vector<LLMMessage> &messages,
        const std::vector<ToolDefinition> &tools,
        const QString &model,
        double temperature = 0.7
    ) override;

    void chatStream(
        const std::vector<LLMMessage> &messages,
        const std::vector<ToolDefinition> &tools,
        const QString &model,
        std::function<void(const QString &chunk)> onChunk,
        std::function<void(const LLMResponse &final)> onDone,
        std::function<void(const QString &error)> onError
    ) override;

    void abort() override;

    void setBaseUrl(const QString &url) { m_baseUrl = url; }
    void setApiKey(const QString &key) { m_apiKey = key; }
    void setDefaultModel(const QString &model) { m_defaultModel = model; }
    
    void updateConfig(const QString &baseUrl, const QString &apiKey) {
        m_baseUrl = baseUrl;
        m_apiKey = apiKey;
    }

signals:
    void tokenReceived(const QString &chunk);
    void responseComplete(const LLMResponse &response);
    void errorOccurred(const QString &error);

private slots:

private:
    QString prepareChatPayload(
        const std::vector<LLMMessage> &messages,
        const std::vector<ToolDefinition> &tools,
        const QString &model,
        double temperature
    );

    void processSseResponse(
        const QByteArray &responseData,
        std::function<void(const QString &chunk)> onChunk,
        std::function<void(const LLMResponse &final)> onDone,
        std::function<void(const QString &error)> onError
    );

    QString buildErrorString(int statusCode, const QString &baseError);

    QString m_name;
    QString m_baseUrl;
    QString m_apiKey;
    QString m_defaultModel;
    QNetworkAccessManager *m_nam;
    QNetworkReply *m_currentReply = nullptr;
    QByteArray m_lineBuffer;
    int m_retryCount = 0;
    static const int MAX_RETRIES = 3;
};

#endif
