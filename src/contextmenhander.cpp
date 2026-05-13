#include "contextmenhander.h"
#include "agentloop.h"
#include <QAction>
#include <QDebug>

ContextMenuHandler::ContextMenuHandler(AgentLoop *agentLoop, QObject *parent)
    : QObject(parent)
    , m_agentLoop(agentLoop)
    , m_contextMenu(nullptr)
    , m_askAgentAction(nullptr)
{
}

void ContextMenuHandler::installContextMenu(KTextEditor::View *view)
{
    if (!view) {
        qWarning() << "ContextMenuHandler: view is null";
        return;
    }

    m_contextMenu = view->contextMenu();
    if (!m_contextMenu) {
        qWarning() << "ContextMenuHandler: context menu is null";
        return;
    }

    m_askAgentAction = new QAction("Ask agent about this", this);
    connect(m_askAgentAction, &QAction::triggered, this, &ContextMenuHandler::onAskAgentAboutThis);

    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_askAgentAction);
}

void ContextMenuHandler::uninstallContextMenu(KTextEditor::View *view)
{
    if (!view || !m_contextMenu || !m_askAgentAction) {
        return;
    }

    m_contextMenu->removeAction(m_askAgentAction);
    m_askAgentAction->deleteLater();
    m_askAgentAction = nullptr;
    m_contextMenu = nullptr;
}

void ContextMenuHandler::onAskAgentAboutThis()
{
    if (!m_agentLoop) {
        qWarning() << "ContextMenuHandler: AgentLoop is null";
        return;
    }

    auto view = qobject_cast<KTextEditor::View*>(sender()->parent());
    if (!view) {
        qWarning() << "ContextMenuHandler: Could not get view from action";
        return;
    }

    QString selectedText = view->selectionText();
    if (selectedText.isEmpty()) {
        qWarning() << "ContextMenuHandler: No text selected";
        return;
    }

    int cursorLine = view->cursorPosition().line();
    int cursorCol = view->cursorPosition().column();
    
    QString context;
    auto cursorPos = view->cursorPosition();
    
    // Get surrounding context using document text range
    int start = qMax(0, cursorPos.line() - 5);
    int end = cursorPos.line() + 5;
    
    // Use document->text() to get lines - it takes (startLine, startCol, endLine, endCol)
    for (int i = start; i <= end; ++i) {
        QString line;
        // Safely get line text - avoid broken APIs
        if (i >= 0) {
            // Get line as plain text using KTextEditor::Range
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

    m_agentLoop->addUserMessage(QString(), message);
}
