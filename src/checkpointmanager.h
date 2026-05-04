#ifndef CHECKPOINTMANAGER_H
#define CHECKPOINTMANAGER_H

#include <QString>
#include <QStringList>

class CheckpointManager {
public:
    static QString createBackup(const QString &filePath);
    static bool restoreBackup(const QString &backupPath, const QString &originalPath);
    static QStringList listBackups(const QString &filePath);
    static int cleanupOldBackups(const QString &filePath, int maxBackups = 5);
    static qint64 totalBackupSize(const QString &filePath);
    static void cleanupAllBackups(const QString &directory = QString(), int maxBackups = 5);

private:
    static QString getBackupPath(const QString &filePath, const QString &timestamp);
};

#endif
