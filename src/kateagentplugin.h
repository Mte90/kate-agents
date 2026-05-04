#ifndef KATEAGENTPLUGIN_H
#define KATEAGENTPLUGIN_H
#include <KTextEditor/Plugin>
#include <KTextEditor/MainWindow>
#include <QObject>
#include <QAction>
#include <QPointer>

class AgentLoop;
class LLMProvider;
class ToolRegistry;
class ConfigManager;
class PermissionManager;
class AgentPanel;
class ContextMenuHandler;

class KateAgentPlugin : public KTextEditor::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.KTextEditor.Plugin" FILE "kateagentplugin.json")
public:
    explicit KateAgentPlugin(QObject *p=nullptr, const QVariantList &a=QVariantList());
    ~KateAgentPlugin() override;
    QObject *createView(KTextEditor::MainWindow *mw) override;
    KTextEditor::ConfigPage *configPage(int n, QWidget *w) override;

private:
    LLMProvider *m_provider = nullptr;
    ToolRegistry *m_registry = nullptr;
    AgentLoop *m_agentLoop = nullptr;
    QPointer<AgentPanel> m_agentPanel;
    ConfigManager *m_config = nullptr;
    PermissionManager *m_permissions = nullptr;
    ContextMenuHandler *m_contextMenuHandler = nullptr;
    
    friend class AgentConfigPage;
};
#endif
