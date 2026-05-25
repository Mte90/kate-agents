#include <QtTest/QtTest>
#include "../src/configmanager.h"
#include "../src/toolregistry.h"
#include "../src/permissionmanager.h"
#include "../src/agentloop.h"
#include "../src/llmprovider.h"
#include "../src/threadjson.h"
#include <QSignalSpy>
#include <QFile>
#include <QDir>

class MockIntegrationProvider : public LLMProvider
{
    Q_OBJECT
public:
    MockIntegrationProvider() : LLMProvider() {}
    QString name() const override { return QStringLiteral("mock-integration"); }
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
            resp.content = QStringLiteral("mock integration response");
            onDone(resp);
        }
    }

    bool m_abortCalled = false;
    void abort() override { m_abortCalled = true; }
};

class MockTool : public AgentTool
{
    Q_OBJECT
public:
    explicit MockTool(const QString &toolName, bool requiresPerm = false)
        : m_name(toolName), m_requiresPermission(requiresPerm) {}

    QString name() const override { return m_name; }
    QString description() const override { return QStringLiteral("Mock tool for testing"); }
    QJsonObject parametersSchema() const override
    {
        return QJsonObject{{"type", "object"}, {"properties", QJsonObject{}}};
    }
    QJsonObject execute(const QJsonObject &args) override
    {
        m_lastArgs = args;
        m_callCount++;
        return QJsonObject{{"result", QStringLiteral("executed")}, {"calls", m_callCount}};
    }
    bool requiresPermission() const override { return m_requiresPermission; }

    int callCount() const { return m_callCount; }
    QJsonObject lastArgs() const { return m_lastArgs; }

private:
    QString m_name;
    bool m_requiresPermission;
    int m_callCount = 0;
    QJsonObject m_lastArgs;
};

class TestIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        ThreadJsonStorage::setCurrentProjectId(
            QStringLiteral("test-integration-%1").arg(QDateTime::currentMSecsSinceEpoch()));
    }

    void testToolRegistryWithPermissionManager()
    {
        ToolRegistry registry;
        PermissionManager pm;
        registry.setPermissionManager(&pm);

        auto *tool = new MockTool("restricted_tool", true);
        registry.registerTool(tool);

        pm.setDefaultPolicy(PermissionPolicy::Deny);

        QJsonObject args;
        args["input"] = "test";
        QJsonObject result = registry.executeTool("restricted_tool", args);

        QVERIFY(result.contains("error"));
        QCOMPARE(result["success"].toBool(), false);
    }

    void testToolRegistryAllowedWithPermission()
    {
        ToolRegistry registry;
        PermissionManager pm;
        registry.setPermissionManager(&pm);

        auto *tool = new MockTool("allowed_tool", false);
        registry.registerTool(tool);

        pm.setDefaultPolicy(PermissionPolicy::Allow);

        QJsonObject args;
        args["input"] = "test";
        QJsonObject result = registry.executeTool("allowed_tool", args);

        QVERIFY(result.contains("success"));
    }

    void testToolRegistryWithoutPermissionManager()
    {
        ToolRegistry registry;

        auto *tool = new MockTool("free_tool", true);
        registry.registerTool(tool);

        QJsonObject args;
        args["input"] = "test";
        QJsonObject result = registry.executeTool("free_tool", args);

        QVERIFY(result.contains("success"));
    }

    void testToolRegistryPermissionChange()
    {
        ToolRegistry registry;
        PermissionManager pm;
        registry.setPermissionManager(&pm);

        auto *tool = new MockTool("flip_tool", true);
        registry.registerTool(tool);

        pm.setDefaultPolicy(PermissionPolicy::Allow);
        QJsonObject result1 = registry.executeTool("flip_tool", {});
        QVERIFY(result1.contains("success"));

        pm.setDefaultPolicy(PermissionPolicy::Deny);
        pm.clearSessionPermissions();
        QJsonObject result2 = registry.executeTool("flip_tool", {});
        QVERIFY(result2.contains("error"));
    }

    void testAgentLoopWithToolRegistry()
    {
        MockIntegrationProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        auto *tool = new MockTool("read_file");
        registry.registerTool(tool);

        QVERIFY(registry.hasTool("read_file"));
        QCOMPARE(static_cast<int>(registry.getToolDefinitions().size()), 1);
    }

    void testAgentLoopExecuteTurnErrorPropagation()
    {
        MockIntegrationProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QSignalSpy errorSpy(&loop, &AgentLoop::error);
        loop.executeTurn("nonexistent");

        QCOMPARE(errorSpy.count(), 1);
    }

    void testAgentLoopAbortCascadesToProvider()
    {
        MockIntegrationProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QVERIFY(!provider.m_abortCalled);
        loop.abort();
        QVERIFY(provider.m_abortCalled);
    }

    void testAgentLoopCreateThreadAndAddMessage()
    {
        MockIntegrationProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);
        loop.setSystemPrompt("Test prompt");

        ConversationThread thread = loop.createThread("Integration Test");
        QVERIFY(!thread.id.isEmpty());
        QCOMPARE(thread.title, QStringLiteral("Integration Test"));

        loop.addUserMessage(thread.id, "Hello from integration test");
        loop.addUserMessage(thread.id, "Second message");

        QVERIFY(!loop.isRunning());
    }

    void testConfigManagerWithAgentLoop()
    {
        ConfigManager config;
        MockIntegrationProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        loop.setSystemPrompt(config.getSystemPrompt());
        loop.setMaxIterations(config.getMaxIterations());

        ConversationThread thread = loop.createThread();
        QVERIFY(!thread.id.isEmpty());
    }

    void testToolRegistrationAndDefinitions()
    {
        ToolRegistry registry;
        auto *tool1 = new MockTool("tool_a");
        auto *tool2 = new MockTool("tool_b");
        auto *tool3 = new MockTool("tool_c");

        registry.registerTool(tool1);
        registry.registerTool(tool2);
        registry.registerTool(tool3);

        QCOMPARE(static_cast<int>(registry.getToolDefinitions().size()), 3);
        QVERIFY(registry.hasTool("tool_a"));
        QVERIFY(registry.hasTool("tool_b"));
        QVERIFY(registry.hasTool("tool_c"));
        QVERIFY(!registry.hasTool("tool_d"));
    }

    void testToolRegistryUnregister()
    {
        ToolRegistry registry;

        QSignalSpy spy(&registry, &ToolRegistry::toolUnregistered);
        auto *tool = new MockTool("temp_tool");
        registry.registerTool(tool);

        QVERIFY(registry.hasTool("temp_tool"));
        registry.unregisterTool("temp_tool");
        QVERIFY(!registry.hasTool("temp_tool"));
        QCOMPARE(spy.count(), 1);
    }

    void testToolRegistryRegisterSignal()
    {
        ToolRegistry registry;
        QSignalSpy spy(&registry, &ToolRegistry::toolRegistered);

        auto *tool = new MockTool("sig_tool");
        registry.registerTool(tool);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("sig_tool"));
    }

    void testToolExecutionWithArgs()
    {
        ToolRegistry registry;
        auto *tool = new MockTool("arg_tool");
        registry.registerTool(tool);

        QJsonObject args;
        args["path"] = "/tmp/test.txt";
        args["recursive"] = true;
        QJsonObject result = registry.executeTool("arg_tool", args);

        QVERIFY(result["success"].toBool());
    }

    void testToolExecutionNonExistent()
    {
        ToolRegistry registry;
        QJsonObject result = registry.executeTool("ghost_tool", {});

        QCOMPARE(result["success"].toBool(), false);
        QVERIFY(result["error"].toString().contains("not found"));
    }

    void testPermissionManagerSessionAcrossTools()
    {
        PermissionManager pm;
        pm.setDefaultPolicy(PermissionPolicy::Confirm);

        pm.grantPermission("read_file");
        pm.grantPermission("grep");
        QVERIFY(pm.isAllowed("read_file"));
        QVERIFY(pm.isAllowed("grep"));
        QVERIFY(!pm.isAllowed("edit_file"));

        pm.clearSessionPermissions();
        QVERIFY(!pm.isAllowed("read_file"));
        QVERIFY(!pm.isAllowed("grep"));
    }

    void testConfigManagerProvidersPreserved()
    {
        ConfigManager mgr;
        ProviderConfig cfg;
        cfg.name = "custom";
        cfg.type = "openai-compatible";
        cfg.baseUrl = "http://localhost:8080";
        cfg.enabled = true;
        mgr.setProviderConfig("custom", cfg);

        QCOMPARE(static_cast<int>(mgr.getProviders().size()), 2);

        ProviderConfig retrieved = mgr.getProviderConfig("custom");
        QCOMPARE(retrieved.name, QStringLiteral("custom"));
        QCOMPARE(retrieved.baseUrl, QStringLiteral("http://localhost:8080"));
    }

    void testAgentLoopThreadUpdatedSignal()
    {
        MockIntegrationProvider provider;
        ToolRegistry registry;
        AgentLoop loop(&provider, &registry);

        QSignalSpy spy(&loop, &AgentLoop::threadUpdated);

        QString threadId = QStringLiteral("sig-test-%1").arg(QDateTime::currentMSecsSinceEpoch());
        loop.addUserMessage(threadId, "trigger signal");

        QCOMPARE(spy.count(), 1);
    }

    void testMultipleToolRegistrationsSameName()
    {
        ToolRegistry registry;
        auto *tool1 = new MockTool("same_name");
        auto *tool2 = new MockTool("same_name");

        registry.registerTool(tool1);
        registry.registerTool(tool2);

        QCOMPARE(static_cast<int>(registry.getToolDefinitions().size()), 1);
        QVERIFY(registry.hasTool("same_name"));
    }

    void testPermissionManagerRequiresPermissionViaRegistry()
    {
        ToolRegistry registry;
        auto *tool = new MockTool("perm_tool", true);
        registry.registerTool(tool);

        QVERIFY(registry.requiresPermission("perm_tool"));
        QVERIFY(!registry.requiresPermission("no_perm_tool"));
        QVERIFY(!registry.requiresPermission("nonexistent"));
    }

    void testAgentLoopSetProviderAndRegistry()
    {
        MockIntegrationProvider provider;
        ToolRegistry registry;
        AgentLoop loop(nullptr, nullptr);

        loop.setProvider(&provider);
        loop.setToolRegistry(&registry);

        QVERIFY(!loop.isRunning());
    }

    void testConfigManagerSaveLoadRoundTrip()
    {
        ConfigManager mgr1;
        mgr1.setActiveProvider("test-int");
        mgr1.setActiveModel("model-int");
        mgr1.setMaxIterations(15);
        mgr1.setTemperature(0.5);
        mgr1.save();

        ConfigManager mgr2;
        mgr2.load();
        QCOMPARE(mgr2.getActiveProvider(), QStringLiteral("test-int"));
        QCOMPARE(mgr2.getActiveModel(), QStringLiteral("model-int"));
        QCOMPARE(mgr2.getMaxIterations(), 15);

        mgr1.setActiveProvider("regolo");
        mgr1.setActiveModel("qwen3-coder-next");
        mgr1.setMaxIterations(20);
        mgr1.setTemperature(0.7);
        mgr1.save();
    }

    void testToolRegistryNullTool()
    {
        ToolRegistry registry;
        registry.registerTool(nullptr);
        QCOMPARE(static_cast<int>(registry.getToolDefinitions().size()), 0);
    }

    void testToolRegistryEmptyNameTool()
    {
        ToolRegistry registry;
        auto *tool = new MockTool("");
        registry.registerTool(tool);
        QCOMPARE(static_cast<int>(registry.getToolDefinitions().size()), 0);
    }
};

QTEST_MAIN(TestIntegration)
#include "test_integration.moc"
