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
        QString errorMsg = i18n("Tool not found: %1", name);
        if (m_auditLogger) {
            m_auditLogger->logError("tool_execution", errorMsg);
        }
        return QJsonObject{{"error", errorMsg}, {"success", false}};
    }

    if (m_tools[name]->requiresPermission()) {
        // Check if permission is allowed
        if (m_permissionManager && !m_permissionManager->isAllowed(name)) {
            // Request permission - this will emit signal for UI to show dialog
            m_permissionManager->requestPermission(name);
            
            // Re-check if permission was granted after user interaction
            if (!m_permissionManager->isAllowed(name)) {
                // Permission denied by user
                if (m_auditLogger) {
                    m_auditLogger->logPermissionDecision(name, false);
                }
                return QJsonObject{
                    {"error", i18n("Permission denied for tool '%1'.", name)},
                    {"success", false},
                    {"requires_permission", true}
                };
            }
            // Permission granted - log and proceed
            if (m_auditLogger) {
                m_auditLogger->logPermissionDecision(name, true);
            }
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
        
        QString errorMsg = i18n("Tool '%1' timed out after %2 seconds. The operation may have been blocked or is taking longer than expected.", name, timeoutSeconds);
        if (m_auditLogger) {
            m_auditLogger->logToolExecution(name, args, false, errorMsg);
        }
        
        return QJsonObject{
            {"error", errorMsg},
            {"success", false},
            {"timed_out", true},
            {"tool", name},
            {"timeout_seconds", timeoutSeconds},
            {"hint", i18n("The tool is still running in the background. Consider retrying with a longer timeout or simplifying the command.")}
        };
    }

    workerThread->wait();
    workerThread->deleteLater();

    QJsonObject result = ctx.result;
    
    // Log to audit trail
    if (m_auditLogger) {
        bool success = result["success"].toBool();
        QString error = result.contains("error") ? result["error"].toString() : QString();
        m_auditLogger->logToolExecution(name, args, success, error);
    }
    
    // Add helpful hints based on error type
    if (result.contains("error") && !result["success"].toBool()) {
        QString errorMsg = result["error"].toString();
        
        // Add context-specific hints
        if (errorMsg.contains("Permission")) {
            result["hint"] = i18n("Grant permission in the dialog that appeared, then retry the tool.");
        } else if (errorMsg.contains("timed out")) {
            result["hint"] = i18n("The tool is still running in the background. Consider retrying with a longer timeout or simplifying the command.");
        } else if (errorMsg.contains("not found")) {
            result["hint"] = i18n("Check that the file or command exists and that you have the correct path.");
        } else if (errorMsg.contains("blocked")) {
            result["hint"] = i18n("This command is blocked for security reasons. Try a safer alternative or request permission for specific commands.");
        }
    }
    
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
