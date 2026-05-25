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
    static void setCurrentProjectIdFromFile(const QString &filePath);
    static QString getProjectPrefix(const QString &projectId);
    static QString getThreadFilePath(const QString &projectId);
    
    // Thread operations (single file per project)
    static QStringList listThreads();
    static QStringList listThreadsForProject(const QString &projectId);
    static QList<LLMMessage> loadThread(const QString &threadId);
    static QString loadThreadTitle(const QString &threadId);
    static bool saveThread(const QString &threadId, const QList<LLMMessage> &messages, const QString &currentModel = QString(), const QString &title = QString());
    static bool deleteThread(const QString &threadId);

private:
    static QString detectGitRepoRoot();
    static QString detectGitRepoRootFromDir(const QString &startDir);
    static QString getRepoName(const QString &repoPath);
    static QString getProjectIdFromFile(const QString &filePath);

    static QString s_currentProjectId;
};

#endif // THREADJSON_H
