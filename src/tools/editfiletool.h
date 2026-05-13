#ifndef EDITFILETOOL_H
#define EDITFILETOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include "../checkpointmanager.h"
#include "../ui/diffpreviewdialog.h"
#include <QFile>
#include <QTextStream>

class EditFileTool : public AgentTool
{
    Q_OBJECT

public:
    explicit EditFileTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "edit_file"; }
    QString description() const override {
        return i18n("Edits a file by replacing old_text with new_text.");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject pathProp;
        pathProp["type"] = "string";
        pathProp["description"] = i18n("Path of the file to edit");
        properties["path"] = pathProp;
        
        QJsonObject oldTextProp;
        oldTextProp["type"] = "string";
        oldTextProp["description"] = i18n("Existing text to find");
        properties["old_text"] = oldTextProp;
        
        QJsonObject newTextProp;
        newTextProp["type"] = "string";
        newTextProp["description"] = i18n("New text to insert");
        properties["new_text"] = newTextProp;
        
        QJsonObject replaceAllProp;
        replaceAllProp["type"] = "boolean";
        replaceAllProp["description"] = i18n("If true, replace all occurrences");
        replaceAllProp["default"] = false;
        properties["replace_all"] = replaceAllProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"path", "old_text", "new_text"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString path = args["path"].toString();
        QString oldText = args["old_text"].toString();
        QString newText = args["new_text"].toString();
        bool replaceAll = args["replace_all"].toBool(false);
        
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QJsonObject{
                {"error", i18n("Cannot open file: ") + path},
                {"success", false}
            };
        }
        
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();
        
        int count = content.count(oldText);
        if (count == 0) {
            return QJsonObject{
                {"error", i18n("Text to replace not found in file")},
                {"success", false}
            };
        }
        
        if (!replaceAll && count > 1) {
            return QJsonObject{
                {"warning", i18n("Found N occurrences. Use replace_all=true to replace all.").replace("N", QString::number(count))},
                {"success", false}
            };
        }
        
        QString backupPath = CheckpointManager::createBackup(path);
        if (backupPath.isEmpty()) {
            return QJsonObject{
                {"error", i18n("Cannot create backup for file: ") + path},
                {"success", false}
            };
        }
        
        // Compute new content
        QString newContent = content;
        if (replaceAll) {
            newContent.replace(oldText, newText);
        } else {
            newContent.replace(oldText, newText, Qt::CaseSensitive);
        }
        
        // Show preview dialog and wait for user confirmation
        DiffPreviewDialog dlg(path, content, newContent);
        if (dlg.exec() != QDialog::Accepted) {
            return QJsonObject{ {"error", "Edit rejected by user"}, {"success", false} };
        }
        
        // User accepted - write the file
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return QJsonObject{
                {"error", i18n("Cannot write to file: ") + path},
                {"success", false}
            };
        }
        
        QTextStream out(&file);
        out << newContent;
        file.close();
        
        CheckpointManager::cleanupOldBackups(path);
        
        return QJsonObject{
            {"success", true},
            {"path", path},
            {"replacements", replaceAll ? count : 1},
            {"backup", backupPath}
        };
    }

    bool requiresPermission() const override { return true; }
};

#endif
