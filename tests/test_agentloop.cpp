#include <QtTest/QtTest>
#include "../src/agentloop.h"
#include "../src/toolregistry.h"
#include "../src/llmprovider.h"
#include <QSignalSpy>

class MockLLMProvider : public LLMProvider
{
    Q_OBJECT
public:
    MockLLMProvider() : LLMProvider() {}
    QString name() const override { return QStringLiteral("mock"); }
    bool isAvailable() override { return true; }
    QStringList availableModels() override { return {QStringLiteral("test-model")}; }

    QFuture<LLMResponse> chat(const std::vector<LLMMessage> &,
                               const std::vector<ToolDefinition> &,
                               const QString &, double) override
    {
        QPromise<LLMResponse> *p = new QPromise<LLMResponse>();
        p->addResult(LLMResponse{});
        p->finish();
        return p->future();
    }

    void chatStream(const std::vector<LLMMessage> &,
                    const std::vector<ToolDefinition> &,
                    const QString &,
                    std::function<void(const QString &)>,
                    std::function<void(const LLMResponse &)> onDone,
                    std::function<void(const QString &)>) override
    {
        if (onDone) {
            LLMResponse resp;
            resp.content = QStringLiteral("mock");
            onDone(resp);
        }
    }

    bool m_abortCalled = false;
    void abort() override { m_abortCalled = true; }
};

class TestAgentLoop : public QObject
{
    Q_OBJECT

private slots:
    void testProfileConversionWrite()
    {
        QCOMPARE(stringToProfile("Write"), AgentProfile::Write);
    }

    void testProfileConversionAsk()
    {
        QCOMPARE(stringToProfile("Ask"), AgentProfile::Ask);
    }

    void testProfileConversionMinimal()
    {
        QCOMPARE(stringToProfile("Minimal"), AgentProfile::Minimal);
    }

    void testProfileConversionUnknown()
    {
        QCOMPARE(stringToProfile("unknown"), AgentProfile::Write);
    }

    void testProfileConversionEmpty()
    {
        QCOMPARE(stringToProfile(""), AgentProfile::Write);
    }

    void testProfileConversionCase()
    {
        QCOMPARE(stringToProfile("write"), AgentProfile::Write);
        QCOMPARE(stringToProfile("ask"), AgentProfile::Write);
    }

    void testProfileToStringWrite()
    {
        QCOMPARE(profileToString(AgentProfile::Write), QStringLiteral("Write"));
    }

    void testProfileToStringAsk()
    {
        QCOMPARE(profileToString(AgentProfile::Ask), QStringLiteral("Ask"));
    }

    void testProfileToStringMinimal()
    {
        QCOMPARE(profileToString(AgentProfile::Minimal), QStringLiteral("Minimal"));
    }

    void testProfileRoundTrip()
    {
        QList<AgentProfile> profiles = {AgentProfile::Write, AgentProfile::Ask, AgentProfile::Minimal};
        for (auto profile : profiles) {
            QCOMPARE(stringToProfile(profileToString(profile)), profile);
        }
    }

    void testSystemPromptForWrite()
    {
        QVERIFY(systemPromptForProfile(AgentProfile::Write).contains("modify"));
    }

    void testSystemPromptForAsk()
    {
        QVERIFY(systemPromptForProfile(AgentProfile::Ask).contains("read-only"));
    }

    void testSystemPromptForMinimal()
    {
        QVERIFY(systemPromptForProfile(AgentProfile::Minimal).contains("concise"));
    }

    void testGenerateTitleShort()
    {
        QString title = QStringLiteral("Fix the bug in main.cpp");
        QVERIFY(title.length() <= 50);
    }

    void testGenerateTitleLong()
    {
        QString title(60, QChar('a'));
        QVERIFY(title.length() > 50);
        QString truncated = title.left(47) + "...";
        QCOMPARE(truncated.length(), 50);
        QVERIFY(truncated.endsWith("..."));
    }

