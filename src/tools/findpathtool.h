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
        QString pattern = args["pattern"].toString();
        QString type = args["type"].toString("file");
        int maxResults = args["max_results"].toInt(50);
        
        QStringList findArgs;
        findArgs << ".";
        findArgs << "-type" << (type == "directory" ? "d" : "f");
        findArgs << "-name" << pattern;
        findArgs << "-maxdepth" << "5";
        
        QProcess process;
        process.setProgram("find");
        process.setArguments(findArgs);
        process.setProcessChannelMode(QProcess::MergedChannels);
        
        process.start();
        process.waitForFinished(5000);
        
        if (process.exitCode() != 0) {
            QString error = process.readAllStandardError();
            return QJsonObject{
                {"error", i18n("Find command failed: ") + error},
                {"success", false}
            };
        }
        
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        
        QJsonArray results;
        int count = 0;
        for (const QString &line : lines) {
            if (count >= maxResults) break;
            if (!line.trimmed().isEmpty()) {
                results.append(line.trimmed());
                count++;
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

    bool requiresPermission() const override { return false; }
};

#endif