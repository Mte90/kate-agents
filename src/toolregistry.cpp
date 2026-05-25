#include "toolregistry.h"
#include "tools/terminaltool.h"

namespace {
int getDefaultTimeout(const QString &toolName) {
    if (toolName == "terminal") {
        return 10;
    }
    return 30;
}
} // namespace

struct ToolExecutionContext {
    QJsonObject result;
    bool completed = false;
    bool timedOut = false;
};

QJsonObject ToolRegistry::executeTool(const QString &name, const QJsonObject &args, int timeoutSeconds)
{
    if (!m_tools.contains(name)) {
        return QJsonObject{{"error", "Tool not found: " + name}, {"success", false}};
    }

    if (m_tools[name]->requiresPermission()) {
        // Check if permission is allowed
        if (m_permissionManager && !m_permissionManager->isAllowed(name)) {
            // Request permission - this will emit signal for UI to show dialog
            m_permissionManager->requestPermission(name);
            return QJsonObject{{"error", "Permission requested. Please confirm in dialog."}, {"success", false}};
        }
        // If no permission manager or allowed, proceed
    }

    if (timeoutSeconds <= 0) {
        timeoutSeconds = getDefaultTimeout(name);
    }

    AgentTool *tool = m_tools[name];
    ToolExecutionContext ctx;
    QTimer timer;
    QEventLoop loop;

    QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
        if (!ctx.completed) {
            ctx.timedOut = true;
        }
        loop.quit();
    });

    QThread *workerThread = new QThread();
    QObject worker;
    worker.moveToThread(workerThread);

    QObject::connect(workerThread, &QThread::started, [&]() {
        ctx.result = tool->execute(args);
        ctx.completed = true;
        workerThread->quit();
    });

    QObject::connect(workerThread, &QThread::finished, &loop, &QEventLoop::quit);

    workerThread->start();
    timer.start(timeoutSeconds * 1000);
    loop.exec();
    timer.stop();

    if (!ctx.completed || ctx.timedOut) {
        workerThread->quit();
        workerThread->wait();
        workerThread->deleteLater();
        return QJsonObject{
            {"error", QString("Tool execution timed out after %1 seconds").arg(timeoutSeconds)},
            {"success", false}
        };
    }

    workerThread->wait();
    workerThread->deleteLater();

    QJsonObject result = ctx.result;
    result.insert("success", true);
    return result;
}

ToolRegistry::ToolRegistry(QObject *parent)
    : QObject(parent)
{
}

ToolRegistry::~ToolRegistry() = default;

void ToolRegistry::registerTool(AgentTool *tool)
{
    if (tool && !tool->name().isEmpty()) {
        m_tools[tool->name()] = tool;
        emit toolRegistered(tool->name());
    }
}

void ToolRegistry::unregisterTool(const QString &name)
{
    if (m_tools.contains(name)) {
        m_tools.remove(name);
        emit toolUnregistered(name);
    }
}

std::vector<ToolDefinition> ToolRegistry::getToolDefinitions() const
{
    std::vector<ToolDefinition> definitions;
    for (auto *tool : m_tools.values()) {
        definitions.push_back(tool->toToolDefinition());
    }
    return definitions;
}

bool ToolRegistry::hasTool(const QString &name) const
{
    return m_tools.contains(name);
}

bool ToolRegistry::requiresPermission(const QString &name) const
{
    if (m_tools.contains(name)) {
        return m_tools[name]->requiresPermission();
    }
    return false;
}
