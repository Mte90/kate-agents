#include "mockthreadstorage.h"

MockThreadStorage::MockThreadStorage()
{
}

QStringList MockThreadStorage::listThreads()
{
    return m_threads.keys();
}

QStringList MockThreadStorage::listThreadsForProject(const QString &projectId)
{
    Q_UNUSED(projectId);
    return m_threads.keys();
}

QList<LLMMessage> MockThreadStorage::loadThread(const QString &threadId)
{
    return m_threads.value(threadId);
}

QString MockThreadStorage::loadThreadTitle(const QString &threadId)
{
    return m_titles.value(threadId);
}

bool MockThreadStorage::saveThread(const QString &threadId,
                                    const QList<LLMMessage> &messages,
                                    const QString &currentModel,
                                    const QString &title)
{
    m_threads.insert(threadId, messages);
    if (!title.isEmpty()) {
        m_titles.insert(threadId, title);
    }
    if (!currentModel.isEmpty()) {
        m_models.insert(threadId, currentModel);
    }
    return true;
}

bool MockThreadStorage::deleteThread(const QString &threadId)
{
    m_threads.remove(threadId);
    m_titles.remove(threadId);
    m_models.remove(threadId);
    return true;
}

void MockThreadStorage::clear()
{
    m_threads.clear();
    m_titles.clear();
    m_models.clear();
}