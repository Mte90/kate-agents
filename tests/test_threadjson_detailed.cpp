#include <QtTest/QtTest>
#include "../src/threadjson.h"

class TestThreadJsonDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testGetThreadDirNotEmpty()
    {
        QString dir = ThreadJsonStorage::getThreadDir();
        QVERIFY(!dir.isEmpty());
        QVERIFY(dir.contains("kate"));
    }

    void testGetProjectPrefixSimple()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("myproject");
        QVERIFY(prefix == "myproject_");
    }

    void testGetProjectPrefixWithSpace()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("my project");
        QVERIFY(prefix.contains("_"));
    }

    void testGetProjectPrefixWithSlash()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("path/to/project");
        QVERIFY(!prefix.contains("/"));
    }

    void testGetProjectPrefixWithSpecialChars()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("proj:ect?");
        QVERIFY(!prefix.contains(":"));
        QVERIFY(!prefix.contains("?"));
    }

    void testGetProjectPrefixEmpty()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("");
        QVERIFY(prefix.isEmpty());
    }

    void testGetThreadFilePath()
    {
        QString path = ThreadJsonStorage::getThreadFilePath("test");
        QVERIFY(path.contains("test_threads.json"));
    }

    void testGetThreadFilePathEmpty()
    {
        QString path = ThreadJsonStorage::getThreadFilePath("");
        QVERIFY(path.contains("threads.json"));
    }

    void testGetRepoNameSimple()
    {
        QString name = ThreadJsonStorage::getRepoName("/path/to/myrepo");
        QVERIFY(name == "myrepo");
    }

    void testGetRepoNameWithDotGit()
    {
        QString name = ThreadJsonStorage::getRepoName("/path/to/project.git");
        QVERIFY(name == "project");
    }

    void testGetRepoNameRoot()
    {
        QString name = ThreadJsonStorage::getRepoName("/");
        QVERIFY(name.isEmpty());
    }

    void testSetCurrentProjectId()
    {
        ThreadJsonStorage::setCurrentProjectId("custom-project");
        QVERIFY(ThreadJsonStorage::s_currentProjectId == "custom-project");
    }
};

QTEST_MAIN(TestThreadJsonDetailed)
#include "test_threadjson_detailed.moc"