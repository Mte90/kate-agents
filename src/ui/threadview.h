#ifndef THREADVIEW_H
#define THREADVIEW_H

#include <QTextBrowser>
#include <QWidget>
#include <QJsonObject>
#include <QList>

class LLMMessage;

class ThreadView : public QTextBrowser
{
    Q_OBJECT

public:
    explicit ThreadView(QWidget *parent = nullptr);
    ~ThreadView() override;

    void appendUserMessage(const QString &message, const QString &profile = QString());
    void appendAssistantMessage(const QString &message);
    void appendToolCall(const QString &toolName, const QJsonObject &args);
    void appendToolResult(const QString &toolName, const QJsonObject &result);
    void showStreamingChunk(const QString &chunk);
    void endStreaming();
    void clear();
    void scrollToBottom();
    void appendHtml(const QString &html);
    void loadMessages(const QList<LLMMessage> &messages);
    void renderThread(const QList<LLMMessage> &messages);
    void setStreamingModel(const QString &model);
    QSize sizeHint() const override { return QSize(0, 0); }
    QSize minimumSizeHint() const override { return QSize(0, 0); }

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
signals:
    void linkClicked(const QString &url);
private slots:
    void toggleCursor();

private:
    bool m_cursorVisible = true;
    QString parseMarkdown(const QString &text) const;
    QString escapeHtml(const QString &text) const;
    
    QString m_streamingContent;
    QString m_streamingModel;
    QTimer *m_cursorTimer = nullptr;
    int m_cursorBlinkCount = 0;
};

#endif
