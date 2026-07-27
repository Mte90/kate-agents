#include <QTest>
#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QtConcurrent>
#include <QFutureWatcher>

#include "../src/llmprovider.h"

struct MockProvider : public LLMProvider
{
    using LLMProvider::LLMProvider;
    bool isAvailable() override { return true; }
    QString name() const override { return "Mock"; }
    QStringList availableModels() override { return {"model1", "model2"}; }
    QFuture<LLMResponse> chat(const std::vector<LLMMessage> &, const std::vector<ToolDefinition> &, const QString &, double) override {
        LLMResponse response;
        return QtConcurrent::run([response]() {
            return response;
        });
    }
    void chatStream(const std::vector<LLMMessage> &, const std::vector<ToolDefinition> &, const QString &,
                   std::function<void(const QString &chunk)>,
                   std::function<void(const LLMResponse &final)>,
                   std::function<void(const QString &error)>) override {
    }
    void abort() override {}
};

void testLLMProviderAbstract()
{
    LLMProvider *provider = new MockProvider(nullptr);
    
    QCOMPARE(provider->name(), "Mock");
    QCOMPARE(provider->isAvailable(), true);
}

void testLLMProviderMessage()
{
    LLMMessage msg1;
    msg1.role = "system";
    msg1.content = "You are helpful";
    
    QCOMPARE(msg1.role, "system");
    QCOMPARE(msg1.content, "You are helpful");
}

void testLLMResponse()
{
    LLMResponse response;
    response.content = "Test response";
    
    QCOMPARE(response.content, "Test response");
}

void testLLMMessageList()
{
    std::vector<LLMMessage> messages;
    
    LLMMessage sys;
    sys.role = "system";
    sys.content = "System prompt";
    messages.push_back(sys);
    
    LLMMessage user;
    user.role = "user";
    user.content = "Hello";
    messages.push_back(user);
    
    QCOMPARE(messages.size(), 2);
    QCOMPARE(messages[0].role, "system");
    QCOMPARE(messages[1].role, "user");
}

void testLLMMessageDefaultConstructor()
{
    LLMMessage msg;
    
    QCOMPARE(msg.role, "");
    QCOMPARE(msg.content, "");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug() << "=== LLM Provider Tests ===";
    
    testLLMProviderAbstract();
    testLLMProviderMessage();
    testLLMResponse();
    testLLMMessageList();
    testLLMMessageDefaultConstructor();
    
    qDebug() << "=== All LLM Provider Tests Complete ===";
    return 0;
}
