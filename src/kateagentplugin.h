#ifndef KATEAGENTPLUGIN_H
#define KATEAGENTPLUGIN_H

#include <KTextEditor/Plugin>
#include <KTextEditor/SessionConfigInterface>
#include <KTextEditor/MainWindow>
#include <QObject>
#include <QAction>
#include <QPointer>
#include <KTextEditor/View>
#include <QSet>
#include <QVariantList>

class AgentLoop;
class LLMProvider;
class ToolRegistry;
class ConfigManager;
class PermissionManager;
class AgentPanel;
class ContextMenuHandler;
class CodebaseIndexer;
class CustomToolManager;
class AuditLogger;

class KateAgentPlugin : public KTextEditor::Plugin
{
    Q_OBJECT
public:
    explicit KateAgentPlugin(QObject *parent, const QVariantList & = QVariantList());
    ~KateAgentPlugin() override;
    QObject *createView(KTextEditor::MainWindow *mw) override;
    
    KTextEditor::ConfigPage *configPage(int number = 0, QWidget *parent = nullptr) override;
    
    int configPages() const override
    {
        return 1;
    }
    
    LLMProvider *provider() const { return m_provider; }
    ToolRegistry *registry() const { return m_registry; }
    AgentLoop *agentLoop() const { return m_agentLoop; }
    ConfigManager *config() const { return m_config; }
    PermissionManager *permissionManager() const { return m_permissions; }
    CodebaseIndexer *codebaseIndexer() const { return m_codebaseIndexer; }
    
    signals:
    void settingsChanged();
    
public slots:
    void savePanelState();

private:
    LLMProvider *m_provider = nullptr;
    ToolRegistry *m_registry = nullptr;
    AgentLoop *m_agentLoop = nullptr;
    QPointer<AgentPanel> m_agentPanel;
    ConfigManager *m_config = nullptr;
    PermissionManager *m_permissions = nullptr;
    CodebaseIndexer *m_codebaseIndexer = nullptr;
    CustomToolManager *m_customToolManager = nullptr;
    AuditLogger *m_auditLogger = nullptr;
    ContextMenuHandler *m_contextMenuHandler = nullptr;
    QSet<KTextEditor::View*> m_installedViews;
    
    friend class AgentConfigPage;
};

// Forward declaration for AgentGuiClient private method
class AgentGuiClient : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    AgentGuiClient(KateAgentPlugin *plugin, KTextEditor::MainWindow *mainwindow);
    ~AgentGuiClient() override;
    QWidget* panel() const;
    
private:
    void createToolView();
    
    KateAgentPlugin *m_plugin;
    KTextEditor::MainWindow *m_mainWindow;
    AgentPanel *m_panel;
    QWidget *m_toolView = nullptr;
};

#endif
