#ifndef BATCHEDEDTSTOOL_H
#define BATCHEDEDTSTOOL_H

#include "toolregistry.h"

class BatchedEditTool : public AgentTool
{
    Q_OBJECT

public:
    explicit BatchedEditTool(QObject *parent = nullptr);
    QString name() const override;
    QString description() const override;
    QJsonObject parametersSchema() const override;
    QJsonObject execute(const QJsonObject &args) override;
};

#endif
