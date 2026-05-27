#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QSet>

/**
 * Lightweight regex-based syntax highlighter for code blocks.
 * Generates styled HTML spans for common programming languages.
 * Designed to work with QTextBrowser's HTML rendering.
 */
class SyntaxHighlighter
{
public:
    static QString highlight(const QString &code, const QString &language);

private:
    static void escapeHtml(QString &text);
    
    // Keyword sets for common languages
    static const QSet<QString>& getCppKeywords();
    static const QSet<QString>& getPythonKeywords();
    static const QSet<QString>& getJavascriptKeywords();
    static const QSet<QString>& getRustKeywords();
    static const QSet<QString>& getGoKeywords();
    static const QSet<QString>& getJavaKeywords();
    static const QSet<QString>& getRKeywords();
    static const QSet<QString>& getSqlKeywords();
    
    // Generic keywords that apply to most languages
    static const QSet<QString>& getCommonKeywords();
    
    // Pattern definitions
    static const QRegularExpression& getCommentPattern();
    static const QRegularExpression& getStringPattern();
    static const QRegularExpression& getNumberPattern();
    static const QRegularExpression& getPreprocessorPattern();
};

#endif // SYNTAXHIGHLIGHTER_H
