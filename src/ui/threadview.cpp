#include "threadview.h"
#include "llmprovider.h"
#include "syntaxhighlighter.h"
#include <KLocalizedString>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QJsonDocument>
#include <QTimer>
#include <QScrollBar>
#include <QResizeEvent>
#include <QClipboard>
#include <QApplication>

ThreadView::ThreadView(QWidget *parent)
    : QTextBrowser(parent)
{
    setReadOnly(true);
    setOpenExternalLinks(false);
    setAcceptRichText(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(0);
    setMinimumHeight(0);
    
    setWordWrapMode(QTextOption::WordWrap);
    document()->setTextWidth(QWIDGETSIZE_MAX);

    // Handle anchor clicks for "show older messages" link
    connect(this, &QTextBrowser::anchorClicked, this, &ThreadView::onAnchorClicked);
    
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
    docSheet += ".user-message { background-color: " + pal.color(QPalette::AlternateBase).lighter(110).name() + "; border-left: 4px solid #3b82f6; border-radius: 6px; padding: 10px 12px; margin: 12px 0; white-space: pre-wrap; word-break: break-word; overflow-wrap: break-word; }\n";
    docSheet += ".assistant-message { background-color: " + pal.color(QPalette::Base).lighter(105).name() + "; border-left: 4px solid #10b981; border-radius: 6px; padding: 10px 12px; margin: 12px 0; white-space: pre-wrap; word-break: break-word; overflow-wrap: break-word; }\n";
    docSheet += ".tool-call { background-color: " + pal.color(QPalette::Base).name() + "; border: 1px solid " + pal.color(QPalette::Midlight).name() + "; border-radius: 4px; padding: 6px 8px; margin: 6px 0; font-family: monospace; font-size: 0.9em; word-break: break-word; }\n";
    docSheet += ".tool-result { background-color: " + pal.color(QPalette::Base).name() + "; border: 1px solid " + pal.color(QPalette::Mid).name() + "; border-radius: 4px; padding: 6px 8px; margin: 6px 0; font-family: monospace; font-size: 0.9em; word-break: break-word; }\n";
    docSheet += ".tool-result.error { background-color: " + pal.color(QPalette::Base).lighter(108).name() + "; border: 1px solid " + pal.color(QPalette::Text).name() + "; }\n";
    docSheet += ".terminal-output { background-color: " + pal.color(QPalette::Base).name() + "; border: 1px solid " + pal.color(QPalette::Midlight).name() + "; border-radius: 4px; margin: 6px 0; overflow: hidden; }\n";
    docSheet += ".terminal-header { background-color: " + pal.color(QPalette::Mid).name() + "; padding: 6px 8px; font-family: monospace; font-size: 0.9em; border-bottom: 1px solid " + pal.color(QPalette::Midlight).name() + "; }\n";
    docSheet += ".terminal-body { padding: 8px; font-family: monospace; font-size: 0.85em; overflow-x: auto; white-space: pre-wrap; word-break: break-all; }\n";
    docSheet += "code { background-color: " + pal.color(QPalette::Base).lighter(105).name() + "; padding: 2px 4px; border-radius: 3px; font-family: monospace; font-size: 0.9em; }\n";
    docSheet += "pre { background-color: " + pal.color(QPalette::Base).lighter(108).name() + "; padding: 8px; border-radius: 4px; overflow-x: auto; position: relative; }\n";
    docSheet += "pre code { background-color: transparent; padding: 0; }\n";
    docSheet += ".copy-btn { position: absolute; top: 4px; right: 4px; background-color: rgba(0,0,0,0.3); color: white; border: none; padding: 4px 8px; border-radius: 4px; cursor: pointer; font-size: 0.8em; }\n";
    docSheet += ".copy-btn:hover { background-color: rgba(0,0,0,0.5); }\n";
    docSheet += ".copy-btn.copied { background-color: #10b981; }\n";
    docSheet += "h1, h2, h3, h4, h5, h6 { margin: 12px 0 8px 0; font-weight: bold; }\n";
    docSheet += "h1 { font-size: 1.3em; }\n";
    docSheet += "h2 { font-size: 1.15em; }\n";
    docSheet += "h3 { font-size: 1.05em; }\n";
    docSheet += "h4 { font-size: 1em; }\n";
    docSheet += "h5 { font-size: 0.95em; }\n";
    docSheet += "h6 { font-size: 0.9em; }\n";
    docSheet += "a { color: " + pal.color(QPalette::Highlight).name() + "; text-decoration: underline; }\n";
    docSheet += "hr { border: none; border-top: 1px solid " + pal.color(QPalette::Mid).name() + "; margin: 16px 0; opacity: 0.5; }\n";
    docSheet += ".model-label { color: " + pal.color(QPalette::Highlight).name() + "; font-size: 0.85em; font-weight: bold; display: block; margin-bottom: 4px; }\n";
    docSheet += ".model-name { font-weight: bold; }\n";
    docSheet += ".cursor { animation: blink 1s step-end infinite; }\n";
    docSheet += "@keyframes blink { 0%, 50% { opacity: 1; } 51%, 100% { opacity: 0; } }\n";
    
    // Syntax highlighting colors
    docSheet += ".hl-keyword { color: #8959a8; font-weight: bold; }\n";
    docSheet += ".hl-string { color: #718c00; }\n";
    docSheet += ".hl-comment { color: #8e908c; font-style: italic; }\n";
    docSheet += ".hl-number { color: #f5871f; }\n";
    docSheet += ".hl-preproc { color: #da70d6; }\n";
    
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

QString ThreadView::parseMarkdown(const QString &text)
{
    // Step 1: Process code blocks BEFORE HTML escaping (SyntaxHighlighter handles its own escaping)
    // Use placeholder markers to avoid double-escaping the code block HTML
    QString result = text;
    
    QRegularExpression codeBlockRegex("```(\\w*)\\n([\\s\\S]*?)```");
    QRegularExpressionMatch match;
    int offset = 0;
    
    // Collect code block replacements (process right-to-left to preserve positions)
    struct CodeBlockReplacement {
        qsizetype position;
        qsizetype length;
        QString placeholder;
        QString codeId;
    };
    QList<CodeBlockReplacement> blocks;
    
    while ((match = codeBlockRegex.match(result, offset)).hasMatch()) {
        QString language = match.captured(1);
        QString code = match.captured(2);
        
        QString id = QString("code_%1").arg(++m_codeBlockCounter);
        m_codeBlocks[id] = code;
        
        QString highlighted = SyntaxHighlighter::highlight(code, language);
        
        QString html = QString("<pre><a href='copy:%1' class='copy-btn'>Copy</a><code>%2</code></pre>")
                       .arg(id, highlighted);
        
        QString placeholder = QString("\x01CODEBLOCK_%1\x01").arg(m_codeBlockCounter);
        blocks.prepend({match.capturedStart(), match.capturedLength(), placeholder, id});
        m_codeBlockHtml[id] = html;
        offset = match.capturedEnd();
    }
    
    for (const auto &block : blocks) {
        result.replace(block.position, block.length, block.placeholder);
    }
    
    result = escapeHtml(result);
    
    // Headers before code block restoration to avoid matching # inside code blocks
    result.replace(QRegularExpression("^######\\s+(.+)$", QRegularExpression::MultilineOption), "<h6>\\1</h6>");
    result.replace(QRegularExpression("^#####\\s+(.+)$", QRegularExpression::MultilineOption), "<h5>\\1</h5>");
    result.replace(QRegularExpression("^####\\s+(.+)$", QRegularExpression::MultilineOption), "<h4>\\1</h4>");
    result.replace(QRegularExpression("^###\\s+(.+)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    result.replace(QRegularExpression("^##\\s+(.+)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    result.replace(QRegularExpression("^#\\s+(.+)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");
    
    for (const auto &block : blocks) {
        result.replace(escapeHtml(block.placeholder), m_codeBlockHtml[block.codeId]);
    }
    
    // Step 5: Process remaining inline markdown
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

static int s_messageId = 0;

void ThreadView::appendUserMessage(const QString &message, const QString &profile)
{
    QString header = i18n("User:");
    if (!profile.isEmpty()) {
        header += QString(" <span style='font-weight: normal; font-size: 0.85em; color: " + palette().color(QPalette::Highlight).name() + ";'>[%1]</span>").arg(escapeHtml(profile));
    }
    QString prefix = m_isFirstMessage ? "" : "<hr>";
    m_isFirstMessage = false;
    int msgId = ++s_messageId;
    appendHtml(prefix + QString("<div class='user-message' id='msg_%1' style='position:relative'><a href='delete:%1' style='position:absolute;right:8px;top:4px;color:#888;text-decoration:none;font-size:0.7em;'>✕</a><strong>%2</strong><br>%3</div>")
               .arg(QString::number(msgId), header, parseMarkdown(message)));
}

void ThreadView::appendAssistantMessage(const QString &message, const QString &thinking)
{
    QString content;
    if (!thinking.isEmpty()) {
        content += QString("<details><summary style='cursor:pointer;color:#888;font-size:0.85em;user-select:none;'>🤔 Thinking</summary><div style='background:rgba(0,0,0,0.05);padding:8px;border-radius:4px;margin:4px 0;font-size:0.9em;color:#888;font-style:italic;white-space:pre-wrap;'>%1</div></details><br>")
                   .arg(escapeHtml(thinking));
    }
    
    int msgId = ++s_messageId;
    content += QString("<div class='assistant-message' id='msg_%1' style='position:relative'><a href='delete:%1' style='position:absolute;right:8px;top:4px;color:#888;text-decoration:none;font-size:0.7em;'>✕</a>%2</div>").arg(QString::number(msgId), parseMarkdown(message));
    QString prefix = m_isFirstMessage ? "" : "<hr>";
    m_isFirstMessage = false;
    appendHtml(prefix + content);
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
        m_streamingStartPosition = textCursor().position();
    }
    
    textCursor().insertText(chunk);
    
    insertHtml("<span class='cursor'>|</span>");
    scrollToBottom();
    
    if (m_streamingContent.length() == chunk.length()) {
        m_cursorBlinkCount = 0;
        m_cursorTimer->start();
    }
}

void ThreadView::resetStreaming()
{
    m_streamingContent.clear();
    m_streamingThinking.clear();
}

void ThreadView::endStreaming()
{
    m_cursorTimer->stop();
    
    QString streamingContent = m_streamingContent;
    m_streamingContent.clear();
    
    if (!streamingContent.isEmpty() && m_streamingStartPosition > 0) {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        cursor.setPosition(m_streamingStartPosition);
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        
        QString markdownHtml = parseMarkdown(streamingContent);
        insertHtml(markdownHtml);
        insertHtml("</div><br>");
    } else if (!streamingContent.isEmpty()) {
        insertHtml(parseMarkdown(streamingContent));
        insertHtml("<br>");
    }
    
    scrollToBottom();
    m_streamingStartPosition = 0;
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
    m_isFirstMessage = true;
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
    m_allMessages = messages;
    clear();
    
    const bool truncated = m_allMessages.size() > MAX_VISIBLE_MESSAGES;
    const int startIndex = truncated ? m_allMessages.size() - MAX_VISIBLE_MESSAGES : 0;
    
    if (truncated) {
        const int older = m_allMessages.size() - MAX_VISIBLE_MESSAGES;
        appendHtml(QString("<p style='font-style: italic; color: #666;'><a href='show-all'>%1</a></p>")
                   .arg(i18np("Show %1 older message...", "Show %1 older messages...", older)));
    }
    
    for (int i = startIndex; i < m_allMessages.size(); ++i) {
        const auto &msg = m_allMessages[i];
        if (msg.role == "user") {
            appendUserMessage(msg.content, msg.profile);
        } else if (msg.role == "assistant") {
            appendAssistantMessage(msg.content, msg.thinking);
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

void ThreadView::setSource(const QUrl &name)
{
    QString urlStr = name.toString();
    if (urlStr == "show-all" || urlStr.startsWith("copy:")) {
        onAnchorClicked(name);
        return;
    }
    QTextBrowser::setSource(name);
}

void ThreadView::onAnchorClicked(const QUrl &url)
{
    QString urlStr = url.toString();
    if (urlStr == "show-all") {
        loadMessages(m_allMessages);
    } else if (urlStr.startsWith("copy:")) {
        QString id = urlStr.mid(5);
        if (m_codeBlocks.contains(id)) {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(m_codeBlocks[id]);
        }
    } else if (urlStr.startsWith("delete:")) {
        QString id = urlStr.mid(7);
        emit deleteMessageRequested(id.toInt());
    }
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
