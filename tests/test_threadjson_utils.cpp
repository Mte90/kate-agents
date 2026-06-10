#include <QtTest/QtTest>
#include "../src/threadjson.h"

class TestThreadJsonUtils : public QObject
{
    Q_OBJECT

private slots:

    void testGetThreadDir()
    {
        QString dir = ThreadJsonStorage::getThreadDir();
        QVERIFY(!dir.isEmpty());
        QVERIFY(dir.contains("kate/agents"));
    }

    void testGetProjectPrefixEmpty()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("");
        QVERIFY(prefix.isEmpty());
    }

    void testGetProjectPrefixSimple()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("myproject");
        QVERIFY(prefix == "myproject_");
    }

    void testGetProjectPrefixWithSpaces()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("my project");
        QVERIFY(prefix.contains("my_project"));
    }

    void testGetProjectPrefixWithSlash()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("my/project");
        QVERIFY(!prefix.contains("/"));
        QVERIFY(prefix.contains("_"));
    }

    void testGetProjectPrefixWithColon()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("my:project");
        QVERIFY(!prefix.contains(":"));
    }

    void testGetProjectPrefixWithQuestionMark()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("my?project");
        QVERIFY(!prefix.contains("?"));
    }

    void testGetProjectPrefixWithAsterisk()
    {
        QString prefix = ThreadJsonStorage::getProjectPrefix("my*project");
        QVERIFY(!prefix.contains("*"));
    }

    void testGetThreadFilePath()
    {
        QString path = ThreadJsonStorage::getThreadFilePath("testproject");
        QVERIFY(path.contains("testproject_threads.json"));
    }

    void testGetThreadFilePathEmpty()
    {
        QString path = ThreadJsonStorage::getThreadFilePath("");
        QVERIFY(path.contains("threads.json"));
    }

    void testGetRepoName()
    {
        QString name = ThreadJsonStorage::getRepoName("/path/to/myrepo");
        QVERIFY(name == "myrepo");
    }

    void testGetRepoNameWithDotGit()
    {
        QString name = ThreadJsonStorage::getRepoName("/path/to/myrepo.git");
        QVERIFY(name == "myrepo");
    }

    void testGetRepoNameRoot()
    {
        QString name = ThreadJsonStorage::getRepoName("/");
        QVERIFY(name.isEmpty());
    }
};

QTEST_MAIN(TestThreadJsonUtils)
#include "test_threadjson_utils.moc"