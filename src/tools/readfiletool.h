#ifndef READFILETOOL_H
#define READFILETOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QFile>
#include <QFileInfo>
#include <QStringConverter>
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
        static constexpr qint64 kMaxFileBytes = 5*1024 * 1024; // 5 MiB
        const QString path = args["path"].toString().trimmed();
        if (path.isEmpty()) {
            return QJsonObject{
                {"error", i18n("File path cannot be empty")},
                {"success", false}
            };
        }

        const QFileInfo info(path);
        if (!info.exists() || !info.isFile()) {
            return QJsonObject{
                {"error", i18n("File does not exist or is not a regular file: %1", path)},
                {"success", false}
            };
        }

        if (info.size() > kMaxFileBytes) {
            return QJsonObject{
                {"error", i18n("File exceeds maximum allowed size (%1 bytes): %2", kMaxFileBytes, path)},
                {"success", false}
            };
        }

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QJsonObject{
                {"error", i18n("Cannot read file: ") + path},
                {"success", false}
            };
        }

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        const QString content = in.readAll();
        const bool truncated = !in.atEnd();
        file.close();

        const int lineCount = content.isEmpty() ? 0 : content.split('\n').size();

        return QJsonObject{
            {"success", true},
            {"content", content},
            {"path", path},
            {"lines", lineCount},
            {"truncated", truncated}
        };
    }

    bool requiresPermission() const override { return true; }
};

#endif
