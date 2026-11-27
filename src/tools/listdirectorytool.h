#ifndef LISTDIRECTORYTOOL_H
#define LISTDIRECTORYTOOL_H

#include <KLocalizedString>

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QDir>
#include <QFileInfo>
#include <QSet>

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
        const QString path = args["path"].toString();
        const bool recursive = args["recursive"].toBool(false);

        if (path.isEmpty()) {
            return QJsonObject{
                {"error", i18n("Directory path cannot be empty")},
                {"success", false}
            };
        }

        const QDir dir(path);
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

        QSet<QString> visitedDirs;
        int totalEntries = 0;
        const QJsonArray entries = listDirectoryEntries(dir, recursive, 0, visitedDirs, totalEntries);

        return QJsonObject{
            {"success", true},
            {"entries", entries},
            {"path", path},
            {"recursive", recursive}
        };
    }

    bool requiresPermission() const override { return true; }

private:
    static constexpr int kMaxDepth = 6;
    static constexpr int kMaxEntries = 1000;

    QJsonArray listDirectoryEntries(const QDir &dir,
                                    bool recursive,
                                    int depth,
                                    QSet<QString> &visitedDirs,
                                    int &totalEntries)
    {
        QJsonArray entries;
        if (totalEntries >= kMaxEntries) {
            return entries;
        }

        const QString canonicalDir = QFileInfo(dir.absolutePath()).canonicalFilePath();
        if (!canonicalDir.isEmpty()) {
            if (visitedDirs.contains(canonicalDir)) {
                return entries;
            }
            visitedDirs.insert(canonicalDir);
        }

        const QFileInfoList infoList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        for (const QFileInfo &info : infoList) {
            if (totalEntries >= kMaxEntries) {
                break;
            }

            QJsonObject entry;
            entry["name"] = info.fileName();
            entry["path"] = info.absoluteFilePath();
            totalEntries++;

            if (info.isSymLink()) {
                entry["type"] = "symlink";
            } else if (info.isDir()) {
                entry["type"] = "directory";
                if (recursive && depth < kMaxDepth) {
                    const QString targetPath = info.canonicalFilePath();
                    if (!targetPath.isEmpty() && !visitedDirs.contains(targetPath)) {
                        QDir subDir(targetPath);
                        entry["entries"] = listDirectoryEntries(subDir, recursive, depth + 1, visitedDirs, totalEntries);
                    }
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
