#ifndef CODEBASESEARCHTOOL_H
#define CODEBASESEARCHTOOL_H

#include "toolregistry.h"
#include "codebaseindexer.h"

class CodebaseSearchTool : public AgentTool
{
    Q_OBJECT

public:
    explicit CodebaseSearchTool(CodebaseIndexer *indexer, QObject *parent = nullptr);
    QString name() const override;
    QString description() const override;
    QJsonObject parametersSchema() const override;
    QJsonObject execute(const QJsonObject &args) override;

private:
    CodebaseIndexer *m_indexer;
};

#endif
