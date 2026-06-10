#ifndef READFILETOOL_H
#define READFILETOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QFile>
#include <QTextStream>

class ReadFileTool : public AgentTool
{
    Q_OBJECT

public:
    explicit ReadFileTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "read_file"; }
    QString description() const override {
        return i18n("Read a file - specify the full path to display its contents");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        QJsonObject pathProp;
        pathProp["type"] = "string";
        pathProp["description"] = i18n("Path of the file to read");
        properties["path"] = pathProp;
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"path"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString path = args["path"].toString();
        
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QJsonObject{
                {"error", i18n("Cannot read file: ") + path},
                {"success", false}
            };
        }

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        int lineCount = content.split('\n').size();

        return QJsonObject{
            {"success", true},
            {"content", content},
            {"path", path},
            {"lines", lineCount}
        };
    }

    bool requiresPermission() const override { return false; }
};

#endif
