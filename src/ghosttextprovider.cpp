#include "ghosttextprovider.h"

void GhostTextProvider::setSuggestion(const QString &text, int line, int column)
{
    if (m_ghostText == text && m_suggestionLine == line && m_suggestionColumn == column) {
        return;
    }

    m_ghostText = text;
    m_suggestionLine = line;
    m_suggestionColumn = column;
    m_hasSuggestion = !text.isEmpty();
}

void GhostTextProvider::clearSuggestion()
{
    m_ghostText.clear();
    m_hasSuggestion = false;
}

KTextEditor::Cursor GhostTextProvider::suggestionPosition() const
{
    return {m_suggestionLine, m_suggestionColumn};
}

QList<int> GhostTextProvider::inlineNotes(int line) const
{
    if (!m_hasSuggestion || line != m_suggestionLine) {
        return {};
    }

    return {m_suggestionColumn};
}

QSize GhostTextProvider::inlineNoteSize(const KTextEditor::InlineNote &note) const
{
    if (!m_hasSuggestion) {
        return {};
    }

    QFont font = note.font();
    QFontMetrics fm(font);
    int width = fm.horizontalAdvance(m_ghostText);
    int height = fm.height();
    return {width, height};
}

void GhostTextProvider::paintInlineNote(const KTextEditor::InlineNote &note, QPainter &painter,
                                        Qt::LayoutDirection direction) const
{
    if (!m_hasSuggestion) {
        return;
    }

    QFont font = note.font();
    painter.setFont(font);

    QColor ghostColor = Qt::gray;
    ghostColor.setAlpha(180);
    painter.setPen(ghostColor);
    painter.drawText(0, 0, m_ghostText);
}

void GhostTextProvider::inlineNoteActivated(const KTextEditor::InlineNote &note,
                                            Qt::MouseButtons buttons,
                                            const QPoint &globalPos)
{
    Q_UNUSED(note)
    Q_UNUSED(globalPos)

    if (buttons & Qt::LeftButton && m_hasSuggestion) {
        emit suggestionAccepted(m_ghostText);
    }
}

void GhostTextProvider::inlineNoteFocusInEvent(const KTextEditor::InlineNote &note,
                                               const QPoint &globalPos)
{
    Q_UNUSED(note)
    Q_UNUSED(globalPos)
}

void GhostTextProvider::inlineNoteFocusOutEvent(const KTextEditor::InlineNote &note)
{
    Q_UNUSED(note)
}

void GhostTextProvider::inlineNoteMouseMoveEvent(const KTextEditor::InlineNote &note,
                                                 const QPoint &globalPos)
{
    Q_UNUSED(note)
    Q_UNUSED(globalPos)
}
