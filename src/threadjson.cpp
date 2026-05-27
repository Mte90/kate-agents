#include "threadjson.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDebug>
#include <QProcess>

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
    QString sanitized = projectId;
    sanitized.replace(" ", "_");
    sanitized.replace("/", "_");
    sanitized.replace("\\", "_");
    sanitized.replace(":", "_");
    sanitized.replace("*", "_");
    sanitized.replace("?", "_");
    sanitized.replace('"', "_");
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
    if (gitDir.isEmpty()) {
        s_currentProjectId = "default";
    } else {
        s_currentProjectId = getRepoName(gitDir);
    }
    return s_currentProjectId;
}

QString ThreadJsonStorage::getThreadFilePath(const QString &projectId)
{
    QString prefix = getProjectPrefix(projectId);
    return getThreadDir() + "/" + prefix + "threads.json";
}

static QJsonObject loadThreadsFile(const QString &projectId)
{
    QString filePath = ThreadJsonStorage::getThreadFilePath(projectId);
    QFile file(filePath);
    
    if (!file.exists()) {
        return QJsonObject();
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        // Failed to open threads file for reading: << filePath
        return QJsonObject();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    if (data.isEmpty()) {
        return QJsonObject();
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return QJsonObject();
    }
    
    return doc.object();
}

static bool saveThreadsFile(const QString &projectId, const QJsonObject &data)
{
    QString filePath = ThreadJsonStorage::getThreadFilePath(projectId);
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        // ThreadJsonStorage::saveThreadsFile - Failed to open threads file for writing: << filePath
        return false;
    }
    
    QJsonDocument doc(data);
    QByteArray jsonBytes = doc.toJson(QJsonDocument::Indented);
    
    file.write(jsonBytes);
    file.close();
    
    
    // Verify the file was actually written
    QFile verifyFile(filePath);
    if (verifyFile.open(QIODevice::ReadOnly)) {
        QByteArray verifyData = verifyFile.readAll();
        verifyFile.close();
    }
    
    return true;
}

QStringList ThreadJsonStorage::listThreads()
{
    return listThreadsForProject(getCurrentProjectId());
}

QStringList ThreadJsonStorage::listThreadsForProject(const QString &projectId)
{
    QStringList threads;
    QJsonObject root = loadThreadsFile(projectId);
    
    // Get threads object
    QString prefix = getProjectPrefix(projectId);
    QString threadsKey = prefix + "threads";
    
    if (root.contains(threadsKey) && root[threadsKey].isObject()) {
        QJsonObject threadsObj = root[threadsKey].toObject();
        threads = threadsObj.keys();
    }
    
    return threads;
}

QList<LLMMessage> ThreadJsonStorage::loadThread(const QString &threadId)
{
    QList<LLMMessage> messages;
    QString projectId = getCurrentProjectId();
    QJsonObject root = loadThreadsFile(projectId);
    
    QString prefix = getProjectPrefix(projectId);
    QString threadsKey = prefix + "threads";
    
    if (!root.contains(threadsKey) || !root[threadsKey].isObject()) {
        return messages;
    }
    
    QJsonObject threadsObj = root[threadsKey].toObject();
    if (!threadsObj.contains(threadId) || !threadsObj[threadId].isObject()) {
        return messages;
    }
    
    QJsonObject threadObj = threadsObj[threadId].toObject();
    QJsonArray messagesArray = threadObj["messages"].toArray();
    
    for (const QJsonValue &msgValue : messagesArray) {
        if (!msgValue.isObject()) continue;
        
        QJsonObject msgObj = msgValue.toObject();
        LLMMessage msg;
        
        msg.role = msgObj["role"].toString();
        msg.content = msgObj["content"].toString();
        msg.profile = msgObj["profile"].toString();
        msg.toolCallId = msgObj["toolCallId"].toString();
        
        messages.append(msg);
    }
    
    return messages;
}

QString ThreadJsonStorage::loadThreadTitle(const QString &threadId)
{
    QString projectId = getCurrentProjectId();
    QJsonObject root = loadThreadsFile(projectId);
    
    QString prefix = getProjectPrefix(projectId);
    QString threadsKey = prefix + "threads";
    
    if (!root.contains(threadsKey) || !root[threadsKey].isObject()) {
        return QString();
    }
    
    QJsonObject threadsObj = root[threadsKey].toObject();
    if (!threadsObj.contains(threadId) || !threadsObj[threadId].isObject()) {
        return QString();
    }
    
    QJsonObject threadObj = threadsObj[threadId].toObject();
    return threadObj["title"].toString();
}

