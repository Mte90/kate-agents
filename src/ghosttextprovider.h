#ifndef GHOSTTEXTPROVIDER_H
#define GHOSTTEXTPROVIDER_H

#include <KTextEditor/InlineNoteProvider>
#include <KTextEditor/InlineNote>
#include <QObject>
#include <QString>
#include <QSize>
#include <QPainter>
#include <QFontMetrics>

/**
 * @brief GhostTextProvider implements KTextEditor::InlineNoteProvider for AI suggestions
 * 
 * This class displays inline ghost text suggestions from the AI agent at the cursor position.
 * Users can accept the suggestion by clicking on it or pressing Tab.
 * 
 * Based on the git blame plugin and LSP client inlay hints implementation.
 */
class GhostTextProvider : public KTextEditor::InlineNoteProvider
{
    Q_OBJECT

public:
    explicit GhostTextProvider(QObject *parent = nullptr) : KTextEditor::InlineNoteProvider() { Q_UNUSED(parent); }
    ~GhostTextProvider() override = default;

    /**
     * @brief Set the ghost text to display
     * @param text The suggestion text, empty to hide
     * @param line Line number where to display (0-based)
     * @param column Column position where to display (0-based)
     */
    void setSuggestion(const QString &text, int line, int column);

    /**
     * @brief Clear the current suggestion
     */
    void clearSuggestion();

    /**
     * @brief Check if there's an active suggestion
     */
    bool hasSuggestion() const { return !m_ghostText.isEmpty(); }

    /**
     * @brief Get current suggestion text
     */
    QString suggestion() const { return m_ghostText; }

    /**
     * @brief Get current suggestion position
     */
    KTextEditor::Cursor suggestionPosition() const;

    // KTextEditor::InlineNoteProvider interface
    QList<int> inlineNotes(int line) const override;
    QSize inlineNoteSize(const KTextEditor::InlineNote &note) const override;
    void paintInlineNote(const KTextEditor::InlineNote &note, QPainter &painter,
                         Qt::LayoutDirection direction) const override;

    void inlineNoteActivated(const KTextEditor::InlineNote &note, Qt::MouseButtons buttons,
                             const QPoint &globalPos) override;
    void inlineNoteFocusInEvent(const KTextEditor::InlineNote &note,
                                const QPoint &globalPos) override;
    void inlineNoteFocusOutEvent(const KTextEditor::InlineNote &note) override;
    void inlineNoteMouseMoveEvent(const KTextEditor::InlineNote &note,
                                  const QPoint &globalPos) override;

signals:
    /**
     * @brief Emitted when user accepts the suggestion (click or Tab)
     */
    void suggestionAccepted(const QString &text);

    /**
     * @brief Emitted when user rejects the suggestion (Escape or click elsewhere)
     */
    void suggestionRejected();

private:
    QString m_ghostText;
    int m_suggestionLine = 0;
    int m_suggestionColumn = 0;
    bool m_hasSuggestion = false;
};

#endif // GHOSTTEXTPROVIDER_H
