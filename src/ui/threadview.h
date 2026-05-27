#ifndef THREADVIEW_H
#define THREADVIEW_H

#include <QTextBrowser>
#include <QUrl>
#include <QWidget>
#include <QJsonObject>
#include <QList>
#include <QMap>

#include "syntaxhighlighter.h"

class LLMMessage;

class ThreadView : public QTextBrowser
{
    Q_OBJECT

public:
    explicit ThreadView(QWidget *parent = nullptr);
    ~ThreadView() override;

    void appendUserMessage(const QString &message, const QString &profile = QString());
    void appendAssistantMessage(const QString &message, const QString &thinking = QString());
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
    void setSource(const QUrl &name);
    
signals:
    void linkClicked(const QString &url);
private slots:
    void toggleCursor();
    void onAnchorClicked(const QUrl &url);

private:
    static constexpr int MAX_VISIBLE_MESSAGES = 100;
    
    bool m_cursorVisible = true;
    QList<LLMMessage> m_allMessages;
    QString parseMarkdown(const QString &text);
    QString escapeHtml(const QString &text) const;
    
    QString m_streamingContent;
    QString m_streamingThinking;
    QString m_streamingModel;
    int m_streamingStartPosition = 0;
    QTimer *m_cursorTimer = nullptr;
    int m_cursorBlinkCount = 0;
    
    // Store code block content for copy functionality
    QMap<QString, QString> m_codeBlocks;
    int m_codeBlockCounter = 0;
};

#endif
