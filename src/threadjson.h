#ifndef THREADJSON_H
#define THREADJSON_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "llmprovider.h"

class ThreadJsonStorage
{
public:
    static QString getThreadDir();
    static QString saveThread(const QString &threadId, const QList<LLMMessage> &messages, const QString &title);
    static QList<LLMMessage> loadThread(const QString &threadId);
    static QStringList listThreads();
    static bool deleteThread(const QString &threadId);

private:
    static QString getThreadPath(const QString &threadId);
    static QJsonObject messagesToJson(const QList<LLMMessage> &messages);
    static QList<LLMMessage> jsonToMessages(const QJsonObject &json);
};

#endif // THREADJSON_H
