#include "syntaxhighlighter.h"
#include <QSet>

static const QSet<QString> cppKeywords = {
    "auto", "break", "case", "catch", "class", "const", "continue", "default",
    "delete", "do", "else", "enum", "explicit", "export", "extern", "for",
    "friend", "goto", "if", "inline", "mutable", "namespace", "new", "nullptr",
    "operator", "private", "protected", "public", "register", "return", "sizeof",
    "static", "static_cast", "struct", "switch", "template", "this", "throw",
    "try", "typedef", "typeid", "typename", "union", "using", "virtual", "volatile",
    "while", "int", "long", "short", "unsigned", "signed", "char", "double",
    "float", "bool", "void", "true", "false", "const_cast", "dynamic_cast",
    "reinterpret_cast", "noexcept", "override", "final", "decltype", "constexpr"
};

static const QSet<QString> pythonKeywords = {
    "and", "as", "assert", "break", "class", "continue", "def", "del", "elif",
    "else", "except", "finally", "for", "from", "global", "if", "import", "in",
    "is", "lambda", "nonlocal", "not", "or", "pass", "raise", "return", "try",
    "while", "with", "yield", "True", "False", "None", "async", "await"
};

static const QSet<QString> javascriptKeywords = {
    "var", "let", "const", "function", "return", "if", "else", "for", "while",
    "do", "switch", "case", "break", "continue", "default", "try", "catch",
    "finally", "throw", "new", "delete", "typeof", "instanceof", "void",
    "debugger", "with", "class", "extends", "super", "import", "export",
    "from", "as", "async", "await", "static", "get", "set", "true", "false",
    "null", "undefined", "this", "typeof", "instanceof"
};

static const QSet<QString> rustKeywords = {
    "as", "async", "await", "break", "const", "continue", "crate", "else",
    "enum", "extern", "false", "fn", "for", "if", "impl", "in", "let", "loop",
    "match", "mod", "move", "mut", "pub", "ref", "return", "self", "Self",
    "static", "struct", "super", "true", "trait", "type", "unsafe", "use",
    "where", "while", "dyn", "abstract", "become", "box", "do", "final",
    "macro", "override", "priv", "try", "typeof", "unsized", "virtual", "yield"
};

static const QSet<QString> goKeywords = {
    "break", "case", "chan", "const", "continue", "default", "defer", "else",
    "fallthrough", "for", "func", "go", "goto", "if", "import", "interface",
    "map", "package", "range", "return", "select", "struct", "switch", "type",
    "var"
};

static const QSet<QString> javaKeywords = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new",
    "package", "private", "protected", "public", "return", "short", "static",
    "strictfp", "super", "switch", "synchronized", "this", "throw", "throws",
    "transient", "try", "void", "volatile", "while", "true", "false", "null"
};

static const QSet<QString> sqlKeywords = {
    "SELECT", "FROM", "WHERE", "INSERT", "UPDATE", "DELETE", "JOIN", "LEFT",
    "RIGHT", "INNER", "OUTER", "ON", "AND", "OR", "NOT", "NULL", "IS", "LIKE",
    "IN", "BETWEEN", "EXISTS", "CASE", "WHEN", "THEN", "ELSE", "END", "AS",
    "ORDER", "BY", "GROUP", "HAVING", "LIMIT", "OFFSET", "DISTINCT", "ALL",
    "ANY", "SOME", "ASC", "DESC", "CREATE", "DROP", "ALTER", "TABLE", "INDEX",
    "VIEW", "TRIGGER", "PROCEDURE", "FUNCTION", "PRIMARY", "FOREIGN", "KEY",
    "REFERENCES", "UNIQUE", "CHECK", "DEFAULT", "CASCADE", "SET", "VALUES",
    "INTO", "INTO", "CROSS", "FULL", "NATURAL", "USING", "WITH", "RECURSIVE"
};

static const QSet<QString> commonKeywords = {
    "int", "float", "double", "char", "long", "short", "bool", "void", "string",
    "const", "static", "extern", "inline", "virtual", "override", "final",
    "private", "protected", "public", "namespace", "using", "typedef", "struct",
    "enum", "union", "class", "interface", "implements", "extends", "import",
    "export", "module", "package"
};

static const QRegularExpression commentPattern("(//|#|/\\*|\\b--)[^\\n]*");
static const QRegularExpression stringPattern("(\"\"\"[\\s\\S]*?\"\"\"|'''[\\s\\S]*?'''|\"[^\"]*\"|'[^']*'|`[^`]*`)");
static const QRegularExpression numberPattern("\\b\\d+\\.?\\d*([eE][+-]?\\d+)?\\b");
static const QRegularExpression preprocessorPattern("#\\w+[^\\n]*");

