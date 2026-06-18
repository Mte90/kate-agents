#include "auditlogger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>

AuditLogger::AuditLogger(QObject *parent)
    : QObject(parent)
{
}

AuditLogger::~AuditLogger()
{
    if (m_logFile && m_logFile->isOpen()) {
        m_logFile->close();
        delete m_logFile;
    }
}

void AuditLogger::setProjectId(const QString &projectId)
{
    m_projectId = projectId;
    // Reopen log file for new project
    if (m_logFile) {
        delete m_logFile;
        m_logFile = nullptr;
    }
}

void AuditLogger::setThreadId(const QString &threadId)
{
    m_threadId = threadId;
}

void AuditLogger::logToolExecution(const QString &toolName, const QJsonObject &args, bool success, const QString &error)
{
    AuditEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.action = "tool_executed";
    entry.toolName = toolName;
    entry.success = success;
    entry.errorMessage = error;
    entry.threadId = m_threadId;
    entry.projectId = m_projectId;
    entry.details = args;
    
    writeEntry(entry);
}

void AuditLogger::logApiCall(const QString &model, int promptTokens, int completionTokens)
{
    AuditEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.action = "api_call";
    entry.success = true;
    entry.threadId = m_threadId;
    entry.projectId = m_projectId;
    entry.details = {
        {"model", model},
        {"prompt_tokens", promptTokens},
        {"completion_tokens", completionTokens}
    };
    
    writeEntry(entry);
}

void AuditLogger::logPermissionDecision(const QString &toolName, bool granted)
{
    AuditEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.action = "permission_decision";
    entry.toolName = toolName;
    entry.success = granted;
    entry.threadId = m_threadId;
    entry.projectId = m_projectId;
    entry.details = {
        {"granted", granted}
    };
    
    writeEntry(entry);
}

void AuditLogger::logError(const QString &action, const QString &error)
{
    AuditEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.action = "error";
    entry.success = false;
    entry.errorMessage = error;
    entry.threadId = m_threadId;
    entry.projectId = m_projectId;
    entry.details = {
        {"action", action}
    };
    
    writeEntry(entry);
}

QString AuditLogger::getLogFilePath() const
{
    QString projectId = m_projectId;
    if (projectId.isEmpty()) {
        projectId = "default";
    }
    
    QString logDir = QDir::homePath() + "/.local/share/kate-agents/audit";
    QDir().mkpath(logDir);
    
    return logDir + "/" + projectId + "_audit.jsonl";
}

void AuditLogger::writeEntry(const AuditEntry &entry)
{
    QString logPath = getLogFilePath();
    
    // Check and rotate log file if too large
    QFileInfo fileInfo(logPath);
    if (fileInfo.exists() && fileInfo.size() > m_maxLogSize) {
        // Archive old log
        QString archivePath = logPath + "." + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        QFile::rename(logPath, archivePath);
    }
    
    // Open file if not already open
    if (!m_logFile || m_logFile->fileName() != logPath) {
        if (m_logFile) {
            delete m_logFile;
        }
        m_logFile = new QFile(logPath);
        if (!m_logFile->open(QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Failed to open audit log:" << logPath;
            return;
        }
    }
    
    // Write entry as JSON line
    QJsonObject obj;
    obj["timestamp"] = entry.timestamp.toString(Qt::ISODate);
    obj["action"] = entry.action;
    if (!entry.toolName.isEmpty()) {
        obj["tool_name"] = entry.toolName;
    }
    obj["success"] = entry.success;
    if (!entry.errorMessage.isEmpty()) {
        obj["error"] = entry.errorMessage;
    }
    if (!entry.threadId.isEmpty()) {
        obj["thread_id"] = entry.threadId;
    }
    if (!entry.projectId.isEmpty()) {
        obj["project_id"] = entry.projectId;
    }
    if (!entry.details.isEmpty()) {
        obj["details"] = entry.details;
    }
    
    QTextStream out(m_logFile);
    out << QJsonDocument(obj).toJson(QJsonDocument::Compact) << "\n";
    m_logFile->flush();
}