bool ThreadJsonStorage::saveThread(const QString &threadId, const QList<LLMMessage> &messages, const QString &currentModel, const QString &title)
{
    QString projectId = getCurrentProjectId();
    QJsonObject root = loadThreadsFile(projectId);
    
    QString prefix = getProjectPrefix(projectId);
    QString threadsKey = prefix + "threads";
    
    if (!root.contains(threadsKey) || !root[threadsKey].isObject()) {
        root[threadsKey] = QJsonObject();
    }
    
    QJsonObject threadsObj = root[threadsKey].toObject();
    QJsonArray messagesArray;
    
    for (const LLMMessage &msg : messages) {
        QJsonObject msgObj;
        msgObj["role"] = msg.role;
        msgObj["content"] = msg.content;
        msgObj["profile"] = msg.profile;
        msgObj["toolCallId"] = msg.toolCallId;
        
        messagesArray.append(msgObj);
    }
    
    // Save currentModel if provided
    QJsonObject threadObj;
    threadObj["messages"] = messagesArray;
    if (!currentModel.isEmpty()) {
        threadObj["currentModel"] = currentModel;
    }
    // Save title if provided
    if (!title.isEmpty()) {
        threadObj["title"] = title;
    }
    
    threadsObj[threadId] = threadObj;
    root[threadsKey] = threadsObj;
    
    return saveThreadsFile(projectId, root);
}

bool ThreadJsonStorage::deleteThread(const QString &threadId)
{
    QString projectId = getCurrentProjectId();
    
    QJsonObject root = loadThreadsFile(projectId);
    
    QString prefix = getProjectPrefix(projectId);
    QString threadsKey = prefix + "threads";
    
    
    if (!root.contains(threadsKey) || !root[threadsKey].isObject()) {
        // ThreadJsonStorage::deleteThread - threads key not found or not an object
        return false;
    }
    
    QJsonObject threadsObj = root[threadsKey].toObject();
    
    if (!threadsObj.contains(threadId)) {
        // ThreadJsonStorage::deleteThread - threadId not found in storage
        return false;
    }
    
    threadsObj.remove(threadId);
    root[threadsKey] = threadsObj;
    
    
    bool saved = saveThreadsFile(projectId, root);
    
    return saved;
}

QString ThreadJsonStorage::detectGitRepoRoot()
{
    return detectGitRepoRootFromDir(QDir::currentPath());
}

QString ThreadJsonStorage::detectGitRepoRootFromDir(const QString &startDir)
{
    if (startDir.isEmpty()) {
        return QString();
    }
    
    QProcess process;
    process.setWorkingDirectory(startDir);
    process.start("git", QStringList() << "rev-parse" << "--show-toplevel");
    process.waitForFinished(1000);
    
    if (process.exitCode() == 0) {
        return QString(process.readAllStandardOutput()).trimmed();
    }
    
    return QString();
}

QString ThreadJsonStorage::getProjectIdFromFile(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return "default";
    }
    
    QFileInfo fi(filePath);
    QString dir = fi.absoluteDir().absolutePath();
    
    // Search up the directory tree for a git repo
    while (!dir.isEmpty()) {
        QString gitDir = dir + "/.git";
        if (QDir(gitDir).exists() || QFile(gitDir).exists()) {
            // Found git repo, get its name
            QFileInfo dirInfo(dir);
            return dirInfo.fileName();
        }
        
        // Move to parent directory
        QString parentDir = QDir(dir).absolutePath();
        if (parentDir == dir) {
            // Reached root, no git repo found
            break;
        }
        dir = parentDir;
    }
    
    return "default";
}

QString ThreadJsonStorage::getRepoName(const QString &gitDir)
{
    QFileInfo fi(gitDir);
    return fi.fileName();
}

void ThreadJsonStorage::setCurrentProjectId(const QString &projectId)
{
    s_currentProjectId = projectId;
}

void ThreadJsonStorage::setCurrentProjectIdFromFile(const QString &filePath)
{
    s_currentProjectId = getProjectIdFromFile(filePath);
}