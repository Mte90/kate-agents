#include "threadstorage.h"
#include "threadjson.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

ThreadStorage::ThreadStorage(QObject *parent)
    : QObject(parent)
    , m_currentProjectId(QString())
    , m_storage(nullptr)
{
}

ThreadStorage::~ThreadStorage()
{
}

QString ThreadStorage::databasePath() const
{
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString kateAgentDir = dataLocation + QLatin1String("/kate/agents");
    return kateAgentDir;
}

bool ThreadStorage::initialize()
{
    QString threadDir = databasePath();
    QDir().mkpath(threadDir);
    
    if (!m_storage) {
        m_storage = &ThreadJsonStorage::instance();
    }
    return true;
}

void ThreadStorage::setStorageBackend(IThreadStorage *storage)
{
    m_storage = storage;
}

void ThreadStorage::setCurrentProjectId(const QString &projectId)
{
    m_currentProjectId = projectId;
    ThreadJsonStorage::setCurrentProjectId(projectId);
}

QMap<QString, ConversationThread> ThreadStorage::loadAllThreads()
{
    QMap<QString, ConversationThread> threads;
    
    if (!m_storage) {
        m_storage = &ThreadJsonStorage::instance();
    }
    
    QStringList threadIds = m_storage->listThreads();
    
    // Get current project prefix to strip it from loaded IDs
    QString projectId = ThreadJsonStorage::getCurrentProjectId();
    QString prefix = ThreadJsonStorage::getProjectPrefix(projectId);
    
    for (const QString &fullThreadId : threadIds) {
        // Strip prefix from thread ID if present
        QString threadId = fullThreadId;
        if (!prefix.isEmpty() && fullThreadId.startsWith(prefix)) {
            threadId = fullThreadId.mid(prefix.length());
        }
        
        QList<LLMMessage> messages = m_storage->loadThread(fullThreadId);
        
        ConversationThread thread;
        thread.id = threadId;  // Store ID without prefix
        thread.messages = messages;
        
        threads[threadId] = thread;
    }
    
    return threads;
}

QMap<QString, ConversationThread> ThreadStorage::loadThreadsForProject(const QString &projectId)
{
    QMap<QString, ConversationThread> threads;
    
    if (!m_storage) {
        m_storage = &ThreadJsonStorage::instance();
    }
    
    QString effectiveProjectId = projectId.isEmpty() ? m_currentProjectId : projectId;
    if (effectiveProjectId.isEmpty()) {
        effectiveProjectId = ThreadJsonStorage::getCurrentProjectId();
    }
    
    QString prefix = ThreadJsonStorage::getProjectPrefix(effectiveProjectId);
    QStringList fullThreadIds = m_storage->listThreadsForProject(effectiveProjectId);
    
    for (const QString &fullThreadId : fullThreadIds) {
        // Strip prefix from thread ID if present
        QString threadId = fullThreadId;
        if (!prefix.isEmpty() && fullThreadId.startsWith(prefix)) {
            threadId = fullThreadId.mid(prefix.length());
        }
        
        QList<LLMMessage> messages = m_storage->loadThread(fullThreadId);
        
        ConversationThread thread;
        thread.id = threadId;  // Store ID without prefix
        thread.messages = messages;
        
        threads[threadId] = thread;
    }
    
    return threads;
}

bool ThreadStorage::saveThread(const ConversationThread &thread)
{
    if (!m_storage) {
        m_storage = &ThreadJsonStorage::instance();
    }
    
    QList<LLMMessage> messages;
    messages.reserve(thread.messages.size());
    
    for (const auto &msg : thread.messages) {
        LLMMessage lmMsg;
        lmMsg.role = msg.role;
        lmMsg.content = msg.content;
        lmMsg.profile = msg.profile;
        lmMsg.thinking = msg.thinking;
        lmMsg.toolCallId = msg.toolCallId;
        messages.append(lmMsg);
    }
    
    return m_storage->saveThread(thread.id, messages, thread.currentModel, thread.title);
}

bool ThreadStorage::saveAllThreads(const QMap<QString, ConversationThread> &threads)
{
    for (const auto &threadPair : threads) {
        if (!saveThread(threadPair)) {
            return false;
        }
    }
    return true;
}

bool ThreadStorage::deleteThread(const QString &threadId)
{
    if (!m_storage) {
        m_storage = &ThreadJsonStorage::instance();
    }
    
    return m_storage->deleteThread(threadId);
}