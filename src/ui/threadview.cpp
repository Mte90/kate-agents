#include "threadview.h"
#include "llmprovider.h"
#include <KLocalizedString>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QJsonDocument>
#include <QTimer>
#include <QScrollBar>
#include <QResizeEvent>

ThreadView::ThreadView(QWidget *parent)
    : QTextBrowser(parent)
{
    setReadOnly(true);
    setOpenExternalLinks(false);
    setAcceptRichText(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(0);
    setMinimumHeight(0);
    
    // Ensure proper word wrapping and text reflow
    setWordWrapMode(QTextOption::WordWrap);
    document()->setTextWidth(400);

    // Initialize cursor timer for streaming effect
    m_cursorTimer = new QTimer(this);
    m_cursorTimer->setInterval(500);
    connect(m_cursorTimer, &QTimer::timeout, this, &ThreadView::toggleCursor);
    
    // Widget-level stylesheet (background color only)
    QPalette pal = palette();
    QString widgetSheet;
    widgetSheet += "QTextBrowser { background-color: " + pal.color(QPalette::Base).name() + "; color: " + pal.color(QPalette::Text).name() + "; }";
    setStyleSheet(widgetSheet);
    
    // Document-level stylesheet for HTML content
    QString docSheet;
    docSheet += ".user-message { background-color: " + pal.color(QPalette::AlternateBase).lighter(110).name() + "; border-left: 3px solid " + pal.color(QPalette::Highlight).name() + "; border-radius: 6px; padding: 10px 12px; margin: 12px 0; white-space: pre-wrap; word-break: break-word; overflow-wrap: break-word; }\n";
    docSheet += ".assistant-message { background-color: " + pal.color(QPalette::Base).lighter(105).name() + "; border-left: 3px solid " + pal.color(QPalette::Mid).name() + "; border-radius: 6px; padding: 10px 12px; margin: 12px 0; white-space: pre-wrap; word-break: break-word; overflow-wrap: break-word; }\n";
    docSheet += ".tool-call { background-color: " + pal.color(QPalette::Base).name() + "; border: 1px solid " + pal.color(QPalette::Midlight).name() + "; border-radius: 4px; padding: 6px 8px; margin: 6px 0; font-family: monospace; font-size: 0.9em; word-break: break-word; }\n";
    docSheet += ".tool-result { background-color: " + pal.color(QPalette::Base).name() + "; border: 1px solid " + pal.color(QPalette::Mid).name() + "; border-radius: 4px; padding: 6px 8px; margin: 6px 0; font-family: monospace; font-size: 0.9em; word-break: break-word; }\n";
    docSheet += ".tool-result.error { background-color: " + pal.color(QPalette::Base).lighter(108).name() + "; border: 1px solid " + pal.color(QPalette::Text).name() + "; }\n";
    docSheet += ".terminal-output { background-color: " + pal.color(QPalette::Base).name() + "; border: 1px solid " + pal.color(QPalette::Midlight).name() + "; border-radius: 4px; margin: 6px 0; overflow: hidden; }\n";
    docSheet += ".terminal-header { background-color: " + pal.color(QPalette::Mid).name() + "; padding: 6px 8px; font-family: monospace; font-size: 0.9em; border-bottom: 1px solid " + pal.color(QPalette::Midlight).name() + "; }\n";
    docSheet += ".terminal-body { padding: 8px; font-family: monospace; font-size: 0.85em; overflow-x: auto; white-space: pre-wrap; word-break: break-all; }\n";
    docSheet += "code { background-color: " + pal.color(QPalette::Base).lighter(105).name() + "; padding: 2px 4px; border-radius: 3px; font-family: monospace; font-size: 0.9em; }\n";
    docSheet += "pre { background-color: " + pal.color(QPalette::Base).lighter(108).name() + "; padding: 8px; border-radius: 4px; overflow-x: auto; }\n";
    docSheet += "pre code { background-color: transparent; padding: 0; }\n";
    docSheet += "a { color: " + pal.color(QPalette::Highlight).name() + "; text-decoration: underline; }\n";
    docSheet += "hr { border: none; border-top: 1px solid " + pal.color(QPalette::Midlight).name() + "; margin: 20px 0 24px 0; }\n";
    docSheet += ".model-label { color: " + pal.color(QPalette::Highlight).name() + "; font-size: 0.85em; font-weight: bold; display: block; margin-bottom: 4px; }\n";
    docSheet += ".model-name { font-weight: bold; }\n";
    docSheet += ".cursor { animation: blink 1s step-end infinite; }\n";
    docSheet += "@keyframes blink { 0%, 50% { opacity: 1; } 51%, 100% { opacity: 0; } }\n";
    document()->setDefaultStyleSheet(docSheet);
}

ThreadView::~ThreadView() = default;

QString ThreadView::escapeHtml(const QString &text) const
{
    QString result = text;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    result.replace("\"", "&quot;");
    result.replace("'", "&#39;");
    return result;
}

QString ThreadView::parseMarkdown(const QString &text) const
{
    QString result = escapeHtml(text);
    
    result.replace(QRegularExpression("```(\\w*)\\n([\\s\\S]*?)```"), 
                   "<pre><code>\\2</code></pre>");
    
    result.replace(QRegularExpression("`([^`]+)`"), 
                   "<code>\\1</code>");
    
    result.replace(QRegularExpression("\\*\\*([^*]+)\\*\\*"), 
                   "<strong>\\1</strong>");
    
    result.replace(QRegularExpression("\\*([^*]+)\\*"), 
                   "<em>\\1</em>");
    
    result.replace(QRegularExpression("\\[([^\\]]+)\\]\\(([^)]+)\\)"), 
                   "<a href=\"\\2\">\\1</a>");
    
    result.replace("\n", "<br>");
    
    return result;
}

void ThreadView::appendHtml(const QString &html)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
    insertHtml(html);
    scrollToBottom();
}

