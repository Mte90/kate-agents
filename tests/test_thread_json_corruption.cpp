#include <QtTest/QtTest>
#include "../src/threadjson.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>

class TestThreadJsonCorruption : public QObject
{
    Q_OBJECT

private:
    QString getUniqueProjectId() {
        return "corrupt-proj-" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "-" + QString::number(qrand());
    }

    QString getStoragePath(const QString &projectId) {
        // Based on ARCHITECTURE_DECISIONS: {project}_threads.json
        // We need the actual storage directory.
        // Since ThreadJsonStorage is a static utility, we simulate the file write.
        // Typically these are stored in ~/.local/share/kate-agents/ or similar.
        // For the sake of the test, we'll try to find the file created by ThreadJsonStorage.
        return QString("%1/%2_threads.json").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), projectId);
    }

private slots:
    void testMalformedJsonHandling()
    {
        QString projectId = getUniqueProjectId();
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        // Manually create a corrupted JSON file
        QString path = getStoragePath(projectId);
        QDir().mkpath(QFileInfo(path).absolutePath());
        
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("{ \"threads\": [ { \"id\": \"corrupt\", \"messages\": [ { \"role\": \"user\", \"content\": \"broken" ); // Incomplete JSON
            file.close();
        }
        
        // This should not crash and should return an empty list or handle gracefully
        auto threads = ThreadJsonStorage::loadAllThreads();
        QVERIFY(threads.isEmpty());
    }

    void testEmptyFileHandling()
    {
        QString projectId = getUniqueProjectId();
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        QString path = getStoragePath(projectId);
        QDir().mkpath(QFileInfo(path).absolutePath());
        
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(""); // Totally empty file
            file.close();
        }
        
        auto threads = ThreadJsonStorage::loadAllThreads();
        QVERIFY(threads.isEmpty());
    }

    void testWrongJsonStructure()
    {
        QString projectId = getUniqueProjectId();
        ThreadJsonStorage::setCurrentProjectId(projectId);
        
        QString path = getStoragePath(projectId);
        QDir().mkpath(QFileInfo(path).absolutePath());
        
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("{\"not_threads\": \"wrong_key\"}"); 
            file.close();
        }
        
        auto threads = ThreadJsonStorage::loadAllThreads();
        QVERIFY(threads.isEmpty());
    }
};

QTEST_MAIN(TestThreadJsonCorruption)
#include "test_thread_json_corruption.moc"
