#include "customtoolmanager.h"
#include "customtool.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

CustomToolManager::CustomToolManager(QObject *parent)
    : QObject(parent)
{
    // Default config directory: ~/.local/share/kate-agents/custom-tools/
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/custom-tools";
    m_configDir.setPath(configPath);
    if (!m_configDir.exists()) {
        m_configDir.mkpath(".");
    }
}

void CustomToolManager::setConfigDirectory(const QString &dir)
{
    m_configDir.setPath(dir);
    if (!m_configDir.exists()) {
        m_configDir.mkpath(".");
    }
}

void CustomToolManager::loadCustomTools()
{
    // Clear existing tools
    for (auto it = m_tools.begin(); it != m_tools.end(); ++it) {
        delete it.value();
    }
    m_tools.clear();
    m_toolDefinitions.clear();
    
    // Load all JSON files from config directory
    QStringList files = m_configDir.entryList(QStringList() << "*.json", QDir::Files);
    for (const QString &fileName : files) {
        loadToolFromFile(m_configDir.absoluteFilePath(fileName));
    }
    
    emit toolsLoaded();
}

void CustomToolManager::loadToolFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open custom tool file:" << filePath;
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format in:" << filePath;
        return;
    }
    
    QJsonObject obj = doc.object();
    
    QString name = obj["name"].toString();
    QString description = obj["description"].toString();
    QJsonObject schema = obj["parameters"].toObject();
    QString command = obj["command"].toString();
    
    if (name.isEmpty() || command.isEmpty()) {
        qWarning() << "Missing required fields in custom tool:" << filePath;
        return;
    }
    
    // Register tool
    if (registerTool(name, description, schema, command)) {
        qDebug() << "Loaded custom tool:" << name;
    }
}

bool CustomToolManager::registerTool(const QString &name, const QString &description,
                                     const QJsonObject &schema, const QString &command)
{
    // Check for duplicate
    if (m_tools.contains(name)) {
        qWarning() << "Custom tool already exists:" << name;
        return false;
    }
    
    // Create tool
    CustomTool *tool = new CustomTool(name, description, schema, command);
    m_tools[name] = tool;
    
    // Save definition to file
    QJsonObject def{
        {"name", name},
        {"description", description},
        {"parameters", schema},
        {"command", command}
    };
    
    QString filePath = m_configDir.absoluteFilePath(name + ".json");
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(def);
        file.write(doc.toJson());
        file.close();
    }
    
    m_toolDefinitions[name] = def;
    emit toolAdded(name);
    return true;
}

bool CustomToolManager::unregisterTool(const QString &name)
{
    if (!m_tools.contains(name)) {
        return false;
    }
    
    delete m_tools[name];
    m_tools.remove(name);
    m_toolDefinitions.remove(name);
    
    // Remove file
    QString filePath = m_configDir.absoluteFilePath(name + ".json");
    QFile::remove(filePath);
    
    emit toolRemoved(name);
    return true;
}

QVector<QString> CustomToolManager::toolNames() const
{
    QVector<QString> names;
    for (auto it = m_tools.begin(); it != m_tools.end(); ++it) {
        names.append(it.key());
    }
    return names;
}

AgentTool *CustomToolManager::tool(const QString &name) const
{
    return m_tools.value(name);
}