void ThreadView::appendUserMessage(const QString &message, const QString &profile)
{
    appendHtml("<hr>");
    QString header = i18n("User:");
    if (!profile.isEmpty()) {
        header += QString(" <span style='font-weight: normal; font-size: 0.85em; color: " + palette().color(QPalette::Mid).name() + ";'>[%1]</span>").arg(escapeHtml(profile));
    }
    appendHtml(QString("<div class='user-message'><strong>%1</strong><br>%2</div>")
               .arg(header, parseMarkdown(message)));
}

void ThreadView::appendAssistantMessage(const QString &message)
{
    appendHtml("<hr>");
    appendHtml(QString("<div class='assistant-message'>%1</div>")
               .arg(parseMarkdown(message)));
}

void ThreadView::appendToolCall(const QString &toolName, const QJsonObject &args)
{
    QString argsStr = QJsonDocument(args).toJson(QJsonDocument::Compact);
    appendHtml(QString("<div class='tool-call'><strong>%1</strong><br>%2</div>")
               .arg(i18n("Tool:"), toolName)
               .arg(escapeHtml(argsStr)));
}

void ThreadView::appendToolResult(const QString &toolName, const QJsonObject &result)
{
    bool success = result["success"].toBool(true);
    
    // Special rendering for terminal tool
    if (toolName == "terminal") {
        QString command = result["command"].toString();
        QString output = result["output"].toString();
        int exitCode = result["exit_code"].toInt(0);
        bool timedOut = result["timed_out"].toBool(false);
        
        // Truncate output to 100 lines
        QStringList lines = output.split('\n');
        if (lines.length() > 100) {
            output = lines.mid(0, 100).join('\n') + "\n... (output truncated, 100 lines max)";
        }
        
        QString exitCodeInfo = exitCode != 0 ? QString(" (%1: %2)").arg(i18n("exit code"), QString::number(exitCode)) : "";
        if (timedOut) {
            exitCodeInfo += QString(" [%1]").arg(i18n("TIMED OUT"));
        }
        
        QString html = QString(R"(
            <div class='terminal-output'>
                <div class='terminal-header'>%1 %2%3</div>
                <div class='terminal-body'>%4</div>
            </div>
        )")
        .arg(i18n("Command:"), escapeHtml(command), exitCodeInfo)
        .arg(escapeHtml(output));
        
        appendHtml(html);
        return;
    }
    
    // Default rendering for other tools (compact JSON)
    QString resultStr = QJsonDocument(result).toJson(QJsonDocument::Compact);
    
    if (resultStr.length() > 500) {
        resultStr = resultStr.left(500) + "...";
    }
    
    QString statusIcon = success ? i18n("Success:") : i18n("Failed:");
    appendHtml(QString("<div class='tool-result %1'><strong>%2</strong> %3<br>%4</div>")
               .arg(success ? "" : "error")
               .arg(toolName)
               .arg(statusIcon)
               .arg(escapeHtml(resultStr)));
}

