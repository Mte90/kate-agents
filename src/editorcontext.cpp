#include "editorcontext.h"

#include <KTextEditor/MainWindow>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>

EditorContext::EditorContext()
    : cursorLine(-1)
    , cursorColumn(-1)
{
}

void EditorContext::capture(KTextEditor::MainWindow *mw)
{
    if (!mw || !mw->activeView()) {
        return;
    }

    auto *view = mw->activeView();
    auto *doc = view->document();

    // File path
    filePath = doc->url().toLocalFile();

    // File content (truncated to 2000 chars)
    fileContent = truncate(doc->text(), 2000);

    // Cursor position
    auto cursor = view->cursorPosition();
    cursorLine = cursor.line();
    cursorColumn = cursor.column();

    // Selection (truncated to 1000 chars)
    selection = truncate(view->selectionText(), 1000);

    // Open files list
    openFileList.clear();
    auto *editor = KTextEditor::Editor::instance();
    if (editor) {
        const auto docs = editor->documents();
        for (KTextEditor::Document *d : docs) {
            if (openFileList.count() < 10) {
                openFileList.append(d->url().fileName());
            }
        }
    }
}

QString EditorContext::toSystemPromptChunk() const
{
    if (isEmpty()) {
        return QString();
    }

    QString result;
    result += "[Editor Context]\n";
    result += "Active File: " + filePath + "\n";
    result += "Cursor: line " + QString::number(cursorLine) + ", column " + QString::number(cursorColumn) + "\n";

    if (selection.isEmpty()) {
        result += "Selection: (none)\n";
    } else {
        result += "Selection: " + selection + "\n";
    }

    if (!openFileList.isEmpty()) {
        result += "Open Files: " + openFileList.join(", ") + "\n";
    }

    result += "File Content (first 2000 chars):\n";
    result += fileContent;

    return result;
}

bool EditorContext::isEmpty() const
{
    return cursorLine < 0;
}

QString EditorContext::truncate(const QString &text, int maxLen)
{
    if (text.length() <= maxLen) {
        return text;
    }
    return text.left(maxLen) + "...";
}

QString EditorContext::getBufferContext(const QString &content, int cursorLine, int maxChars) const
{
    if (content.isEmpty()) return QString();
    
    QStringList lines = content.split('\n');
    if (cursorLine >= lines.size()) cursorLine = lines.size() - 1;
    
    QString result;
    int currentLen = 0;
    int startLine = qMax(0, cursorLine - 5);
    int endLine = qMin(lines.size() - 1, cursorLine + 5);
    
    for (int i = startLine; i <= endLine; ++i) {
        if (currentLen + lines[i].length() > maxChars) {
            result += "...";
            break;
        }
        if (i == cursorLine) result += ">>> ";
        result += lines[i] + "\n";
        currentLen += lines[i].length() + 1;
    }
    
    return result.trimmed();
}

QString EditorContext::getBufferContextAroundCursor(const QString &content, int cursorLine, int cursorCol, int linesAround) const
{
    if (content.isEmpty()) return QString();
    
    QStringList lines = content.split('\n');
    if (cursorLine >= lines.size()) cursorLine = lines.size() - 1;
    
    QString result;
    int startLine = qMax(0, cursorLine - linesAround);
    int endLine = qMin(lines.size() - 1, cursorLine + linesAround);
    
    for (int i = startLine; i <= endLine; ++i) {
        if (i == cursorLine) {
            QString lineWithMarker = lines[i];
            if (cursorCol >= 0 && cursorCol < lineWithMarker.length()) {
                lineWithMarker.insert(cursorCol, "<<<CURSOR>>>");
            }
            result += ">>> Line " + QString::number(i+1) + ": " + lineWithMarker + "\n";
        } else {
            result += "    Line " + QString::number(i+1) + ": " + lines[i] + "\n";
        }
    }
    
    return result.trimmed();
}
