#ifndef PERMISSIONMANAGER_H
#define PERMISSIONMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

enum class PermissionPolicy {
    Allow,
    Deny,
    Confirm
};

class PermissionManager : public QObject
{
    Q_OBJECT

public:
    explicit PermissionManager(QObject *parent = nullptr);
    ~PermissionManager() override;

    void setDefaultPolicy(PermissionPolicy policy);
    PermissionPolicy getDefaultPolicy() const { return m_defaultPolicy; }

    void setToolPolicy(const QString &toolName, PermissionPolicy policy);
    PermissionPolicy getToolPolicy(const QString &toolName) const;

    bool requiresConfirmation(const QString &toolName) const;
    bool isAllowed(const QString &toolName) const;

public slots:
    bool requestPermission(const QString &toolName);
    void grantPermission(const QString &toolName);
    void denyPermission(const QString &toolName);
    void clearSessionPermissions();

signals:
    void permissionRequested(const QString &toolName);
    void permissionGranted(const QString &toolName);
    void permissionDenied(const QString &toolName);

private:
    PermissionPolicy m_defaultPolicy = PermissionPolicy::Confirm;
    QMap<QString, PermissionPolicy> m_toolPolicies;
    QMap<QString, bool> m_sessionPermissions;
};

#endif
