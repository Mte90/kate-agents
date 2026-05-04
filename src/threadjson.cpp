#include "threadjson.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>

QString ThreadJsonStorage::getThreadDir()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kate/agents";
    QDir().mkpath(dir);
    return dir;
}

QString ThreadJsonStorage::getThreadPath(const QString &threadId)
{
    return getThreadDir() + "/" + threadId + ".json";
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
    QString path = getThreadPath(threadId);
    
    QJsonObject root = messagesToJson(messages);
    root["title"] = title;
    
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
    QString path = getThreadPath(threadId);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QList<LLMMessage>();
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

bool ThreadJsonStorage::deleteThread(const QString &threadId)
{
    QString path = getThreadPath(threadId);
    return QFile::remove(path);
}
