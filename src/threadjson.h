#ifndef THREADJSON_H
#define THREADJSON_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "llmprovider.h"
#include "ithreadstorage.h"

class ThreadJsonStorage : public IThreadStorage
{
public:
    static ThreadJsonStorage &instance();
    
    static QString getThreadDir();
    static QString getCurrentProjectId();
    static void setCurrentProjectId(const QString &projectId);
    static void setCurrentProjectIdFromFile(const QString &filePath);
    static QString getProjectPrefix(const QString &projectId);
    static QString getThreadFilePath(const QString &projectId);

    // IThreadStorage interface implementation
    QStringList listThreads() override;
    QStringList listThreadsForProject(const QString &projectId) override;
    QList<LLMMessage> loadThread(const QString &threadId) override;
    QString loadThreadTitle(const QString &threadId) override;
    bool saveThread(const QString &threadId, const QList<LLMMessage> &messages, const QString &currentModel = QString(), const QString &title = QString()) override;
    bool deleteThread(const QString &threadId) override;

private:
    static QString detectGitRepoRoot();
    static QString detectGitRepoRootFromDir(const QString &startDir);
    static QString getRepoName(const QString &repoPath);
    static QString getProjectIdFromFile(const QString &filePath);

    static QString s_currentProjectId;
};

#endif // THREADJSON_H