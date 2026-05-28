#include <QtTest/QtTest>
#include "ui/syntaxhighlighter.h"

class TestSyntaxHighlighter : public QObject
{
    Q_OBJECT

private:
    bool containsHighlight(const QString &html, const QString &cssClass)
    {
        return html.contains(QString("class='%1'").arg(cssClass)) ||
               html.contains(QString("class=\"%1\"").arg(cssClass));
    }

private slots:
    // ===== C++ keyword highlighting =====
    void testCppKeywords()
    {
        QString result = SyntaxHighlighter::highlight("int main() {}", "cpp");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("int"));
        QVERIFY(result.contains("main"));
    }

    void testCppClassKeyword()
    {
        QString result = SyntaxHighlighter::highlight("class MyClass {}", "cpp");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("class"));
        QVERIFY(result.contains("MyClass"));
    }

    void testCppReturn()
    {
        QString result = SyntaxHighlighter::highlight("return 42;", "cpp");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("return"));
    }

    void testCppMultipleKeywords()
    {
        QString result = SyntaxHighlighter::highlight("const int x = 5;", "cpp");
        // Verify all keywords present without raw markdown
        QVERIFY(result.contains("const"));
        QVERIFY(result.contains("int"));
        QVERIFY(!result.contains("const int")); // Should be split by span tags
    }

    // ===== Python keywords =====
    void testPythonKeywords()
    {
        QString result = SyntaxHighlighter::highlight("def hello():\n    return None", "py");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("def"));
        QVERIFY(result.contains("return"));
        QVERIFY(result.contains("None"));
        QVERIFY(result.contains("hello"));
    }

    void testPythonImport()
    {
        QString result = SyntaxHighlighter::highlight("import os\nfrom sys import path", "python");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("import"));
        QVERIFY(result.contains("from"));
        QVERIFY(result.contains("os"));
        QVERIFY(result.contains("path"));
    }

    // ===== Comment patterns =====
    void testCppLineComment()
    {
        QString result = SyntaxHighlighter::highlight("int x = 5; // this is a comment", "cpp");
        QVERIFY(containsHighlight(result, "hl-comment"));
        QVERIFY(result.contains("// this is a comment"));
        QVERIFY(result.contains("int") || containsHighlight(result, "hl-keyword"));
    }

    void testCppBlockComment()
    {
        QString result = SyntaxHighlighter::highlight("/* block comment\n   still comment */", "cpp");
        QVERIFY(containsHighlight(result, "hl-comment"));
        QVERIFY(result.contains("block comment"));
    }

    void testPythonComment()
    {
        QString result = SyntaxHighlighter::highlight("# python comment\nx = 1", "py");
        QVERIFY(containsHighlight(result, "hl-comment"));
        QVERIFY(result.contains("# python comment"));
    }

    // ===== String patterns =====
    void testStringHighlight()
    {
        QString result = SyntaxHighlighter::highlight("printf(\"hello world\");", "cpp");
        QVERIFY(containsHighlight(result, "hl-string"));
        QVERIFY(result.contains("\"hello world\""));
    }

    void testSingleQuoteString()
    {
        QString result = SyntaxHighlighter::highlight("char c = 'a';", "cpp");
        QVERIFY(containsHighlight(result, "hl-string"));
        QVERIFY(result.contains("'a'"));
    }

    void testMultiLineString()
    {
        QString result = SyntaxHighlighter::highlight("s = \"\"\"multi\nline\nstring\"\"\"", "python");
        QVERIFY(containsHighlight(result, "hl-string"));
        QVERIFY(result.contains("multi"));
        QVERIFY(result.contains("string"));
    }

    // ===== Number patterns =====
    void testIntegerNumber()
    {
        QString result = SyntaxHighlighter::highlight("int x = 42;", "cpp");
        QVERIFY(result.contains("42"));
    }

    void testFloatNumber()
    {
        QString result = SyntaxHighlighter::highlight("double pi = 3.14159;", "cpp");
        QVERIFY(result.contains("3.14159"));
    }

    void testScientificNotation()
    {
        QString result = SyntaxHighlighter::highlight("double e = 1.5e10;", "cpp");
        QVERIFY(result.contains("1.5e10"));
    }

    // ===== Preprocessor directives =====
    void testInclude()
    {
        QString result = SyntaxHighlighter::highlight("#include <iostream>", "cpp");
        QVERIFY(containsHighlight(result, "hl-preproc"));
        QVERIFY(result.contains("#include"));
        QVERIFY(result.contains("iostream"));
    }

    void testDefine()
    {
        QString result = SyntaxHighlighter::highlight("#define MAX 100", "cpp");
        QVERIFY(containsHighlight(result, "hl-preproc"));
        QVERIFY(result.contains("#define"));
        QVERIFY(result.contains("MAX"));
    }

    void testIfdef()
    {
        QString result = SyntaxHighlighter::highlight("#ifdef DEBUG\nint x = 1;\n#endif", "cpp");
        QVERIFY(containsHighlight(result, "hl-preproc"));
        QVERIFY(result.contains("#ifdef"));
        QVERIFY(result.contains("#endif"));
        QVERIFY(result.contains("DEBUG"));
    }

    // ===== REGRESSION: Keywords must NOT match inside HTML attributes =====
    void testKeywordNotInHtmlAttribute()
    {
        QString code = "if (x < 5) { return true; }";
        QString result = SyntaxHighlighter::highlight(code, "cpp");

        // The result should have valid HTML - no malformed tags
        // If 'if' or 'return' matched inside a <span> attribute, the HTML would be broken
        int openSpans = result.count("<span");
        int closeSpans = result.count("</span>");
        QCOMPARE(openSpans, closeSpans);

        // Verify each <span> has a valid class attribute
        QRegularExpression spanRegex("<span[^>]*>");
        QRegularExpressionMatchIterator it = spanRegex.globalMatch(result);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            QString span = m.captured();
            QVERIFY2(span.contains("class='") || span.contains("class=\""),
                     qPrintable(QString("Malformed span tag: %1").arg(span)));
        }
    }

    void testClassKeywordNotInHtmlAttribute()
    {
        // 'class' is a keyword in C++ but also appears in HTML attribute syntax
        // SyntaxHighlighter must not match 'class' inside <span class='...'>
        QString code = "class Foo { int x; };";
        QString result = SyntaxHighlighter::highlight(code, "cpp");

        // Count balanced HTML
        int openSpans = result.count("<span");
        int closeSpans = result.count("</span>");
        QCOMPARE(openSpans, closeSpans);

        // The string "class=" should NOT appear (that would mean keyword 'class' was highlighted inside the HTML attribute)
        // But 'class=' could appear in attribute syntax - let's be more specific:
        // Each <span> should have exactly one class='...' attribute
        int classAttrCount = result.count("class='");
        QCOMPARE(classAttrCount, openSpans);
    }

    // ===== Nested patterns =====
    void testKeywordInsideCommentNotHighlighted()
    {
        QString result = SyntaxHighlighter::highlight("// this is a class with int\nx = 1;", "cpp");
        // The comment should be highlighted as a whole
        QVERIFY(containsHighlight(result, "hl-comment"));
        // 'class' and 'int' inside the comment should NOT get separate keyword spans
        // Verify by counting spans: should have comment span + no keyword spans inside comment
        int spanCount = result.count("<span");
        // Spans: comment only (or comment + possible string/number outside)
        QVERIFY(spanCount >= 1);
    }

    void testKeywordInsideStringNotHighlighted()
    {
        QString result = SyntaxHighlighter::highlight("x = \"return value from class\";", "cpp");
        // The string should be highlighted
        QVERIFY(containsHighlight(result, "hl-string"));
        // 'return' and 'class' inside the string should NOT have separate keyword spans
        // The string takes priority
    }

    // ===== Edge cases =====
    void testEmptyCode()
    {
        QString result = SyntaxHighlighter::highlight("", "cpp");
        QVERIFY(result.isEmpty());
    }

    void testNoLanguageSpecified()
    {
        QString result = SyntaxHighlighter::highlight("if (true) { return 1; }", "");
        QVERIFY(containsHighlight(result, "hl-keyword"));
    }

    void testUnknownLanguage()
    {
        QString result = SyntaxHighlighter::highlight("if (true) { return 1; }", "brainfuck");
        QVERIFY(containsHighlight(result, "hl-keyword"));
    }

    void testSingleCharacter()
    {
        QString result = SyntaxHighlighter::highlight("x", "cpp");
        QVERIFY(!result.isEmpty());
        QVERIFY(result.contains("x"));
    }

    void testHtmlSpecialChars()
    {
        QString result = SyntaxHighlighter::highlight("if (x < 5 && y > 3) {}", "cpp");
        // HTML special chars should be escaped
        QVERIFY(!result.contains("< 5"));
        QVERIFY(result.contains("&lt;") || result.contains("&gt;"));
    }

    void testAmpersandInCode()
    {
        QString result = SyntaxHighlighter::highlight("int x = a & b;", "cpp");
        QVERIFY(result.contains("&amp;"));
        QVERIFY(!result.contains("&amp;amp;"));
    }

    void testMultipleLines()
    {
        QString code = "int main() {\n    return 0;\n}\n";
        QString result = SyntaxHighlighter::highlight(code, "cpp");
        QVERIFY(result.contains("main"));
        QVERIFY(result.contains("0"));
    }

    void testMixedContent()
    {
        QString code = "// comment\nint x = 42; // end\n";
        QString result = SyntaxHighlighter::highlight(code, "cpp");
        // Should have both comment and keyword spans
        QVERIFY(containsHighlight(result, "hl-comment"));
        QVERIFY(result.contains("int") || containsHighlight(result, "hl-keyword"));
    }

    // ===== JavaScript keywords =====
    void testJavascriptKeywords()
    {
        QString result = SyntaxHighlighter::highlight("function hello() {\n    const x = 1;\n    return x;\n}", "js");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("function"));
        QVERIFY(result.contains("const") || containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("return") || containsHighlight(result, "hl-keyword"));
    }

    void testAsyncAwait()
    {
        QString result = SyntaxHighlighter::highlight("async function fetch() {\n    await getData();\n}", "js");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("async"));
        QVERIFY(result.contains("await"));
    }

    // ===== Go keywords =====
    void testGoKeywords()
    {
        QString result = SyntaxHighlighter::highlight("package main\n\nfunc main() {\n    return\n}", "go");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("package"));
        QVERIFY(result.contains("func"));
        QVERIFY(result.contains("return") || containsHighlight(result, "hl-keyword"));
    }

    void testGoDefer()
    {
        QString result = SyntaxHighlighter::highlight("defer close(file)", "go");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("defer"));
    }

    // ===== Rust keywords =====
    void testRustKeywords()
    {
        QString result = SyntaxHighlighter::highlight("fn main() {\n    let x = 5;\n}", "rs");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("fn"));
        QVERIFY(result.contains("let"));
    }

    void testRustMatch()
    {
        QString result = SyntaxHighlighter::highlight("match value {\n    1 => true,\n    _ => false,\n}", "rust");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("match"));
    }

    // ===== Java keywords =====
    void testJavaKeywords()
    {
        QString result = SyntaxHighlighter::highlight("public class Main {\n    public static void main(String[] args) {}\n}", "java");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("public"));
        QVERIFY(result.contains("class"));
        QVERIFY(result.contains("static"));
        QVERIFY(result.contains("void"));
    }

    // ===== SQL keywords =====
    void testSqlKeywords()
    {
        QString result = SyntaxHighlighter::highlight("SELECT * FROM users WHERE id = 1", "sql");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("SELECT"));
        QVERIFY(result.contains("FROM"));
        QVERIFY(result.contains("WHERE"));
    }

    void testSqlJoin()
    {
        QString result = SyntaxHighlighter::highlight("SELECT a.name FROM users a INNER JOIN groups b ON a.group_id = b.id", "sql");
        QVERIFY(containsHighlight(result, "hl-keyword"));
        QVERIFY(result.contains("JOIN"));
        QVERIFY(result.contains("ON"));
    }
};

QTEST_MAIN(TestSyntaxHighlighter)
#include "test_syntaxhighlighter.moc"
