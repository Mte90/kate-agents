#ifndef LLMPROVIDER_H
#define LLMPROVIDER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QFuture>
#include <QDateTime>
#include <QList>

struct LLMMessage {
    QString role;
    QString content;
    QString toolCallId;
    
    LLMMessage() = default;
    LLMMessage(const LLMMessage&) = default;
    LLMMessage& operator=(const LLMMessage&) = default;
};

struct ToolCall {
    QString id;
    QString name;
    QJsonObject arguments;
};

struct ToolDefinition {
    QString type;
    struct Function {
        QString name;
        QString description;
        QJsonObject parameters;
    } function;
};

struct LLMResponse {
    QString content;
    QString finishReason;
    std::vector<ToolCall> toolCalls;
    int promptTokens = 0;
    int completionTokens = 0;
};

struct ConversationThread {
    QString id;
    QString title;
    QDateTime createdAt;
    QDateTime updatedAt;
    QList<LLMMessage> messages;
    QString currentModel;
    bool isActive = false;
};

class LLMProvider : public QObject
{
    Q_OBJECT

public:
    explicit LLMProvider(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~LLMProvider() = default;

    virtual QString name() const = 0;
    virtual bool isAvailable() = 0;
    virtual QStringList availableModels() = 0;

    virtual QFuture<LLMResponse> chat(
        const std::vector<LLMMessage> &messages,
        const std::vector<ToolDefinition> &tools,
        const QString &model,
        double temperature = 0.7
    ) = 0;

    virtual void chatStream(
        const std::vector<LLMMessage> &messages,
        const std::vector<ToolDefinition> &tools,
        const QString &model,
        std::function<void(const QString &chunk)> onChunk,
        std::function<void(const LLMResponse &final)> onDone,
        std::function<void(const QString &error)> onError
    ) = 0;

signals:
    void modelListChanged();
};

#endif