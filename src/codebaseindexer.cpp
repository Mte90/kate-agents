#include "codebaseindexer.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

CodebaseIndexer::CodebaseIndexer(QObject *parent)
    : QObject(parent)
{
}

void CodebaseIndexer::setProjectPath(const QString &path)
{
    QMutexLocker locker(&m_mutex);
    m_projectPath = path;
}

void CodebaseIndexer::startIndexing()
{
    if (m_isIndexing || m_projectPath.isEmpty()) {
        return;
    }
    
    m_isIndexing = true;
    emit indexingStarted();
    
    m_indexingFuture = QtConcurrent::run([this]() {
        QMutexLocker locker(&m_mutex);
        m_symbols.clear();
        m_files.clear();
        
        QDir dir(m_projectPath);
        QStringList filters = supportedExtensions();
        dir.setNameFilters(filters);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        
        // Use QDirIterator for recursive listing in Qt6
        QDirIterator it(m_projectPath, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
        
        QStringList files;
        while (it.hasNext()) {
            QString path = it.next();
            QFileInfo fi(path);
            QString suffix = fi.suffix().toLower();
            if (!suffix.isEmpty()) {
                // Filter by extension
                bool matched = false;
                for (const QString &ext : filters) {
                    if (suffix == ext.mid(1)) { // Remove '*' prefix
                        matched = true;
                        break;
                    }
                }
                if (matched) {
                    files.append(path);
                }
            }
        }
        
        int totalFiles = files.size();
        int processedFiles = 0;
        
        for (const QString &filePath : files) {
            if (!m_isIndexing) {
                break;
            }
            
            indexFile(filePath);
            processedFiles++;
            
            int percent = (processedFiles * 100) / qMax(1, totalFiles);
            emit indexingProgress(percent, QString("Indexing: %1/%2 files").arg(processedFiles).arg(totalFiles));
        }
        
        m_isIndexing = false;
        emit indexingCompleted(true);
        emit indexUpdated();
    });
}

void CodebaseIndexer::cancelIndexing()
{
    m_isIndexing = false;
}

void CodebaseIndexer::indexFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    m_files.append(filePath);
    parseFileForSymbols(filePath, content);
}

void CodebaseIndexer::parseFileForSymbols(const QString &filePath, const QString &content)
{
    QStringList lines = content.split('\n');
    QString fileType = QFileInfo(filePath).suffix().toLower();
    
    QRegularExpression functionPattern(R"((?:function|def|fn|auto|void|int|bool|string|const|static)\s+(\w+)\s*\([^)]*\)\s*(?:const)?\s*\{?)");
    QRegularExpression classPattern(R"((?:class|struct|interface)\s+(\w+))");
    QRegularExpression variablePattern(R"((?:let|var|const|auto|int|bool|string)\s+(\w+)\s*[=;])");
    
    for (int i = 0; i < lines.size(); i++) {
        const QString &line = lines[i].trimmed();
        
        // Match functions
        QRegularExpressionMatch functionMatch = functionPattern.match(line);
        if (functionMatch.hasMatch()) {
            CodeSnippet snippet;
            snippet.filePath = filePath;
            snippet.lineNumber = i + 1;
            snippet.content = line;
            snippet.symbolName = functionMatch.captured(1);
            snippet.symbolType = "function";
            m_symbols.append(snippet);
        }
        
        // Match classes/structs
        QRegularExpressionMatch classMatch = classPattern.match(line);
        if (classMatch.hasMatch()) {
            CodeSnippet snippet;
            snippet.filePath = filePath;
            snippet.lineNumber = i + 1;
            snippet.content = line;
            snippet.symbolName = classMatch.captured(1);
            snippet.symbolType = "class";
            m_symbols.append(snippet);
        }
        
        // Match variables (simplified)
        QRegularExpressionMatch varMatch = variablePattern.match(line);
        if (varMatch.hasMatch()) {
            CodeSnippet snippet;
            snippet.filePath = filePath;
            snippet.lineNumber = i + 1;
            snippet.content = line;
            snippet.symbolName = varMatch.captured(1);
            snippet.symbolType = "variable";
            m_symbols.append(snippet);
        }
    }
}

QStringList CodebaseIndexer::supportedExtensions() const
{
    return QStringList()
        << "*.cpp" << "*.h" << "*.hpp" << "*.c" << "*.cc" << "*.cxx"
        << "*.py" << "*.pyi"
        << "*.js" << "*.ts" << "*.tsx" << "*.jsx" << "*.mjs"
        << "*.rs"
        << "*.go"
        << "*.java"
        << "*.kt" << "*.kts"
        << "*.swift"
        << "*.rb"
        << "*.php"
        << "*.cs"
        << "*.scala"
        << "*.lua"
        << "*.pl" << "*.pm"
        << "*.sh" << "*.bash"
        << "*.yaml" << "*.yml" << "*.json" << "*.xml" << "*.toml"
        << "*.md" << "*.txt";
}

QVector<CodeSnippet> CodebaseIndexer::searchSymbols(const QString &query) const
{
    QMutexLocker locker(&m_mutex);
    QVector<CodeSnippet> results;
    
    QString lowerQuery = query.toLower();
    for (const CodeSnippet &snippet : m_symbols) {
        if (snippet.symbolName.toLower().contains(lowerQuery)) {
            results.append(snippet);
        }
    }
    
    return results;
}

QVector<CodeSnippet> CodebaseIndexer::searchContent(const QString &query) const
{
    QMutexLocker locker(&m_mutex);
    QVector<CodeSnippet> results;
    
    QString lowerQuery = query.toLower();
    for (const CodeSnippet &snippet : m_symbols) {
        if (snippet.content.toLower().contains(lowerQuery)) {
            results.append(snippet);
        }
    }
    
    return results;
}

QVector<QString> CodebaseIndexer::listFiles() const
{
    QMutexLocker locker(&m_mutex);
    QVector<QString> result;
    for (const QString &file : m_files) {
        result.append(file);
    }
    return result;
}
