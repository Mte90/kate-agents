#ifndef FINDPATHTOOL_H
#define FINDPATHTOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QProcess>
#include <QJsonArray>

class FindPathTool : public AgentTool
{
    Q_OBJECT

public:
    explicit FindPathTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "findpath"; }
    QString description() const override {
        return i18n("Find files by name - takes a pattern like *.cpp or filename");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        
        QJsonObject properties;
        
        QJsonObject patternProp;
        patternProp["type"] = "string";
        patternProp["description"] = i18n("Pattern to search for (glob-style, e.g., *.txt)");
        properties["pattern"] = patternProp;
        
        QJsonObject typeProp;
        typeProp["type"] = "string";
        typeProp["description"] = i18n("Type to search for: 'file' or 'directory'");
        typeProp["default"] = "file";
        properties["type"] = typeProp;
        
        QJsonObject maxResultsProp;
        maxResultsProp["type"] = "integer";
        maxResultsProp["description"] = i18n("Maximum number of results to return");
        maxResultsProp["default"] = 50;
        properties["max_results"] = maxResultsProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"pattern"};
        
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        const QString pattern = args["pattern"].toString().trimmed();
        const QString type = args["type"].toString("file");
        const int maxResults = args["max_results"].toInt(50);
        
        if (pattern.isEmpty() || pattern.startsWith(QLatin1Char('-'))) {
            return QJsonObject{
                {"error", i18n("Invalid search pattern")},
                {"success", false}
            };
        }

        if (type != "file" && type != "directory") {
            return QJsonObject{
                {"error", i18n("Invalid type '%1': must be 'file' or 'directory'", type)},
                {"success", false}
            };
        }

        const QStringList findArgs{
            ".",
            "-maxdepth", "5",
            "-type", (type == "directory" ? "d" : "f"),
            "-name", pattern
        };
        
        QProcess process;
        process.setProgram("find");
        process.setArguments(findArgs);
        process.setProcessChannelMode(QProcess::SeparateChannels);
        
        process.start();
        if (!process.waitForStarted(2000)) {
            return QJsonObject{
                {"error", i18n("Failed to start find command: %1", process.errorString())},
                {"success", false}
            };
        }

        if (!process.waitForFinished(5000)) {
            process.kill();
            process.waitForFinished(1000);
            return QJsonObject{
                {"error", i18n("Find command timed out")},
                {"success", false}
            };
        }

        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            const QString errText = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
            return QJsonObject{
                {"error", i18n("Find command failed: %1", errText.isEmpty() ? process.errorString() : errText)},
                {"success", false}
            };
        }
        
        const QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        
        QJsonArray results;
        for (const QString &line : lines) {
            if (results.size() >= maxResults) {
                break;
            }
            const QString trimmed = line.trimmed();
            if (!trimmed.isEmpty()) {
                results.append(trimmed);
            }
        }
        
        return QJsonObject{
            {"success", true},
            {"results", results},
            {"count", results.size()},
            {"pattern", pattern},
            {"type", type}
        };
    }

    bool requiresPermission() const override { return true; }
};

#endif