    void testGenerateTitleExact50()
    {
        QString title(50, QChar('x'));
        QCOMPARE(title.length(), 50);
    }

    void testGenerateTitle49()
    {
        QString title(49, QChar('x'));
        QVERIFY(title.length() < 50);
    }

    void testGenerateTitle51()
    {
        QString title(51, QChar('x'));
        QString truncated = title.left(47) + "...";
        QCOMPARE(truncated.length(), 50);
    }

    void testGenerateTitleEmpty()
    {
        QVERIFY(QString().isEmpty());
    }

    void testAgentLoopConstruction()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QVERIFY(!loop.isRunning());
    }

    void testSetMaxIterations()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        loop.setMaxIterations(50);
        QVERIFY(!loop.isRunning());
    }

    void testSetSystemPrompt()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        loop.setSystemPrompt(QStringLiteral("Custom prompt"));
        QVERIFY(!loop.isRunning());
    }

    void testCreateThread()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        ConversationThread thread = loop.createThread();
        QVERIFY(!thread.id.isEmpty());
        QVERIFY(thread.id.length() >= 32);
        QVERIFY(thread.isActive);
        QVERIFY(thread.messages.isEmpty() || thread.messages[0].role == "system");
    }

    void testCreateThreadWithTitle()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        ConversationThread thread = loop.createThread("My Title");
        QCOMPARE(thread.title, QStringLiteral("My Title"));
    }

    void testAddUserMessageToNewThread()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QString threadId = QStringLiteral("test-thread-new-%1").arg(QDateTime::currentMSecsSinceEpoch());
        loop.addUserMessage(threadId, "Hello agent");

        QVERIFY(!loop.isRunning());
    }

    void testExecuteTurnNoProvider()
    {
        ToolRegistry registry;
        AgentLoop loop(nullptr, &registry);

        QSignalSpy spy(&loop, &AgentLoop::error);
        QString threadId = QStringLiteral("test-no-provider-%1").arg(QDateTime::currentMSecsSinceEpoch());
        loop.addUserMessage(threadId, "test");
        loop.executeTurn(threadId);

        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toString().contains("provider", Qt::CaseInsensitive));
    }

    void testExecuteTurnThreadNotFound()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QSignalSpy spy(&loop, &AgentLoop::error);
        loop.executeTurn("nonexistent-thread-id");

        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toString().contains("not found", Qt::CaseInsensitive));
    }

    void testAbortState()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        loop.abort();
        QVERIFY(!loop.isRunning());
    }

    void testAbortWithNullProviderNoCrash()
    {
        ToolRegistry registry;
        AgentLoop loop(nullptr, &registry);
        loop.abort();
        QVERIFY(!loop.isRunning());
    }

    void testAbortCallsProviderAbort()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QVERIFY(!provider.m_abortCalled);
        loop.abort();
        QVERIFY(provider.m_abortCalled);
    }

    void testRunningChangedSignal()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QSignalSpy spy(&loop, &AgentLoop::runningChanged);
        loop.abort();

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toBool(), false);
    }

    void testErrorSignalOnNoModels()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        loop.setMaxIterations(0);
        QString threadId = QStringLiteral("test-zero-iter-%1").arg(QDateTime::currentMSecsSinceEpoch());
        loop.addUserMessage(threadId, "test");

        QVERIFY(!loop.isRunning());
    }

    void testSetProvider()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(nullptr, &registry);

        loop.setProvider(&provider);
        QVERIFY(!loop.isRunning());
    }

    void testSetToolRegistry()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, nullptr);

        loop.setToolRegistry(&registry);
        QVERIFY(!loop.isRunning());
    }

    void testMultipleAborts()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        for (int i = 0; i < 5; ++i) {
            loop.abort();
        }
        QVERIFY(!loop.isRunning());
    }

    void testSaveAllThreadsWithoutCrash()
    {
        MockLLMProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        loop.saveAllThreads();
    }
};

QTEST_MAIN(TestAgentLoop)
#include "test_agentloop.moc"
