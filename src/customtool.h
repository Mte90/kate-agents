#ifndef CUSTOMTOOL_H
#define CUSTOMTOOL_H

#include "toolregistry.h"
#include <QJsonObject>
#include <QProcess>

class CustomTool : public AgentTool
{
    Q_OBJECT

public:
    explicit CustomTool(const QString &name, const QString &description,
                       const QJsonObject &schema, const QString &command,
                       QObject *parent = nullptr);
    QString name() const override;
    QString description() const override;
    QJsonObject parametersSchema() const override;
    QJsonObject execute(const QJsonObject &args) override;

private:
    QString m_name;
    QString m_description;
    QJsonObject m_schema;
    QString m_command;
};

#endif
