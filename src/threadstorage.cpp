#include "threadstorage.h"
#include "threadjson.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

ThreadStorage::ThreadStorage(QObject *parent)
    : QObject(parent)
    , m_currentProjectId(QString())
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
    return true;
}

void ThreadStorage::setCurrentProjectId(const QString &projectId)
{
    m_currentProjectId = projectId;
    ThreadJsonStorage::setCurrentProjectId(projectId);
}

QMap<QString, ConversationThread> ThreadStorage::loadAllThreads()
{
    QMap<QString, ConversationThread> threads;
    QStringList threadIds = ThreadJsonStorage::listThreads();
    
    for (const QString &threadId : threadIds) {
        QList<LLMMessage> messages = ThreadJsonStorage::loadThread(threadId);
        
        ConversationThread thread;
        thread.id = threadId;
        thread.messages = messages;
        
        threads[threadId] = thread;
    }
    
    return threads;
}

QMap<QString, ConversationThread> ThreadStorage::loadThreadsForProject(const QString &projectId)
{
    QMap<QString, ConversationThread> threads;
    
    QString effectiveProjectId = projectId.isEmpty() ? m_currentProjectId : projectId;
    if (effectiveProjectId.isEmpty()) {
        effectiveProjectId = ThreadJsonStorage::getCurrentProjectId();
    }
    
    QStringList threadIds = ThreadJsonStorage::listThreadsForProject(effectiveProjectId);
    
    for (const QString &threadId : threadIds) {
        QList<LLMMessage> messages = ThreadJsonStorage::loadThread(threadId);
        
        ConversationThread thread;
        thread.id = threadId;
        thread.messages = messages;
        
        threads[threadId] = thread;
    }
    
    return threads;
}

bool ThreadStorage::saveThread(const ConversationThread &thread)
{
    QList<LLMMessage> messages;
    messages.reserve(thread.messages.size());
    
    for (const auto &msg : thread.messages) {
        LLMMessage lmMsg;
        lmMsg.role = msg.role;
        lmMsg.content = msg.content;
        lmMsg.toolCallId = msg.toolCallId;
        messages.append(lmMsg);
    }
    
    QString path = ThreadJsonStorage::saveThread(thread.id, messages, thread.title);
    return !path.isEmpty();
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
    return ThreadJsonStorage::deleteThread(threadId);
}
