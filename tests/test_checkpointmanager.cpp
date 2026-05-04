#include <QtTest/QtTest>
#include "../src/checkpointmanager.h"
#include <QFile>
#include <QDir>

class TestCheckpointManager : public QObject
{
    Q_OBJECT

private:
    QString m_testDir;
    QString m_testFilePath;

private slots:
    void initTestCase()
    {
        m_testDir = QDir::tempPath() + "/checkpoint_test_" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QDir().mkpath(m_testDir);
        m_testFilePath = m_testDir + "/testfile.txt";
        
        QFile file(m_testFilePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("Original content\nLine 2\nLine 3");
        file.close();
    }

    void cleanupTestCase()
    {
        QDir(m_testDir).removeRecursively();
    }

    void testBackupCreation()
    {
        QString backup = CheckpointManager::createBackup(m_testFilePath);
        QVERIFY(!backup.isEmpty());
        QVERIFY(QFile::exists(backup));
    }

    void testBackupContent()
    {
        QString backup = CheckpointManager::createBackup(m_testFilePath);
        QVERIFY(!backup.isEmpty());
        
        QFile backupFile(backup);
        QVERIFY(backupFile.open(QIODevice::ReadOnly));
        QCOMPARE(backupFile.readAll(), QByteArray("Original content\nLine 2\nLine 3"));
    }

    void testCleanupOldBackups()
    {
        // Create backups with delays to ensure unique timestamps
        QStringList createdBackups;
        for (int i = 0; i < 7; i++) {
            QString backup = CheckpointManager::createBackup(m_testFilePath);
            if (!backup.isEmpty()) {
                createdBackups.append(backup);
            }
            QTest::qSleep(1100);
        }
        
        // After creating 7 backups, cleanup should keep only 5
        int remaining = CheckpointManager::cleanupOldBackups(m_testFilePath, 5);
        QCOMPARE(remaining, 5);
    }

    void testInvalidFilePath()
    {
        QString invalidPath = "/nonexistent/file.txt";
        QString backup = CheckpointManager::createBackup(invalidPath);
        QVERIFY(backup.isEmpty());
    }

    void testMultipleFiles()
    {
        QString file1 = m_testDir + "/file1.txt";
        QString file2 = m_testDir + "/file2.txt";
        
        QFile f1(file1);
        QVERIFY(f1.open(QIODevice::WriteOnly));
        f1.write("File 1");
        f1.close();
        
        QFile f2(file2);
        QVERIFY(f2.open(QIODevice::WriteOnly));
        f2.write("File 2");
        f2.close();
        
        QString backup1 = CheckpointManager::createBackup(file1);
        QString backup2 = CheckpointManager::createBackup(file2);
        
        QVERIFY(!backup1.isEmpty());
        QVERIFY(!backup2.isEmpty());
    }

    void testBackupTimestampFormat()
    {
        QString backup = CheckpointManager::createBackup(m_testFilePath);
        QVERIFY(!backup.isEmpty());
        
        // Check timestamp format: YYYYMMDD-HHMMSS in filename
        QVERIFY(backup.contains(QRegularExpression("\\.bak\\.\\d{8}-\\d{6}")));
    }

    void testBackupPreservesOriginal()
    {
        QByteArray originalContent;
        {
            QFile file(m_testFilePath);
            QVERIFY(file.open(QIODevice::ReadOnly));
            originalContent = file.readAll();
        }
        
        CheckpointManager::createBackup(m_testFilePath);
        
        {
            QFile file(m_testFilePath);
            QVERIFY(file.open(QIODevice::ReadOnly));
            QCOMPARE(file.readAll(), originalContent);
        }
    }

    void testEmptyFile()
    {
        QString emptyFile = m_testDir + "/empty.txt";
        QFile file(emptyFile);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.close();
        
        QString backup = CheckpointManager::createBackup(emptyFile);
        QVERIFY(!backup.isEmpty());
    }
};

QTEST_MAIN(TestCheckpointManager)
#include "test_checkpointmanager.moc"
