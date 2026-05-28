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
    "export", "module", "package",
    "if", "else", "for", "while", "do", "switch", "case", "break", "continue",
    "return", "try", "catch", "throw", "new", "delete", "sizeof", "this",
    "true", "false", "NULL", "nullptr", "typename", "operator"
};

static const QRegularExpression commentPattern("(//|(?!#\\w)#[^\\n]*|/\\*|\\b--)[^\\n]*");
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
    // Don't escape " or ' — they are needed for string/regex pattern matching
    // and are safe in text content (only dangerous in HTML attribute values)
}

struct HighlightMatch {
    int start;
    int length;
    int priority; // lower = higher priority (processed first)
    QString cssClass;
};

QString SyntaxHighlighter::highlight(const QString &code, const QString &language)
{
    QString result = code;
    escapeHtml(result);
    
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
    
    QStringList kwList = keywords->values();
    kwList.append(commonKeywords.values());
    kwList.removeDuplicates();
    QString kwPattern = "\\b(" + kwList.join("|") + ")\\b";
    QRegularExpression keywordRegex(kwPattern, QRegularExpression::CaseInsensitiveOption);
    
    // Define highlighting rules with priority (lower = higher priority)
    // Comments and strings have highest priority so keywords/numbers inside them are not matched
    struct Rule {
        QRegularExpression regex;
        QString cssClass;
        int priority;
    };
    
    QList<Rule> rules;
    rules.append({getCommentPattern(), QStringLiteral("hl-comment"), 1});
    rules.append({getStringPattern(), QStringLiteral("hl-string"), 1});
    rules.append({getPreprocessorPattern(), QStringLiteral("hl-preproc"), 2});
    rules.append({keywordRegex, QStringLiteral("hl-keyword"), 3});
    rules.append({numberPattern, QStringLiteral("hl-number"), 3});
    
    // Step 1: Collect ALL matches on the ORIGINAL escaped string (before any span insertion)
    // This prevents the in-place modification bug and the keyword-in-HTML-attribute bug
    QList<HighlightMatch> allMatches;
    
    for (const Rule &rule : rules) {
        QRegularExpressionMatchIterator it = rule.regex.globalMatch(result);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            int matchStart = match.capturedStart(0); // Use full match (group 0)
            int matchLen = match.capturedLength(0);
            
            if (matchStart < 0 || matchLen <= 0) {
                continue;
            }
            
            // Check if this match overlaps with an already-collected higher-priority match
            bool overlaps = false;
            for (const HighlightMatch &existing : allMatches) {
                // existing has higher priority (lower number) or same priority
                if (existing.priority > rule.priority) {
                    continue; // Only higher-priority (lower number) matches can block
                }
                // Check overlap: two ranges [a, a+L) and [b, b+L)
                if (matchStart < existing.start + existing.length &&
                    matchStart + matchLen > existing.start) {
                    overlaps = true;
                    break;
                }
            }
            if (overlaps) {
                continue;
            }
            
            allMatches.append({matchStart, matchLen, rule.priority, rule.cssClass});
        }
    }
    
    // Step 2: Sort by position DESCENDING so replacements don't shift later positions
    std::sort(allMatches.begin(), allMatches.end(),
              [](const HighlightMatch &a, const HighlightMatch &b) {
                  return a.start > b.start;
              });
    
    // Step 3: Apply replacements from last to first (right-to-left in the string)
    for (const HighlightMatch &m : allMatches) {
        QString before = result.mid(0, m.start);
        QString content = result.mid(m.start, m.length);
        QString after = result.mid(m.start + m.length);
        result = before + QStringLiteral("<span class='%1'>%2</span>").arg(m.cssClass, content) + after;
    }
    
    return result;
}
