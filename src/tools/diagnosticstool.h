#ifndef DIAGNOSTICSTOOL_H
#define DIAGNOSTICSTOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Document>

class DiagnosticsTool : public AgentTool
{
    Q_OBJECT

public:
    explicit DiagnosticsTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "diagnostics"; }
    QString description() const override {
        return i18n("Get code errors and warnings - takes optional file path or checks entire project");
    }

    void setMainWindow(KTextEditor::MainWindow *mw) {
        m_mainWindow = mw;
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject filePathProp;
        filePathProp["type"] = "string";
        filePathProp["description"] = i18n("Path to the file to get diagnostics for (optional, defaults to all open documents)");
        properties["file_path"] = filePathProp;
        
        schema["properties"] = properties;
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        Q_UNUSED(args)
        return QJsonObject{
            {"error", i18n("Diagnostics API not available in this KTextEditor version")},
            {"success", false}
        };
    }

    bool requiresPermission() const override { return false; }

private:
    KTextEditor::MainWindow *m_mainWindow = nullptr;
};

#endif