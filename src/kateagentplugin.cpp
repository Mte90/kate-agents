#include "kateagentplugin.h"
#include "agentloop.h"
#include "llmprovider.h"
#include "openaiprovider.h"
#include "toolregistry.h"
#include "configmanager.h"
#include "permissionmanager.h"
#include "contextmenhander.h"
#include "ui/agentpanel.h"
#include "ui/agentconfigpage.h"
#include "tools/readfiletool.h"
#include "tools/editfiletool.h"
#include "tools/greptool.h"
#include "tools/terminaltool.h"
#include "tools/websearchtool.h"
#include "tools/urlfetchtool.h"
#include "tools/diagnosticstool.h"
#include "tools/findpathtool.h"
#include "tools/listdirectorytool.h"
#include "tools/createdirectorytool.h"
#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KTextEditor/View>
#include <KTextEditor/MainWindow>
#include <KXMLGUIClient>
#include <KXMLGUIFactory>
#include <QMainWindow>
#include <QDebug>
#include <QTimer>

K_PLUGIN_FACTORY_WITH_JSON(KateAgentPluginFactory, "kateagentplugin.json", registerPlugin<KateAgentPlugin>();)

class AgentGuiClient : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    AgentGuiClient(KateAgentPlugin *plugin, KTextEditor::MainWindow *mainwindow)
        : QObject()
        , KXMLGUIClient()
        , m_plugin(plugin)
        , m_mainWindow(mainwindow)
    {
        setComponentName("kateagent", i18n("Kate Agent"));
        qDebug() << "AgentGuiClient: Constructor starting";
        qDebug() << "AgentGuiClient: setComponentName done";
        
        qDebug() << "AgentGuiClient: Creating AgentPanel...";
        m_panel = new AgentPanel(plugin->m_agentLoop, plugin->m_registry, 
                                 plugin->m_provider, plugin->m_config, 
                                 plugin->m_permissions);
        qDebug() << "AgentGuiClient: AgentPanel created";
        
        qDebug() << "AgentGuiClient: Creating action collection...";
        auto ac = actionCollection();
        qDebug() << "AgentGuiClient: Action collection created";
        
        qDebug() << "AgentGuiClient: Creating toggle action...";
        auto toggleAction = ac->addAction("kateagent-toggle");
        toggleAction->setText(i18n("Toggle Kate Agent Panel"));
        toggleAction->setIcon(QIcon::fromTheme("preferences-system"));
        ac->setDefaultShortcut(toggleAction, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_A));
        qDebug() << "AgentGuiClient: Toggle action created";
        
        connect(toggleAction, &QAction::triggered, m_plugin, [this]() {
            qDebug() << "AgentGuiClient: Toggle action triggered";
            if (!m_toolView) {
                qDebug() << "AgentGuiClient: Creating tool view...";
                m_toolView = m_mainWindow->createToolView(
                    m_plugin, 
                    "agent_panel", 
                    KTextEditor::MainWindow::Right, 
                    QIcon::fromTheme("preferences-system"), 
                    i18n("Kate Agent")
                );
                m_panel->setParent(m_toolView);
                m_panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                QVBoxLayout *layout = new QVBoxLayout(m_toolView);
                layout->setContentsMargins(0, 0, 0, 0);
                layout->setSpacing(0);
                layout->addWidget(m_panel);
                qDebug() << "AgentGuiClient: Tool view created";
            }
            if (m_toolView->isVisible()) {
                m_mainWindow->hideToolView(m_toolView);
            } else {
                m_mainWindow->showToolView(m_toolView);
            }
        });
        qDebug() << "AgentGuiClient: Toggle action connected";

        qDebug() << "AgentGuiClient: Installing GUI client...";
        m_mainWindow->guiFactory()->addClient(this);
        qDebug() << "AgentGuiClient: GUI client installed";

        // Auto-show tool view after event loop starts (deferred to avoid Kate init crash)
        QTimer::singleShot(0, this, [this]() {
            qDebug() << "AgentGuiClient: Auto-showing tool view";
            if (!m_toolView) {
                m_toolView = m_mainWindow->createToolView(
                    m_plugin, 
                    "agent_panel", 
                    KTextEditor::MainWindow::Right, 
                    QIcon::fromTheme("preferences-system"), 
                    i18n("Kate Agent")
                );
                m_panel->setParent(m_toolView);
                m_panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                QVBoxLayout *layout = new QVBoxLayout(m_toolView);
                layout->setContentsMargins(0, 0, 0, 0);
                layout->setSpacing(0);
                layout->addWidget(m_panel);
                qDebug() << "AgentGuiClient: Tool view created (auto-show)";
            }
            m_mainWindow->showToolView(m_toolView);
        });

        qDebug() << "AgentGuiClient: Constructor complete";
    }
    
    ~AgentGuiClient() override {
        qDebug() << "AgentGuiClient: Destructor starting";
        if (m_mainWindow) {
            qDebug() << "AgentGuiClient: Removing GUI client...";
            m_mainWindow->guiFactory()->removeClient(this);
            qDebug() << "AgentGuiClient: GUI client removed";
        }
        qDebug() << "AgentGuiClient: Destructor complete";
    }
    
    QWidget* panel() const { return m_panel; }
    
private:
    KateAgentPlugin *m_plugin;
    KTextEditor::MainWindow *m_mainWindow;
    AgentPanel *m_panel;
    QWidget *m_toolView = nullptr;
};

KateAgentPlugin::KateAgentPlugin(QObject *parent, const QVariantList &) : KTextEditor::Plugin(parent)
{
    qDebug() << "[KateAgentPlugin] Constructor called - plugin loading...";
    m_registry = new ToolRegistry(this);
    m_config = new ConfigManager(this);
    m_config->load();
    m_config->save();  // Force save default config so Kate shows config page
    
    // Initialize provider with config
    auto providerCfg = m_config->getProviderConfig(m_config->getActiveProvider());
    m_provider = new OpenAIProvider(providerCfg.baseUrl, providerCfg.apiKey, this);
    
    m_agentLoop = new AgentLoop(m_provider, m_registry, this);
    m_permissions = new PermissionManager(this);
    
    // Register all tools
    m_registry->registerTool(new ReadFileTool(this));
    m_registry->registerTool(new EditFileTool(this));
    m_registry->registerTool(new GrepTool(this));
    m_registry->registerTool(new TerminalTool(this));
    m_registry->registerTool(new WebSearchTool(this));
    m_registry->registerTool(new URLFetchTool(this));
    m_registry->registerTool(new DiagnosticsTool(this));
    m_registry->registerTool(new FindPathTool(this));
    m_registry->registerTool(new ListDirectoryTool(this));
    m_registry->registerTool(new CreateDirectoryTool(this));
}

KateAgentPlugin::~KateAgentPlugin()
{
    if (m_agentLoop) {
        if (m_agentLoop->isRunning()) {
            m_agentLoop->abort();
        }
        m_agentLoop->saveAllThreads();
    }
}

QObject *KateAgentPlugin::createView(KTextEditor::MainWindow *mw)
{
    qDebug() << "KateAgentPlugin: createView starting";
    
    m_agentLoop->setMainWindow(mw);
    
    return new AgentGuiClient(this, mw);
}

KTextEditor::ConfigPage *KateAgentPlugin::configPage(int number, QWidget *parent)
{
    // Kate calls configPage() with different numbers to discover config pages
    // Return nullptr for number != 0, only create page for number == 0
    if (number != 0) {
        return nullptr;
    }
    Q_UNUSED(parent)
    return new AgentConfigPage(parent, this);
}

#include "kateagentplugin.moc"

