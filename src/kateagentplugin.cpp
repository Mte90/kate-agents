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
#include <QDockWidget>
#include <QMainWindow>
#include <QDebug>

K_PLUGIN_FACTORY_WITH_JSON(KateAgentPluginFactory, "kateagentplugin.json", registerPlugin<KateAgentPlugin>();)

KateAgentPlugin::KateAgentPlugin(QObject *parent, const QVariantList &) : KTextEditor::Plugin(parent)
{
    qDebug() << "[KateAgentPlugin] Constructor called - plugin loading...";
    m_registry = new ToolRegistry(this);
    m_config = new ConfigManager(this);
    m_config->load();
    
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
    // Create AgentPanel - this connects all AgentLoop signals to UI slots
    m_agentPanel = new AgentPanel(m_agentLoop, m_registry, m_provider, m_config, m_permissions);
    
    // Connect settingsChanged signal to reload models in the panel
    connect(this, &KateAgentPlugin::settingsChanged, m_agentPanel, [this]() {
        if (m_agentPanel) {
            m_agentPanel->reloadModels();
        }
    });
    
    // Set main window reference for editor context access
    m_agentLoop->setMainWindow(mw);
    
    // Create context menu handler
    m_contextMenuHandler = new ContextMenuHandler(m_agentLoop, this);
    
    // Install context menu for existing views
    for (auto *view : mw->views()) {
        m_contextMenuHandler->installContextMenu(view);
    }
    
    // Connect view created signal to install context menu for new views
    connect(mw, &KTextEditor::MainWindow::viewCreated, this, [this](KTextEditor::View *view) {
        m_contextMenuHandler->installContextMenu(view);
    });
    
    // Create dock widget to show the panel in Kate's sidebar
    QDockWidget *dock = new QDockWidget(tr("Kate Agent"), mw->window());
    dock->setObjectName("KateAgentDock");
    dock->setWidget(m_agentPanel);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    
    // Add dock widget to the right sidebar using QMainWindow
    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(mw->window());
    if (mainWindow) {
        mainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
        // Show dock widget by default
        dock->show();
    }
    
    // Create action collection for shortcuts
    auto v = new QObject(this);
    auto ac = new KActionCollection(v);
    ac->setParent(mw->window());
    
    // Action to toggle dock visibility
    auto toggleAction = ac->addAction("kateagent-toggle");
    toggleAction->setText(i18n("Toggle Kate Agent Panel"));
    toggleAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_A));
    toggleAction->setIcon(QIcon::fromTheme("dialog-information"));
    QObject::connect(toggleAction, &QAction::triggered, mw, [dock]() {
        if (dock->isVisible()) {
            dock->hide();
        } else {
            dock->show();
            dock->raise();
            dock->activateWindow();
        }
    });
    
    // Quick ask action
    auto b = ac->addAction("kateagent-quick");
    b->setText(i18n("Quick Ask"));
    b->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_Q));
    QObject::connect(b, &QAction::triggered, mw, [mw]() {
        auto v = mw->activeView();
        if (v && !v->selectionText().isEmpty()) {
            QVariantMap m;
            m["type"] = "Info";
            m["category"] = "Kate Agent";
            m["text"] = "Selected: " + v->selectionText().left(60);
            mw->showMessage(m);
        }
    });
    
    return v;
}

KTextEditor::ConfigPage *KateAgentPlugin::configPage(int number, QWidget *parent)
{
    qDebug() << "[KateAgentPlugin] configPage() called with number=" << number << ", parent=" << parent;
    if (number != 0) {
        return nullptr;
    }
    auto *page = new AgentConfigPage(parent, this);
    qDebug() << "[KateAgentPlugin] configPage() returning page:" << page;
    return page;
}

int KateAgentPlugin::configPages() const
{
    return 1;
}

#include "kateagentplugin.moc"

