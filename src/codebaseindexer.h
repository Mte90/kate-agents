#ifndef CODEBASEINDEXER_H
#define CODEBASEINDEXER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVector>
#include <QMutex>
#include <QFuture>
#include <QStringList>
#include <functional>

struct CodeSnippet {
    QString filePath;
    int lineNumber;
    QString content;
    QString symbolName;
    QString symbolType; // function, class, variable, etc.
};

class CodebaseIndexer : public QObject
{
    Q_OBJECT

public:
    explicit CodebaseIndexer(QObject *parent = nullptr);
    
    void setProjectPath(const QString &path);
    QString projectPath() const { return m_projectPath; }
    
    void startIndexing();
    void cancelIndexing();
    
    QVector<CodeSnippet> searchSymbols(const QString &query) const;
    QVector<CodeSnippet> searchContent(const QString &query) const;
    QVector<QString> listFiles() const;
    
    int symbolCount() const { return m_symbols.size(); }
    int fileCount() const { return m_files.size(); }
    bool isIndexing() const { return m_isIndexing; }

signals:
    void indexingStarted();
    void indexingProgress(int percent, const QString &status);
    void indexingCompleted(bool success);
    void indexUpdated();

private:
    void indexFile(const QString &filePath);
    void parseFileForSymbols(const QString &filePath, const QString &content);
    QStringList supportedExtensions() const;
    QVector<CodeSnippet> searchByPredicate(const QString &query, std::function<bool(const CodeSnippet&)> predicate) const;
    
    QString m_projectPath;
    mutable QMutex m_mutex;
    QVector<CodeSnippet> m_symbols;
    QStringList m_files;
    bool m_isIndexing = false;
    QFuture<void> m_indexingFuture;
};

#endif
