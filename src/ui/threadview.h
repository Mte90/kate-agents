#ifndef THREADVIEW_H
#define THREADVIEW_H

#include <QTextBrowser>
#include <QWidget>
#include <QJsonObject>

class ThreadView : public QTextBrowser
{
    Q_OBJECT

public:
    explicit ThreadView(QWidget *parent = nullptr);
    ~ThreadView() override;

    void appendUserMessage(const QString &message);
    void appendAssistantMessage(const QString &message);
    void appendToolCall(const QString &toolName, const QJsonObject &args);
    void appendToolResult(const QString &toolName, const QJsonObject &result);
    void showStreamingChunk(const QString &chunk);
    void clear();
    void appendHtml(const QString &html);

signals:
    void linkClicked(const QString &url);

private slots:
    void toggleCursor();

private:
    bool m_cursorVisible = true;
    QString parseMarkdown(const QString &text) const;
    QString escapeHtml(const QString &text) const;
    
    QString m_streamingContent;
    QTimer *m_cursorTimer = nullptr;
    int m_cursorBlinkCount = 0;
};

#endif