const QSet<QString>& SyntaxHighlighter::getCppKeywords() { return cppKeywords; }
const QSet<QString>& SyntaxHighlighter::getPythonKeywords() { return pythonKeywords; }
const QSet<QString>& SyntaxHighlighter::getJavascriptKeywords() { return javascriptKeywords; }
const QSet<QString>& SyntaxHighlighter::getRustKeywords() { return rustKeywords; }
const QSet<QString>& SyntaxHighlighter::getGoKeywords() { return goKeywords; }
const QSet<QString>& SyntaxHighlighter::getJavaKeywords() { return javaKeywords; }
const QSet<QString>& SyntaxHighlighter::getSqlKeywords() { return sqlKeywords; }
const QSet<QString>& SyntaxHighlighter::getCommonKeywords() { return commonKeywords; }
const QRegularExpression& SyntaxHighlighter::getCommentPattern() { return commentPattern; }
const QRegularExpression& SyntaxHighlighter::getStringPattern() { return stringPattern; }
const QRegularExpression& SyntaxHighlighter::getNumberPattern() { return numberPattern; }
const QRegularExpression& SyntaxHighlighter::getPreprocessorPattern() { return preprocessorPattern; }

void SyntaxHighlighter::escapeHtml(QString &text)
{
    text.replace("&", "&amp;");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    text.replace("\"", "&quot;");
}

QString SyntaxHighlighter::highlight(const QString &code, const QString &language)
{
    QString result = code;
    escapeHtml(result);
    
    // Define token types and their HTML spans
    struct Token {
        QRegularExpression pattern;
        QString htmlSpan;
        int priority; // Lower = processed first
    };
    
    // Get keywords for the language
    const QSet<QString> *keywords = nullptr;
    QString langLower = language.toLower();
    
    if (langLower == "cpp" || langLower == "c" || langLower == "c++") {
        keywords = &cppKeywords;
    } else if (langLower == "python" || langLower == "py") {
        keywords = &pythonKeywords;
    } else if (langLower == "javascript" || langLower == "js" || langLower == "jsx") {
        keywords = &javascriptKeywords;
    } else if (langLower == "rust" || langLower == "rs") {
        keywords = &rustKeywords;
    } else if (langLower == "go" || langLower == "golang") {
        keywords = &goKeywords;
    } else if (langLower == "java") {
        keywords = &javaKeywords;
    } else if (langLower == "sql") {
        keywords = &sqlKeywords;
    } else {
        keywords = &commonKeywords;
    }
    
    QStringList kwList = keywords->toList();
    kwList.append(commonKeywords.toList());
    kwList.removeDuplicates();
    QString kwPattern = "\\b(" + kwList.join("|") + ")\\b";
    QRegularExpression keywordRegex(kwPattern, QRegularExpression::CaseInsensitiveOption);
    
    QList<Token> tokens;
    tokens.append({getCommentPattern(), "<span class='hl-comment'>\\1</span>", 1});
    tokens.append({getStringPattern(), "<span class='hl-string'>\\1</span>", 2});
    tokens.append({keywordRegex, "<span class='hl-keyword'>\\1</span>", 3});
    tokens.append({numberPattern, "<span class='hl-number'>\\1</span>", 4});
    tokens.append({getPreprocessorPattern(), "<span class='hl-preproc'>\\1</span>", 5});
    
    QMultiMap<int, QString> marked;
    
    for (const Token &token : tokens) {
        QRegularExpressionMatchIterator it = token.pattern.globalMatch(result);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            int start = match.capturedStart();
            int len = match.capturedLength();
            QString replacement = match.captured(0);
            
            if (token.pattern.pattern() == getCommentPattern().pattern()) {
                int commentStart = match.capturedStart(1);
                int commentLen = match.capturedLength(1);
                QString before = result.mid(0, commentStart);
                QString comment = result.mid(commentStart, commentLen);
                QString after = result.mid(commentStart + commentLen);
                result = before + "<span class='hl-comment'>" + comment + "</span>" + after;
            } else if (token.pattern.pattern() == getStringPattern().pattern()) {
                int strStart = match.capturedStart(1);
                int strLen = match.capturedLength(1);
                QString before = result.mid(0, strStart);
                QString str = result.mid(strStart, strLen);
                QString after = result.mid(strStart + strLen);
                result = before + "<span class='hl-string'>" + str + "</span>" + after;
            } else if (token.pattern.pattern() == keywordRegex.pattern()) {
                int kwStart = match.capturedStart(1);
                int kwLen = match.capturedLength(1);
                QString before = result.mid(0, kwStart);
                QString kw = result.mid(kwStart, kwLen);
                QString after = result.mid(kwStart + kwLen);
                result = before + "<span class='hl-keyword'>" + kw + "</span>" + after;
            } else if (token.pattern.pattern() == numberPattern.pattern()) {
                int numStart = match.capturedStart(1);
                int numLen = match.capturedLength(1);
                QString before = result.mid(0, numStart);
                QString num = result.mid(numStart, numLen);
                QString after = result.mid(numStart + numLen);
                result = before + "<span class='hl-number'>" + num + "</span>" + after;
            } else if (token.pattern.pattern() == getPreprocessorPattern().pattern()) {
                int ppStart = match.capturedStart(1);
                int ppLen = match.capturedLength(1);
                QString before = result.mid(0, ppStart);
                QString pp = result.mid(ppStart, ppLen);
                QString after = result.mid(ppStart + ppLen);
                result = before + "<span class='hl-preproc'>" + pp + "</span>" + after;
            }
        }
    }
    
    return result;
}
