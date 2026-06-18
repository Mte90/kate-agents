#ifndef CUSTOMTOOLMANAGER_H
#define CUSTOMTOOLMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QDir>
#include "toolregistry.h"

class CustomToolManager : public QObject
{
    Q_OBJECT

public:
    explicit CustomToolManager(QObject *parent = nullptr);
    
    void setConfigDirectory(const QString &dir);
    void loadCustomTools();
    void saveCustomTools();
    
    bool registerTool(const QString &name, const QString &description,
                     const QJsonObject &schema, const QString &command);
    bool unregisterTool(const QString &name);
    
    QVector<QString> toolNames() const;
    AgentTool *tool(const QString &name) const;

signals:
    void toolsLoaded();
    void toolAdded(const QString &name);
    void toolRemoved(const QString &name);

private:
    void loadToolFromFile(const QString &filePath);
    QString configDirectory() const { return m_configDir.absolutePath(); }
    
    QMap<QString, QJsonObject> m_toolDefinitions;
    QMap<QString, AgentTool*> m_tools;
    QDir m_configDir;
};

#endif
