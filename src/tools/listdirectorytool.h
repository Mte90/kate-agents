#ifndef LISTDIRECTORYTOOL_H
#define LISTDIRECTORYTOOL_H

#include <KLocalizedString>

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QDir>
#include <QFileInfo>

class ListDirectoryTool : public AgentTool
{
    Q_OBJECT

public:
    explicit ListDirectoryTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "listdirectory"; }

    QString description() const override {
        return i18n("List directory contents - takes optional path, shows files and subfolders");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";

        QJsonObject properties;

        QJsonObject pathProp;
        pathProp["type"] = "string";
        pathProp["description"] = i18n("Path of the directory to list");
        properties["path"] = pathProp;

        QJsonObject recursiveProp;
        recursiveProp["type"] = "boolean";
        recursiveProp["description"] = i18n("Whether to list subdirectories recursively");
        recursiveProp["default"] = false;
        properties["recursive"] = recursiveProp;

        schema["properties"] = properties;
        schema["required"] = QJsonArray{"path"};

        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString path = args["path"].toString();
        bool recursive = args["recursive"].toBool(false);

        QDir dir(path);
        if (!dir.exists()) {
            return QJsonObject{
                {"error", i18n("Directory does not exist: ") + path},
                {"success", false}
            };
        }

        if (!dir.isReadable()) {
            return QJsonObject{
                {"error", i18n("Directory is not readable: ") + path},
                {"success", false}
            };
        }

        QJsonArray entries = listDirectoryEntries(dir, recursive);

        return QJsonObject{
            {"success", true},
            {"entries", entries},
            {"path", path},
            {"recursive", recursive}
        };
    }

    bool requiresPermission() const override { return false; }

private:
    QJsonArray listDirectoryEntries(const QDir &dir, bool recursive) {
        QJsonArray entries;

        QFileInfoList infoList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for (const QFileInfo &info : infoList) {
            QJsonObject entry;
            entry["name"] = info.fileName();
            entry["path"] = info.absoluteFilePath();

            if (info.isSymLink()) {
                entry["type"] = "symlink";
            } else if (info.isDir()) {
                entry["type"] = "directory";
                if (recursive) {
                    QDir subDir(info.absoluteFilePath());
                    QJsonArray subEntries = listDirectoryEntries(subDir, recursive);
                    entry["entries"] = subEntries;
                }
            } else if (info.isFile()) {
                entry["type"] = "file";
            } else {
                entry["type"] = "unknown";
            }

            entries.append(entry);
        }

        return entries;
    }
};

#endif