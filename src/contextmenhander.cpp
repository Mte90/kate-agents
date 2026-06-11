#include "contextmenhander.h"
#include "agentloop.h"
#include <QAction>
#include <QDebug>

ContextMenuHandler::ContextMenuHandler(AgentLoop *agentLoop, QObject *parent)
    : QObject(parent)
    , m_agentLoop(agentLoop)
{
}

void ContextMenuHandler::installContextMenu(KTextEditor::View *view)
{
    if (!view) {
        // ContextMenuHandler: view is null
        return;
    }

    if (m_viewActions.contains(view)) {
        return;
    }

    QMenu *menu = view->contextMenu();
    if (!menu) {
        // ContextMenuHandler: context menu is null
        return;
    }

    QAction *action = new QAction("Ask agent about this", this);
    action->setParent(view);
    connect(action, &QAction::triggered, this, &ContextMenuHandler::onAskAgentAboutThis);

    menu->addSeparator();
    menu->addAction(action);
    m_viewActions.insert(view, action);

    connect(view, &QObject::destroyed, this, [this, view]() {
        m_viewActions.remove(view);
    });
}

void ContextMenuHandler::uninstallContextMenu(KTextEditor::View *view)
{
    if (!view) {
        return;
    }

    QAction *action = m_viewActions.take(view);
    if (action) {
        action->deleteLater();
    }
}

void ContextMenuHandler::onAskAgentAboutThis()
{
    if (!m_agentLoop) {
        // ContextMenuHandler: AgentLoop is null
        return;
    }

    auto *action = qobject_cast<QAction*>(sender());
    if (!action) {
        return;
    }

    auto *view = qobject_cast<KTextEditor::View*>(action->parent());
    if (!view) {
        // ContextMenuHandler: Could not get view from action
        return;
    }

    QString selectedText = view->selectionText();
    if (selectedText.isEmpty()) {
        // ContextMenuHandler: No text selected
        return;
    }

    // Check if document is text-based (not binary)
    KTextEditor::Document *doc = view->document();
    QString mimeType = doc->mimeType();
    if (mimeType.startsWith("image/") || mimeType.startsWith("application/octet-stream")) {
        return;
    }

    int cursorLine = view->cursorPosition().line();
    int cursorCol = view->cursorPosition().column();
    
    QString context;
    auto cursorPos = view->cursorPosition();
    
    int start = qMax(0, cursorPos.line() - 5);
    int end = cursorPos.line() + 5;
    
    for (int i = start; i <= end; ++i) {
        QString line;
        if (i >= 0) {
            KTextEditor::Range range(i, 0, i, 1);
            line = view->document()->text(range).trimmed();
        }
        if (line.isEmpty()) {
            line = "...";
        }
        if (i == cursorPos.line()) {
            context += ">>> ";
        }
        context += line + "\n";
    }

    QString message = QString("Selected text:\n```\n%1\n```\n\nContext around cursor (line %2, col %3):\n```\n%4\n```")
        .arg(selectedText)
        .arg(cursorLine)
        .arg(cursorCol)
        .arg(context);

    QString threadId = m_agentLoop->currentThreadId();
    if (threadId.isEmpty()) {
        // ContextMenuHandler: No active thread, cannot send message
        return;
    }
    m_agentLoop->addUserMessage(threadId, message);
}
