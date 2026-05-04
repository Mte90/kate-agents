#ifndef TOOLREGISTRY_H
#define TOOLREGISTRY_H

#include "llmprovider.h"
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QThread>
#include <QEventLoop>
#include <QtConcurrent>
#include <QFuture>
#include <vector>

class AgentTool : public QObject
{
    Q_OBJECT

public:
    explicit AgentTool(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~AgentTool() = default;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QJsonObject parametersSchema() const = 0;
    virtual QJsonObject execute(const QJsonObject &args) = 0;
    virtual bool requiresPermission() const { return false; }

    ToolDefinition toToolDefinition() const {
        ToolDefinition def;
        def.type = "function";
        def.function.name = name();
        def.function.description = description();
        def.function.parameters = parametersSchema();
        return def;
    }
};

class ToolRegistry : public QObject
{
    Q_OBJECT

public:
    explicit ToolRegistry(QObject *parent = nullptr);
    ~ToolRegistry() override;

    void registerTool(AgentTool *tool);
    void unregisterTool(const QString &name);
    
    std::vector<ToolDefinition> getToolDefinitions() const;
    QJsonObject executeTool(const QString &name, const QJsonObject &args, int timeoutSeconds = -1);
    bool hasTool(const QString &name) const;
    bool requiresPermission(const QString &name) const;

signals:
    void toolRegistered(const QString &name);
    void toolUnregistered(const QString &name);

private:
    QMap<QString, AgentTool*> m_tools;
};

#endif
