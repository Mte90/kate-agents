#include "threadstorage.h"
#include "threadjson.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

ThreadStorage::ThreadStorage(QObject *parent)
    : QObject(parent)
{
}

ThreadStorage::~ThreadStorage()
{
}

QString ThreadStorage::databasePath() const
{
    Q_UNUSED(this);
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
    (void)path;
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
