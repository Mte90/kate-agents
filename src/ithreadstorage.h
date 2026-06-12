#ifndef ITHREADSTORAGE_H
#define ITHREADSTORAGE_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "llmprovider.h"

/**
 * @interface IThreadStorage
 * @brief Abstract interface for thread storage implementations.
 * 
 * This interface defines the contract for storing and retrieving
 * conversation threads. Implementations can use different backends
 * (JSON files, database, etc.) without coupling to the storage mechanism.
 */
class IThreadStorage
{
public:
    virtual ~IThreadStorage() = default;

    /**
     * @brief Load all threads for the current project.
     * @return List of thread IDs.
     */
    virtual QStringList listThreads() = 0;

    /**
     * @brief Load all threads for a specific project.
     * @param projectId The project identifier.
     * @return List of thread IDs.
     */
    virtual QStringList listThreadsForProject(const QString &projectId) = 0;

    /**
     * @brief Load a thread's messages.
     * @param threadId The thread identifier.
     * @return List of messages in the thread.
     */
    virtual QList<LLMMessage> loadThread(const QString &threadId) = 0;

    /**
     * @brief Load a thread's title.
     * @param threadId The thread identifier.
     * @return The thread's title, or empty string if not found.
     */
    virtual QString loadThreadTitle(const QString &threadId) = 0;

    /**
     * @brief Save a thread's messages and metadata.
     * @param threadId The thread identifier.
     * @param messages List of messages to save.
     * @param currentModel Optional model identifier.
     * @param title Optional thread title.
     * @return true if save succeeded, false otherwise.
     */
    virtual bool saveThread(const QString &threadId, 
                            const QList<LLMMessage> &messages, 
                            const QString &currentModel = QString(), 
                            const QString &title = QString()) = 0;

    /**
     * @brief Delete a thread.
     * @param threadId The thread identifier.
     * @return true if deletion succeeded, false otherwise.
     */
    virtual bool deleteThread(const QString &threadId) = 0;
};

#endif // ITHREADSTORAGE_H