void ThreadView::showStreamingChunk(const QString &chunk)
{
    bool isFirst = m_streamingContent.isEmpty();
    m_streamingContent += chunk;
    
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    
    // Remove the previous cursor character before inserting new content
    if (cursor.position() > 0 && !isFirst) {
        QTextCursor removeCursor = cursor;
        removeCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
        if (removeCursor.selectedText() == QStringLiteral("|")) {
            removeCursor.removeSelectedText();
        }
    }
    
    setTextCursor(cursor);
    if (isFirst) {
        insertHtml("<hr>");
        if (!m_streamingModel.isEmpty()) {
            insertHtml(QString("<span style='font-weight: bold; font-size: 0.85em;'>%1</span><br>").arg(escapeHtml(m_streamingModel)));
        }
        insertHtml("<div style='white-space: pre-wrap; word-break: break-word;'>");
    }
    
    // Use insertText to preserve all whitespace exactly
    // insertHtml collapses spaces across chunk boundaries in QTextDocument
    cursor.insertText(chunk);
    
    insertHtml("<span class='cursor'>|</span>");
    scrollToBottom();
    
    if (m_streamingContent.length() == chunk.length()) {
        m_cursorBlinkCount = 0;
        m_cursorTimer->start();
    }
}

void ThreadView::endStreaming()
{
    m_cursorTimer->stop();
    
    // Remove the trailing cursor character from the rendered document
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    if (cursor.position() > 0) {
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    
    cursor.movePosition(QTextCursor::End);
    insertHtml("</div><br>");
    scrollToBottom();
    
    m_streamingContent.clear();
}

void ThreadView::setStreamingModel(const QString &model)
{
    m_streamingModel = model;
}

void ThreadView::scrollToBottom()
{
    QScrollBar *scrollBar = verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ThreadView::clear()
{
    QTextBrowser::clear();
    m_streamingContent.clear();
    m_cursorTimer->stop();
    m_cursorVisible = true;
    m_cursorBlinkCount = 0;
}

void ThreadView::toggleCursor()
{
    m_cursorVisible = !m_cursorVisible;
    m_cursorBlinkCount++;
    
    if (m_cursorBlinkCount >= 3) {
        m_cursorTimer->stop();
    }
}

void ThreadView::loadMessages(const QList<LLMMessage> &messages)
{
    clear();
    
    for (const auto &msg : messages) {
        if (msg.role == "user") {
            appendUserMessage(msg.content, msg.profile);
        } else if (msg.role == "assistant") {
            appendAssistantMessage(msg.content);
        }
    }
    
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void ThreadView::renderThread(const QList<LLMMessage> &messages)
{
    loadMessages(messages);
}

void ThreadView::showEvent(QShowEvent *event)
{
    QTextBrowser::showEvent(event);
    document()->setTextWidth(viewport()->width());
}

void ThreadView::resizeEvent(QResizeEvent *event)
{
    QTextBrowser::resizeEvent(event);
    document()->setTextWidth(viewport()->width());
    scrollToBottom();
}
