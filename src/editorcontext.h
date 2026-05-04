#ifndef EDITORCONTEXT_H
#define EDITORCONTEXT_H

#include <QString>
#include <QStringList>

namespace KTextEditor {
class MainWindow;
}

class EditorContext
{
public:
    EditorContext();

    // Capture current editor state from the main window
    void capture(KTextEditor::MainWindow *mw);

    // Return formatted string suitable for injection into system prompt
    QString toSystemPromptChunk() const;

    // True if no document is open or context is empty
    bool isEmpty() const;

    // Get buffer context with line limits
    QString getBufferContext(const QString &content, int cursorLine, int maxChars) const;

    // Get context around cursor position
    QString getBufferContextAroundCursor(const QString &content, int cursorLine, int cursorCol, int linesAround) const;

private:
    // Truncate string to maxLen, appending "..." if cut
    static QString truncate(const QString &text, int maxLen);

    QString filePath;
    QString fileContent;      // truncated to 2000 chars
    int cursorLine = -1;
    int cursorColumn = -1;
    QString selection;         // truncated to 1000 chars
    QStringList openFileList;
};

#endif // EDITORCONTEXT_H
