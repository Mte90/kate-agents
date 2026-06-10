#ifndef CREATEDIRECTORYTOOL_H
#define CREATEDIRECTORYTOOL_H

#include <KLocalizedString>

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QDir>

class CreateDirectoryTool : public AgentTool
{
    Q_OBJECT

public:
    explicit CreateDirectoryTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "createdirectory"; }

    QString description() const override {
        return i18n("Create a new directory - takes path, creates parent dirs if needed");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject pathProp;
        pathProp["type"] = "string";
        pathProp["description"] = i18n("Path of the directory to create");
        properties["path"] = pathProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"path"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString path = args["path"].toString();
        
        QDir dir;
        if (dir.mkpath(path)) {
            return QJsonObject{
                {"success", true},
                {"message", i18n("Directory created successfully: ") + path},
                {"path", path}
            };
        } else {
            return QJsonObject{
                {"success", false},
                {"message", i18n("Failed to create directory: ") + path}
            };
        }
    }

    bool requiresPermission() const override { return true; }
};

#endif