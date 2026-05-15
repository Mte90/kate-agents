#include "threadjson.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QProcess>
#include <QCryptographicHash>

// Static member initialization
QString ThreadJsonStorage::s_currentProjectId;

QString ThreadJsonStorage::getThreadDir()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kate/agents";
    QDir().mkpath(dir);
    return dir;
}

QString ThreadJsonStorage::getProjectPrefix(const QString &projectId)
{
    if (projectId.isEmpty()) {
        return QString();
    }
    // Sanitize project ID for use in filename (replace spaces and special chars)
    QString sanitized = projectId;
    sanitized.replace(" ", "_");
    sanitized.replace("/", "_");
    sanitized.replace("\\", "_");
    sanitized.replace(":", "_");
    sanitized.replace("*", "_");
    sanitized.replace("?", "_");
    sanitized.replace("\"", "_");
    sanitized.replace("<", "_");
    sanitized.replace(">", "_");
    sanitized.replace("|", "_");
    return sanitized + "_";
}

QString ThreadJsonStorage::getCurrentProjectId()
{
    if (!s_currentProjectId.isEmpty()) {
        return s_currentProjectId;
    }

    QString gitDir = detectGitRepoRoot();
    if (!gitDir.isEmpty()) {
        s_currentProjectId = getRepoName(gitDir);
        qDebug() << "ThreadJsonStorage: Detected git project:" << s_currentProjectId;
        return s_currentProjectId;
    }

    QFileInfo info(QDir::currentPath());
    s_currentProjectId = info.fileName();
    qDebug() << "ThreadJsonStorage: Using current directory as project ID:" << s_currentProjectId;
    return s_currentProjectId;
}

QString ThreadJsonStorage::detectGitRepoRoot()
{
    // Try to find git repo using git rev-parse --show-toplevel
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("git", QStringList() << "rev-parse" << "--show-toplevel");
    
    if (process.waitForFinished(3000)) {
        QString output = QString::fromUtf8(process.readAll()).trimmed();
        if (process.exitCode() == 0 && !output.isEmpty()) {
            return output;
        }
    }
    
    return QString();
}

QString ThreadJsonStorage::getRepoName(const QString &repoPath)
{
    // Get the basename of the repo path (e.g., /path/to/my-repo -> my-repo)
    QFileInfo info(repoPath);
    return info.fileName();
}

void ThreadJsonStorage::setCurrentProjectId(const QString &projectId)
{
    s_currentProjectId = projectId;
    qDebug() << "ThreadJsonStorage: Set project ID to:" << s_currentProjectId;
}

QString ThreadJsonStorage::getThreadPath(const QString &threadId)
{
    QString projectId = getCurrentProjectId();
    QString prefix = getProjectPrefix(projectId);
    
    // If threadId already has a project prefix, use it as-is
    if (!prefix.isEmpty() && threadId.startsWith(prefix)) {
        return getThreadDir() + "/" + threadId + ".json";
    }
    
    // Otherwise, prepend the project prefix
    return getThreadDir() + "/" + prefix + threadId + ".json";
}

QJsonObject ThreadJsonStorage::messagesToJson(const QList<LLMMessage> &messages)
{
    QJsonObject root;
    QJsonArray msgArray;

    for (const auto &msg : messages) {
        QJsonObject msgObj;
        msgObj["role"] = msg.role;
        msgObj["content"] = msg.content;
        msgArray.append(msgObj);
    }

    root["messages"] = msgArray;
    return root;
}

QList<LLMMessage> ThreadJsonStorage::jsonToMessages(const QJsonObject &json)
{
    QList<LLMMessage> messages;

    if (!json.contains("messages") || !json["messages"].isArray()) {
        return messages;
    }

    QJsonArray msgArray = json["messages"].toArray();
    for (const auto &value : msgArray) {
        QJsonObject msgObj = value.toObject();
        LLMMessage msg;
        msg.role = msgObj["role"].toString();
        msg.content = msgObj["content"].toString();
        messages.append(msg);
    }

    return messages;
}

