#ifndef APPLYDIFFTOOL_H
#define APPLYDIFFTOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

class ApplyDiffTool : public AgentTool
{
    Q_OBJECT

public:
    explicit ApplyDiffTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "apply_diff"; }
    QString description() const override {
        return i18n("Applies a unified diff to a file. Use this to make targeted edits.");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject pathProp;
        pathProp["type"] = "string";
        pathProp["description"] = i18n("Path of the file to modify");
        properties["path"] = pathProp;
        
        QJsonObject diffProp;
        diffProp["type"] = "string";
        diffProp["description"] = i18n("Unified diff to apply");
        properties["diff"] = diffProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"path", "diff"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString path = args["path"].toString();
        QString diff = args["diff"].toString();
        
        if (path.isEmpty()) {
            return QJsonObject{
                {"error", i18n("File path is required")},
                {"success", false}
            };
        }
        
        if (diff.isEmpty()) {
            return QJsonObject{
                {"error", i18n("Diff content is required")},
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
        QString content = in.readAll();
        file.close();
        
        QString modifiedContent = applyUnifiedDiff(content, diff);
        
        if (modifiedContent.isEmpty()) {
            return QJsonObject{
                {"error", i18n("Failed to apply diff. The patch may not apply cleanly.")},
                {"success", false}
            };
        }
        
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return QJsonObject{
                {"error", i18n("Cannot write to file: ") + path},
                {"success", false}
            };
        }
        
        QTextStream out(&file);
        out << modifiedContent;
        file.close();
        
        return QJsonObject{
            {"success", true},
            {"message", i18n("Diff applied successfully to %1", path)},
            {"path", path}
        };
    }

    bool requiresPermission() const override { return true; }

private:
    QString applyUnifiedDiff(const QString &originalContent, const QString &diff) {
        Q_UNUSED(originalContent);
        QStringList lines = diff.split('\n');
        
        if (diff.contains("+++")) {
            bool foundNewHeader = false;
            QStringList newLines;
            
            for (const QString &line : lines) {
                if (line.startsWith("+++")) {
                    foundNewHeader = true;
                    continue;
                }
                
                if (foundNewHeader) {
                    if (line.startsWith("-")) {
                        continue;
                    } else if (line.startsWith("+")) {
                        newLines.append(line.mid(1));
                    } else if (line.startsWith("@@") || line.startsWith("diff ") || line.startsWith("index ")) {
                        continue;
                    } else {
                        newLines.append(line);
                    }
                }
            }
            
            if (!newLines.isEmpty()) {
                return newLines.join('\n');
            }
        }
        
        return QString();
    }
};

#endif
