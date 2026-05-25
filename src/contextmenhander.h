#ifndef CONTEXTMENHANDLER_H
#define CONTEXTMENHANDLER_H

#include <QObject>
#include <QMap>
#include <QMenu>
#include <KTextEditor/View>
#include <KTextEditor/Document>

class AgentLoop;

class ContextMenuHandler : public QObject
{
    Q_OBJECT

public:
    explicit ContextMenuHandler(AgentLoop *agentLoop, QObject *parent = nullptr);
    
    void installContextMenu(KTextEditor::View *view);
    void uninstallContextMenu(KTextEditor::View *view);

private slots:
    void onAskAgentAboutThis();

private:
    AgentLoop *m_agentLoop;
    QMap<KTextEditor::View*, QAction*> m_viewActions;
};
#endif // CONTEXTMENHANDLER_H
