#include "batchededitstool.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <KLocalizedString>

BatchedEditTool::BatchedEditTool(QObject *parent)
    : AgentTool(parent)
{
}

QString BatchedEditTool::name() const
{
    return "batched_edits";
}

QString BatchedEditTool::description() const
{
    return QObject::tr(
        "Apply multiple file edits in a single batch. "
        "Each edit specifies a file path, an old string to find, and a new string to replace it with. "
        "All edits are applied atomically - if any edit fails, none are applied. "
        "Use this for coordinated multi-file changes like refactoring or updating configurations."
    );
}

QJsonObject BatchedEditTool::parametersSchema() const
{
    QJsonObject occurrenceObj;
    occurrenceObj["type"] = "integer";
    occurrenceObj["description"] = "Which occurrence to replace (0=first, -1=all). Default: 0";
    occurrenceObj["default"] = 0;
    
    QJsonObject fileObj;
    fileObj["type"] = "string";
    fileObj["description"] = "Path to the file to edit (relative or absolute)";
    
    QJsonObject oldStringObj;
    oldStringObj["type"] = "string";
    oldStringObj["description"] = "Text to find in the file";
    
    QJsonObject newStringObj;
    newStringObj["type"] = "string";
    newStringObj["description"] = "Text to replace oldString with";
    
    QJsonObject editsItemProps;
    editsItemProps["file"] = fileObj;
    editsItemProps["oldString"] = oldStringObj;
    editsItemProps["newString"] = newStringObj;
    editsItemProps["occurrence"] = occurrenceObj;
    
    QJsonObject editsItem;
    editsItem["type"] = "object";
    editsItem["properties"] = editsItemProps;
    QJsonArray requiredEditFields;
    requiredEditFields.append("file");
    requiredEditFields.append("oldString");
    requiredEditFields.append("newString");
    editsItem["required"] = requiredEditFields;
    
    QJsonObject editsProp;
    editsProp["type"] = "array";
    editsProp["description"] = "List of file edits to apply";
    editsProp["items"] = editsItem;
    
    QJsonObject properties;
    properties["edits"] = editsProp;
    
    QJsonObject schema;
    schema["type"] = "object";
    schema["properties"] = properties;
    QJsonArray requiredFields;
    requiredFields.append("edits");
    schema["required"] = requiredFields;
    
    return schema;
}

static QJsonObject makeError(const QString &error)
{
    QJsonObject obj;
    obj["success"] = false;
    obj["error"] = error;
    return obj;
}

static QJsonObject makeSuccess(const QString &message, const QVector<QString> &files)
{
    QJsonObject obj;
    obj["success"] = true;
    obj["message"] = message;
    QJsonArray arr;
    for (const QString &f : files) {
        arr.append(f);
    }
    obj["files"] = arr;
    return obj;
}

QJsonObject BatchedEditTool::execute(const QJsonObject &args)
{
    QJsonArray edits = args["edits"].toArray();
    
    if (edits.isEmpty()) {
        return makeError(i18n("No edits provided"));
    }
    
    struct EditRequest {
        QString filePath;
        QString oldString;
        QString newString;
        int occurrence;
        QString content;
    };
    
    QVector<EditRequest> validatedEdits;
    validatedEdits.reserve(edits.size());
    
    for (const QJsonValue &editVal : edits) {
        QJsonObject edit = editVal.toObject();
        QString filePath = edit["file"].toString();
        QString oldString = edit["oldString"].toString();
        QString newString = edit["newString"].toString();
        int occurrence = edit["occurrence"].toInt(0);
        
        if (filePath.isEmpty() || oldString.isEmpty()) {
            return makeError(i18n("Invalid edit: file and oldString are required"));
        }
        
        QString fullPath = filePath;
        if (!QDir::isAbsolutePath(fullPath)) {
            fullPath = QDir::current().absoluteFilePath(filePath);
        }
        
        QFile file(fullPath);
        if (!file.exists()) {
            return makeError(i18n("File does not exist: %1", fullPath));
        }
        
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return makeError(i18n("Cannot read file: %1", fullPath));
        }
        
        QString content = QString::fromUtf8(file.readAll());
        file.close();
        
        if (!content.contains(oldString)) {
            return makeError(i18n("Text not found in %1: %2", fullPath, oldString));
        }
        
        validatedEdits.append({fullPath, oldString, newString, occurrence, content});
    }
    
    QVector<QString> appliedFiles;
    appliedFiles.reserve(validatedEdits.size());
    
    for (const EditRequest &edit : validatedEdits) {
        QString newContent = edit.content;
        
        if (edit.occurrence == -1) {
            newContent.replace(edit.oldString, edit.newString);
        } else {
            int pos = newContent.indexOf(edit.oldString);
            int count = 0;
            while (pos != -1 && count < edit.occurrence + 1) {
                if (count == edit.occurrence) {
                    newContent.replace(pos, edit.oldString.length(), edit.newString);
                    break;
                }
                pos = newContent.indexOf(edit.oldString, pos + 1);
                count++;
            }
        }
        
        QFile file(edit.filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return makeError(i18n("Cannot write to file: %1", edit.filePath));
        }
        
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << newContent;
        file.close();
        
        appliedFiles.append(edit.filePath);
    }
    
    QString msg = i18n("Successfully applied %1 edits to %2 files", 
                        validatedEdits.size(), appliedFiles.size());
    return makeSuccess(msg, appliedFiles);
}
