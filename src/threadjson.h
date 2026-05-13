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
    static QString getCurrentProjectId();
    static void setCurrentProjectId(const QString &projectId);
    static QString saveThread(const QString &threadId, const QList<LLMMessage> &messages, const QString &title);
    static QList<LLMMessage> loadThread(const QString &threadId);
    static QStringList listThreads();
    static QStringList listThreadsForProject(const QString &projectId);
    static bool deleteThread(const QString &threadId);
    static QString getThreadPath(const QString &threadId);

private:
    static QJsonObject messagesToJson(const QList<LLMMessage> &messages);
    static QList<LLMMessage> jsonToMessages(const QJsonObject &json);
    static QString getProjectPrefix(const QString &projectId);
    static QString detectGitRepoRoot();
    static QString getRepoName(const QString &repoPath);

    static QString s_currentProjectId;
};

#endif // THREADJSON_H
