#ifndef THINKINGPOPUP_H
#define THINKINGPOPUP_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QPointer>
#include "../agentloop.h"

class ThinkingPopup : public QDialog
{
    Q_OBJECT

public:
    explicit ThinkingPopup(AgentLoop *agent, QWidget *parent = nullptr);

public slots:
    void appendThinking(const QString &chunk);

private:
    QPlainTextEdit *m_edit;
};

#endif // THINKINGPOPUP_H
