#include "permissionmanager.h"

PermissionManager::PermissionManager(QObject *parent)
    : QObject(parent)
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
    if (m_sessionPermissions.contains(toolName)) {
        return m_sessionPermissions[toolName];
    }
    PermissionPolicy policy = getToolPolicy(toolName);
    return policy == PermissionPolicy::Allow;
}

bool PermissionManager::requestPermission(const QString &toolName)
{
    emit permissionRequested(toolName);
    if (!requiresConfirmation(toolName)) {
        return isAllowed(toolName);
    }
    return false;
}

void PermissionManager::grantPermission(const QString &toolName)
{
    m_sessionPermissions[toolName] = true;
    emit permissionGranted(toolName);
}

void PermissionManager::denyPermission(const QString &toolName)
{
    m_sessionPermissions[toolName] = false;
    emit permissionDenied(toolName);
}

void PermissionManager::clearSessionPermissions()
{
    m_sessionPermissions.clear();
}
