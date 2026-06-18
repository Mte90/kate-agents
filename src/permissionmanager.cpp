#include "permissionmanager.h"
#include <QTimer>

PermissionManager::PermissionManager(QObject *parent)
    : QObject(parent)
    , m_waitLoop(nullptr)
    , m_permissionGranted(false)
{
}

PermissionManager::~PermissionManager() = default;

void PermissionManager::setDefaultPolicy(PermissionPolicy policy)
{
    m_defaultPolicy = policy;
}

void PermissionManager::setToolPolicy(const QString &toolName, PermissionPolicy policy)
{
    m_toolPolicies[toolName] = policy;
}

PermissionPolicy PermissionManager::getToolPolicy(const QString &toolName) const
{
    if (m_toolPolicies.contains(toolName)) {
        return m_toolPolicies[toolName];
    }
    return m_defaultPolicy;
}

bool PermissionManager::requiresConfirmation(const QString &toolName) const
{
    return getToolPolicy(toolName) == PermissionPolicy::Confirm;
}

bool PermissionManager::isAllowed(const QString &toolName) const
{
    QMutexLocker locker(&m_mutex);
    if (m_sessionPermissions.contains(toolName)) {
        return m_sessionPermissions[toolName];
    }
    PermissionPolicy policy = getToolPolicy(toolName);
    return policy == PermissionPolicy::Allow;
}

bool PermissionManager::requestPermission(const QString &toolName)
{
    if (!requiresConfirmation(toolName)) {
        return isAllowed(toolName);
    }
    
    // Check if already granted in this session
    {
        QMutexLocker locker(&m_mutex);
        if (m_sessionPermissions.contains(toolName) && m_sessionPermissions[toolName]) {
            return true;
        }
    }
    
    // Emit signal for UI to show dialog
    emit permissionRequested(toolName);
    
    // Block until user responds
    QEventLoop waitLoop;
    m_waitLoop = &waitLoop;
    m_pendingTool = toolName;
    m_permissionGranted = false;
    
    // Connect to slots that will be called when user responds
    auto onGranted = [this]() {
        m_permissionGranted = true;
        if (m_waitLoop) {
            m_waitLoop->quit();
        }
    };
    auto onDenied = [this]() {
        m_permissionGranted = false;
        if (m_waitLoop) {
            m_waitLoop->quit();
        }
    };
    
    // Use queued connections to ensure slots run in the right thread
    connect(this, &PermissionManager::permissionGranted, this, onGranted, Qt::QueuedConnection);
    connect(this, &PermissionManager::permissionDenied, this, onDenied, Qt::QueuedConnection);
    
    waitLoop.exec();
    
    // Cleanup
    m_waitLoop = nullptr;
    m_pendingTool.clear();
    
    // Update session permissions based on user choice
    {
        QMutexLocker locker(&m_mutex);
        m_sessionPermissions[toolName] = m_permissionGranted;
    }
    
    return m_permissionGranted;
}

void PermissionManager::grantPermission(const QString &toolName)
{
    QMutexLocker locker(&m_mutex);
    m_sessionPermissions[toolName] = true;
    
    // If we're waiting for this permission, trigger the wait loop
    if (!m_pendingTool.isEmpty() && m_pendingTool == toolName) {
        m_permissionGranted = true;
        if (m_waitLoop) {
            m_waitLoop->quit();
        }
    }
    
    emit permissionGranted(toolName);
}

void PermissionManager::denyPermission(const QString &toolName)
{
    QMutexLocker locker(&m_mutex);
    m_sessionPermissions[toolName] = false;
    
    // If we're waiting for this permission, trigger the wait loop
    if (!m_pendingTool.isEmpty() && m_pendingTool == toolName) {
        m_permissionGranted = false;
        if (m_waitLoop) {
            m_waitLoop->quit();
        }
    }
    
    emit permissionDenied(toolName);
}

void PermissionManager::clearSessionPermissions()
{
    QMutexLocker locker(&m_mutex);
    m_sessionPermissions.clear();
}
