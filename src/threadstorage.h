#ifndef THREADSTORAGE_H
#define THREADSTORAGE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <vector>

#include "llmprovider.h"
#include "ithreadstorage.h"

class ThreadStorage : public QObject
{
    Q_OBJECT

public:
    explicit ThreadStorage(QObject *parent = nullptr);
    ~ThreadStorage() override;

    bool initialize();
    QMap<QString, ConversationThread> loadAllThreads();
    QMap<QString, ConversationThread> loadThreadsForProject(const QString &projectId);
    bool saveThread(const ConversationThread &thread);
    bool saveAllThreads(const QMap<QString, ConversationThread> &threads);
    bool deleteThread(const QString &threadId);
    void setCurrentProjectId(const QString &projectId);

    void setStorageBackend(IThreadStorage *storage);

private:
    QString databasePath() const;
    QString m_currentProjectId;
    IThreadStorage *m_storage;
};

#endif // THREADSTORAGE_H