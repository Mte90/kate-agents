#include "customtool.h"
#include <QJsonDocument>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <KLocalizedString>

CustomTool::CustomTool(const QString &name, const QString &description,
                       const QJsonObject &schema, const QString &command,
                       QObject *parent)
    : AgentTool(parent), m_name(name), m_description(description), 
      m_schema(schema), m_command(command)
{
}

QString CustomTool::name() const
{
    return m_name;
}

QString CustomTool::description() const
{
    return m_description;
}

QJsonObject CustomTool::parametersSchema() const
{
    return m_schema;
}

static QJsonObject makeError(const QString &error)
{
    QJsonObject obj;
    obj["success"] = false;
    obj["error"] = error;
    return obj;
}

static QJsonObject makeSuccess(const QJsonObject &data)
{
    QJsonObject obj = data;
    obj["success"] = true;
    return obj;
}

QJsonObject CustomTool::execute(const QJsonObject &args)
{
    QString command = m_command;
    
    for (auto it = args.begin(); it != args.end(); ++it) {
        QString key = it.key();
        QString value = it.value().toString();
        command.replace(QString("${%1}").arg(key), value);
        command.replace(QString("$%1").arg(key), value);
    }
    
    QStringList parts = command.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        return makeError(i18n("Empty command"));
    }
    
    QString executable = parts.first();
    QStringList arguments = parts.mid(1);
    
    QProcess process;
    process.setProgram(executable);
    process.setArguments(arguments);
    process.setWorkingDirectory(QDir::currentPath());
    process.setProcessChannelMode(QProcess::MergedChannels);
    
    process.start();
    bool finished = process.waitForFinished(30000);
    
    if (!finished) {
        process.kill();
        return makeError(i18n("Command timed out: %1", command));
    }
    
    int exitCode = process.exitCode();
    QString output = QString::fromUtf8(process.readAll());
    
    if (exitCode != 0) {
        return makeError(i18n("Command failed with exit code %1: %2", exitCode, output));
    }
    
    QJsonObject result;
    result["output"] = output;
    result["command"] = command;
    return makeSuccess(result);
}
