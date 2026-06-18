#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QFile>
#include <QJsonDocument>
#include <QDir>

struct AuditEntry {
    QDateTime timestamp;
    QString action;           // e.g., "tool_executed", "api_call", "permission_granted"
    QString toolName;         // if applicable
    QJsonObject details;      // additional context
    bool success;
    QString errorMessage;
    QString threadId;         // which conversation thread
    QString projectId;        // which project
};

class AuditLogger : public QObject
{
    Q_OBJECT

public:
    explicit AuditLogger(QObject *parent = nullptr);
    ~AuditLogger() override;

    void setProjectId(const QString &projectId);
    void setThreadId(const QString &threadId);

    void logToolExecution(const QString &toolName, const QJsonObject &args, bool success, const QString &error = QString());
    void logApiCall(const QString &model, int promptTokens, int completionTokens);
    void logPermissionDecision(const QString &toolName, bool granted);
    void logError(const QString &action, const QString &error);

private:
    void writeEntry(const AuditEntry &entry);
    QString getLogFilePath() const;

    QString m_projectId;
    QString m_threadId;
    QFile *m_logFile = nullptr;
    int m_maxLogSize = 10 * 1024 * 1024;  // 10MB max log file
};

#endif
