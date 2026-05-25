/*
 * Test for thread persistence
 */

#include <QTest>
#include <QDebug>

#include "threadjson.h"
#include "threadjsonstorage.h"
#include "llmprovider.h"

void testThreadJsonSave()
{
    ThreadJson thread("test-thread");
    thread.setId("123");
    thread.setTitle("Test Conversation");
    thread.setCreatedAt(QDateTime::currentDateTime());
    
    thread.messages.append({"role", "user", "content", "test message 1"});
    thread.messages.append({"role", "assistant", "content", "test response 1"});
    
    QJsonObject json = thread.toSettings();
    QCOMPARE(json["id"].toString(), "123");
    QCOMPARE(json["title"].toString(), "Test Conversation");
    QCOMPARE(json["messages"].count(), 2);
    QCOMPARE(json["messages"].at(0).toObject()["role"], "user");
    QCOMPARE(json["messages"].at(1).toObject()["role"], "assistant");
}

void testThreadJsonLoad()
{
    QJsonObject json;
    json["id"] = "456";
    json["title"] = "Loaded Conversation";
    json["createdAt"] = QDate(2025, 1, 15).toString(Qt::ISODate);
    
    QJsonArray messagesArray;
    messagesArray.append({"role", "system", "content", "system prompt"});
    messagesArray.append({"role", "user", "content", "user message"});
    json["messages"] = messagesArray;
    
    ThreadJson thread(json);
    QCOMPARE(thread.id(), "456");
    QCOMPARE(thread.title(), "Loaded Conversation");
    QCOMPARE(thread.messages.count(), 2);
    QCOMPARE(thread.messages[0].role, "system");
    QCOMPARE(thread.messages[1].role, "user");
}

void testThreadJsonEmpty()
{
    QJsonObject json;
    ThreadJson thread(json);
    QCOMPARE(thread.id(), "");
    QCOMPARE(thread.title(), "");
    QCOMPARE(thread.messages.count(), 0);
}

void testThreadJsonDelete()
{
    ThreadJsonStorage storage(nullptr);
    ThreadJson thread;
    thread.setId("123");
    thread.setTitle("To Delete");
    storage.addThread(thread);
    QCOMPARE(storage.allThreads().count(), 1);
    QCOMPARE(storage.allThreads().at("123")->title(), "To Delete");
    storage.deleteThread("123");
    QCOMPARE(storage.allThreads().count(), 0);
}

void testThreadJsonPersistence()
{
    QString jsonString = R"({
        "id": "789",
        "title": "Persistent Thread",
        "messages": [
            {"role": "system", "content": "You are helpful."},
            {"role": "user", "content": "Hello"},
            {"role": "assistant", "content": "Hi there!"}
        ]
    })";
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    ThreadJson thread(doc.object());
    QCOMPARE(thread.id(), "789");
    QCOMPARE(thread.title(), "Persistent Thread");
    QCOMPARE(thread.messages.count(), 3);
}

void testThreadJsonWithProfile()
{
    QJsonObject json;
    json["id"] = "999";
    json["title"] = "Profile Test";
    
    QJsonArray messages;
    messages.append({"role", "user", "content", "First message"});
    messages.append({"role", "assistant", "content", "Response 1"});
    messages.append({"role", "user", "content", "Second message with @grep"});
    messages.append({"role", "assistant", "content", "Tool result"});
    json["messages"] = messages;
    
    ThreadJson thread(json);
    QCOMPARE(thread.messages.count(), 4);
    QCOMPARE(thread.messages[2].content, "Second message with @grep");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "=== Thread Persistence Tests ===";
    
    testThreadJsonSave();
    testThreadJsonLoad();
    testThreadJsonEmpty();
    testThreadJsonDelete();
    testThreadJsonPersistence();
    testThreadJsonWithProfile();
    
    qDebug() << "\n=== All Thread Persistence Tests Complete ===";
    
    return 0;
}
