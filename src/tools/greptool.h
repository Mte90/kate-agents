#ifndef GREPTOOL_H
#define GREPTOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

class GrepTool : public AgentTool
{
    Q_OBJECT

public:
    explicit GrepTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "grep"; }
    QString description() const override {
        return i18n("Searches files in the project using a regex pattern.");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject patternProp;
        patternProp["type"] = "string";
        patternProp["description"] = i18n("Regex pattern to search for");
        properties["pattern"] = patternProp;
        
        QJsonObject pathProp;
        pathProp["type"] = "string";
        pathProp["description"] = i18n("Directory to search in (default: .)");
        properties["path"] = pathProp;
        
        QJsonObject includeProp;
        includeProp["type"] = "string";
        includeProp["description"] = i18n("Glob pattern for file inclusion");
        properties["include"] = includeProp;
        
        QJsonObject maxResultsProp;
        maxResultsProp["type"] = "integer";
        maxResultsProp["description"] = i18n("Maximum number of results");
        maxResultsProp["default"] = 50;
        properties["max_results"] = maxResultsProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"pattern"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString pattern = args["path"].toString();
        QString searchPath = args["path"].toString(".");
        QString include = args["include"].toString("*");
        int maxResults = args["max_results"].toInt(50);
        
        QRegularExpression regex(pattern);
        if (!regex.isValid()) {
            return QJsonObject{
                {"error", i18n("Invalid regex pattern: ") + regex.errorString()},
                {"success", false}
            };
        }
        
        QJsonArray results;
        int count = 0;
        
        QDir dir(searchPath);
        QFileInfoList files = dir.entryInfoList(QStringList() << include, QDir::Files | QDir::Readable);
        
        for (const QFileInfo &fileInfo : files) {
            if (count >= maxResults) break;
            
            QFile file(fileInfo.absoluteFilePath());
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
            
            QTextStream in(&file);
            int lineNum = 0;
            while (!in.atEnd() && count < maxResults) {
                lineNum++;
                QString line = in.readLine();
                QRegularExpressionMatch match = regex.match(line);
                if (match.hasMatch()) {
                    results.append(QJsonObject{
                        {"file", fileInfo.absoluteFilePath()},
                        {"line", lineNum},
                        {"content", line},
                        {"match", match.captured(0)}
                    });
                    count++;
                }
            }
            file.close();
        }
        
        return QJsonObject{
            {"success", true},
            {"results", results},
            {"count", count}
        };
    }

    bool requiresPermission() const override { return false; }
};

#endif
