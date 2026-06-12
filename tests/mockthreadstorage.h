#ifndef MOCKTHREADSTORAGE_H
#define MOCKTHREADSTORAGE_H

#include "ithreadstorage.h"
#include <QMap>
#include <QString>
#include <QList>

class MockThreadStorage : public IThreadStorage
{
public:
    MockThreadStorage();
    ~MockThreadStorage() override = default;

    QStringList listThreads() override;
    QStringList listThreadsForProject(const QString &projectId) override;
    QList<LLMMessage> loadThread(const QString &threadId) override;
    QString loadThreadTitle(const QString &threadId) override;
    bool saveThread(const QString &threadId,
                    const QList<LLMMessage> &messages,
                    const QString &currentModel = QString(),
                    const QString &title = QString()) override;
    bool deleteThread(const QString &threadId) override;
    void clear();

private:
    QMap<QString, QList<LLMMessage>> m_threads;
    QMap<QString, QString> m_titles;
    QMap<QString, QString> m_models;
};

#endif