QString ThreadJsonStorage::saveThread(const QString &threadId, const QList<LLMMessage> &messages, const QString &title)
{
    QString projectId = getCurrentProjectId();
    QString prefix = getProjectPrefix(projectId);
    
    // Don't prepend prefix if threadId already starts with it
    QString fullThreadId = threadId;
    if (!prefix.isEmpty() && !threadId.startsWith(prefix)) {
        fullThreadId = prefix + threadId;
    }
    QString path = getThreadDir() + "/" + fullThreadId + ".json";
    
    QJsonObject root = messagesToJson(messages);
    root["title"] = title;
    root["projectId"] = projectId;  // Store project ID in JSON for metadata
    
    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Cannot write thread:" << path;
        return QString();
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return path;
}

QList<LLMMessage> ThreadJsonStorage::loadThread(const QString &threadId)
{
    // Try with current project prefix first
    QString projectId = getCurrentProjectId();
    QString prefix = getProjectPrefix(projectId);
    QString fullThreadId = prefix + threadId;
    QString path = getThreadDir() + "/" + fullThreadId + ".json";
    
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        // Try without prefix (backward compatibility for old files)
        if (!prefix.isEmpty() && threadId.startsWith(prefix)) {
            path = getThreadDir() + "/" + threadId + ".json";
            file.setFileName(path);
        }
        
        if (!file.open(QIODevice::ReadOnly)) {
            return QList<LLMMessage>();
        }
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return QList<LLMMessage>();
    }

    return jsonToMessages(doc.object());
}

QStringList ThreadJsonStorage::listThreads()
{
    // List all threads (backward compatibility - includes all projects)
    QStringList threads;
    QString dirPath = getThreadDir();

    QDir dir(dirPath);
    QFileInfoList entries = dir.entryInfoList(QStringList() << "*.json", QDir::Files, QDir::Name);

    for (const QFileInfo &entry : entries) {
        QString threadId = entry.completeBaseName();
        if (threadId != "config") {
            threads.append(threadId);
        }
    }

    return threads;
}

QStringList ThreadJsonStorage::listThreadsForProject(const QString &projectId)
{
    QStringList threads;
    QString dirPath = getThreadDir();
    QString prefix = getProjectPrefix(projectId);

    QDir dir(dirPath);
    QFileInfoList entries = dir.entryInfoList(QStringList() << "*.json", QDir::Files, QDir::Name);

    for (const QFileInfo &entry : entries) {
        QString threadId = entry.completeBaseName();
        
        // Skip non-project files
        if (threadId == "config") {
            continue;
        }
        
        // If project ID provided, match by prefix
        if (!prefix.isEmpty()) {
            if (threadId.startsWith(prefix)) {
                // Remove prefix from thread ID
                QString threadIdWithoutPrefix = threadId.mid(prefix.length());
                threads.append(threadIdWithoutPrefix);
            }
        } else {
            // No project ID - include files without any project prefix
            // (backward compatibility for old global chats)
            if (!threadId.contains("_chat_")) {
                // This might be an old-style global chat, include it
                threads.append(threadId);
            }
        }
    }

    return threads;
}

bool ThreadJsonStorage::deleteThread(const QString &threadId)
{
    // Try with current project prefix
    QString projectId = getCurrentProjectId();
    QString prefix = getProjectPrefix(projectId);
    QString fullThreadId = prefix + threadId;
    QString path = getThreadDir() + "/" + fullThreadId + ".json";
    
    if (QFile::remove(path)) {
        return true;
    }
    
    // Try without prefix (backward compatibility)
    if (!prefix.isEmpty() && threadId.startsWith(prefix)) {
        path = getThreadDir() + "/" + threadId + ".json";
        return QFile::remove(path);
    }
    
    return false;
}