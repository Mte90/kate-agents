#include "thinkingpopup.h"

ThinkingPopup::ThinkingPopup(AgentLoop *agent, QWidget *parent)
        : QDialog(parent)
        , m_edit(new QPlainTextEdit(this))
    {
        setWindowTitle(tr("Model Thinking"));
        setAttribute(Qt::WA_DeleteOnClose);
        setModal(false);
        resize(600, 360);

        m_edit->setReadOnly(true);
        auto *layout = new QVBoxLayout(this);
        layout->addWidget(m_edit);

        if (agent) {
//             connect(agent, &AgentLoop::thinkingChunk, this, &ThinkingPopup::appendThinking);
            connect(agent, &AgentLoop::turnCompleted, this, [this]() {
                appendThinking(QStringLiteral("\n--- Done ---\n"));
            });
        }
    }

void ThinkingPopup::appendThinking(const QString &chunk)
    {
        if (chunk.isEmpty()) return;
        QScrollBar *bar = m_edit->verticalScrollBar();
        const bool atBottom = !bar || bar->value() >= bar->maximum() - 4;

        m_edit->moveCursor(QTextCursor::End);
        m_edit->insertPlainText(chunk);

        if (bar && atBottom) {
            bar->setValue(bar->maximum());
        }
    }
