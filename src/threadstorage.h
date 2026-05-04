#ifndef THREADSTORAGE_H
#define THREADSTORAGE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <vector>

#include "llmprovider.h"

class ThreadStorage : public QObject
{
    Q_OBJECT

public:
    explicit ThreadStorage(QObject *parent = nullptr);
    ~ThreadStorage() override;

    bool initialize();
    QMap<QString, ConversationThread> loadAllThreads();
    bool saveThread(const ConversationThread &thread);
    bool saveAllThreads(const QMap<QString, ConversationThread> &threads);
    bool deleteThread(const QString &threadId);

private:
    QString databasePath() const;
};

#endif // THREADSTORAGE_H