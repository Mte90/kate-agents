#include "checkpointmanager.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>
#include <QHash>
#include <algorithm>

QString CheckpointManager::getBackupPath(const QString &filePath, const QString &timestamp)
{
    return filePath + ".bak." + timestamp;
}

QString CheckpointManager::createBackup(const QString &filePath)
{
    if (!QFile::exists(filePath)) {
        return QString();
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
    QString backupPath = getBackupPath(filePath, timestamp);

    if (!QFile::copy(filePath, backupPath)) {
        qWarning() << "Failed to create backup:" << backupPath;
        return QString();
    }

    cleanupOldBackups(filePath);

    return backupPath;
}

bool CheckpointManager::restoreBackup(const QString &backupPath, const QString &originalPath)
{
    if (!QFile::exists(backupPath)) {
        qWarning() << "Backup file does not exist:" << backupPath;
        return false;
    }

    QFile backupFile(backupPath);
    QFile originalFile(originalPath);

    if (!backupFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open backup for reading:" << backupPath;
        return false;
    }

    if (!originalFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Cannot open original for writing:" << originalPath;
        backupFile.close();
        return false;
    }

    QByteArray content = backupFile.readAll();
    originalFile.write(content);

    backupFile.close();
    originalFile.close();

    return true;
}

QStringList CheckpointManager::listBackups(const QString &filePath)
{
    QStringList backups;
    QFileInfo fileInfo(filePath);
    QString dirPath = fileInfo.absolutePath();
    QString baseName = fileInfo.fileName();

    QDir dir(dirPath);
    QStringList filters;
    filters << baseName + ".bak.*";

    QFileInfoList entries = dir.entryInfoList(filters, QDir::Files, QDir::Name);

    for (const QFileInfo &entry : entries) {
        backups.append(entry.absoluteFilePath());
    }

    // Sort newest first (timestamp format ensures alphabetical = chronological)
    std::reverse(backups.begin(), backups.end());

    return backups;
}

int CheckpointManager::cleanupOldBackups(const QString &filePath, int maxBackups)
{
    QStringList backups = listBackups(filePath);
    int removed = 0;

    while (backups.size() > maxBackups) {
        QString oldest = backups.takeLast();
        if (QFile::remove(oldest)) {
            removed++;
        } else {
            qWarning() << "Failed to remove old backup:" << oldest;
        }
    }

    return removed;
}

qint64 CheckpointManager::totalBackupSize(const QString &filePath)
{
    QStringList backups = listBackups(filePath);
    qint64 totalSize = 0;

    for (const QString &backup : backups) {
        QFileInfo info(backup);
        totalSize += info.size();
    }

    return totalSize;
}

void CheckpointManager::cleanupAllBackups(const QString &directory, int maxBackups)
{
    QString dirPath = directory.isEmpty() ? QDir::currentPath() : directory;
    QDir dir(dirPath);

    QStringList filters;
    filters << "*.bak.*";
    QFileInfoList entries = dir.entryInfoList(filters, QDir::Files, QDir::Name);

    // Group backups by base file
    QHash<QString, QStringList> backupsByFile;
    QRegularExpression bakPattern("^([^/]*).bak\\.(.*)$");

    for (const QFileInfo &entry : entries) {
        QRegularExpressionMatch match = bakPattern.match(entry.fileName());
        if (match.hasMatch()) {
            QString baseFile = match.captured(1);
            backupsByFile[baseFile].append(entry.absoluteFilePath());
        }
    }

    // Cleanup each group
    for (auto it = backupsByFile.begin(); it != backupsByFile.end(); ++it) {
        QString baseFile = it.key();
        QStringList backups = it.value();

        // Sort newest first
        std::reverse(backups.begin(), backups.end());

        while (backups.size() > maxBackups) {
            QString oldest = backups.takeLast();
            if (!QFile::remove(oldest)) {
                qWarning() << "Failed to remove old backup:" << oldest;
            }
        }
    }
}
