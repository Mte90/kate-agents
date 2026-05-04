#include "threadview.h"
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QJsonDocument>
#include <QDebug>
#include <QTimer>

ThreadView::ThreadView(QWidget *parent)
    : QTextBrowser(parent)
{
    setReadOnly(true);
    setOpenExternalLinks(false);
    setAcceptRichText(true);
    
    // Initialize cursor timer for streaming effect
    m_cursorTimer = new QTimer(this);
    m_cursorTimer->setInterval(500);
    connect(m_cursorTimer, &QTimer::timeout, this, &ThreadView::toggleCursor);
    
    document()->setDefaultStyleSheet(R"(
        .user-message { 
            background: #2d2d2d; 
            padding: 10px; 
            border-radius: 8px; 
            margin: 5px 0;
            color: #ffffff;
            text-align: right;
        }
        .assistant-message { 
            background: #1e1e1e; 
            padding: 10px; 
            border-radius: 8px; 
            margin: 5px 0;
            color: #e0e0e0;
            text-align: left;
        }
        .assistant-message.streaming .cursor {
            animation: blink 1s step-end infinite;
        }
        @keyframes blink {
            0%, 100% { opacity: 1; }
            50% { opacity: 0; }
        }
        .tool-call { 
            background: #1a237e; 
            color: #90caf9;
            padding: 8px; 
            border-radius: 4px; 
            margin: 5px 0;
            font-family: monospace;
            font-size: 12px;
        }
        .tool-result { 
            background: #1b5e20; 
            color: #a5d6a7;
            padding: 8px; 
            border-radius: 4px; 
            margin: 5px 0;
            font-family: monospace;
            font-size: 12px;
        }
        .tool-result.error { 
            background: #b71c1c; 
            color: #ef9a9a;
        }
        .terminal-output {
            background: #0d0d0d;
            color: #00ff00;
            padding: 12px;
            border-radius: 6px;
            margin: 8px 0;
            font-family: 'Monospace', 'Courier New', monospace;
            font-size: 12px;
            border-left: 3px solid #00ff00;
            white-space: pre-wrap;
            word-wrap: break-word;
        }
        .terminal-header {
            background: #1a1a1a;
            color: #00ff00;
            padding: 6px 10px;
            border-radius: 6px 6px 0 0;
            font-weight: bold;
            font-family: monospace;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .terminal-body {
            background: #0d0d0d;
            color: #00ff00;
            padding: 10px;
            margin: 0;
            font-family: monospace;
            font-size: 12px;
            white-space: pre-wrap;
            word-wrap: break-word;
            overflow-x: auto;
        }
        code {
            background: #424242;
            padding: 2px 4px;
            border-radius: 3px;
        }
        pre {
            background: #212121;
            padding: 10px;
            border-radius: 4px;
            overflow-x: auto;
        }
    )");
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
    ensureCursorVisible();
}

void ThreadView::appendUserMessage(const QString &message)
{
    appendHtml(QString("<div class='user-message'><strong>👤 User:</strong><br>%1</div>")
               .arg(parseMarkdown(message)));
}

void ThreadView::appendAssistantMessage(const QString &message)
{
    appendHtml(QString("<div class='assistant-message'><strong>🤖 Assistant:</strong><br>%1</div>")
               .arg(parseMarkdown(message)));
}

void ThreadView::appendToolCall(const QString &toolName, const QJsonObject &args)
{
    QString argsStr = QJsonDocument(args).toJson(QJsonDocument::Compact);
    appendHtml(QString("<div class='tool-call'>🔧 <strong>%1</strong><br>%2</div>")
               .arg(toolName)
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
        
        QString exitCodeInfo = exitCode != 0 ? QString(" (exit code: %1)").arg(exitCode) : "";
        if (timedOut) {
            exitCodeInfo += " [TIMED OUT]";
        }
        
        QString html = QString(R"(
            <div class='terminal-output'>
                <div class='terminal-header'>▶ %1%2</div>
                <div class='terminal-body'>%3</div>
            </div>
        )")
        .arg(escapeHtml(command))
        .arg(exitCodeInfo)
        .arg(escapeHtml(output));
        
        appendHtml(html);
        return;
    }
    
    // Default rendering for other tools (compact JSON)
    QString resultStr = QJsonDocument(result).toJson(QJsonDocument::Compact);
    
    if (resultStr.length() > 500) {
        resultStr = resultStr.left(500) + "...";
    }
    
    appendHtml(QString("<div class='tool-result %1'>✅ %2<br>%3</div>")
               .arg(success ? "" : "error")
               .arg(toolName)
               .arg(escapeHtml(resultStr)));
}

void ThreadView::showStreamingChunk(const QString &chunk)
{
    if (m_streamingContent.isEmpty()) {
        m_cursorBlinkCount = 0;
        m_cursorTimer->start();
    }
    
    m_streamingContent += chunk;
    
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
    
    insertHtml(parseMarkdown(chunk));
    insertHtml("<span class='cursor'>|</span>");
    ensureCursorVisible();
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
