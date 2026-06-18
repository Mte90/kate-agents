#include "codebasesearchtool.h"
#include <QJsonArray>
#include <KLocalizedString>

CodebaseSearchTool::CodebaseSearchTool(CodebaseIndexer *indexer, QObject *parent)
    : AgentTool(parent), m_indexer(indexer)
{
}

QString CodebaseSearchTool::name() const
{
    return "codebase_search";
}

QString CodebaseSearchTool::description() const
{
    return QObject::tr(
        "Search the codebase for symbols, functions, classes, or content. "
        "Use this to understand the project structure, find specific functions, "
        "or discover how certain patterns are implemented. "
        "Returns matching symbols with file paths and line numbers."
    );
}

QJsonObject CodebaseSearchTool::parametersSchema() const
{
    QJsonObject queryObj;
    queryObj["type"] = "string";
    queryObj["description"] = "Search query (symbol name or content to find)";
    
    QJsonArray enumValues;
    enumValues.append("symbol");
    enumValues.append("content");
    enumValues.append("file");
    
    QJsonObject searchTypeObj;
    searchTypeObj["type"] = "string";
    searchTypeObj["enum"] = enumValues;
    searchTypeObj["description"] = "Type of search: symbol (function/class names), content (text in code), or file (list files)";
    searchTypeObj["default"] = "symbol";
    
    QJsonObject limitObj;
    limitObj["type"] = "integer";
    limitObj["description"] = "Maximum number of results to return";
    limitObj["default"] = 20;
    
    QJsonObject properties;
    properties["query"] = queryObj;
    properties["searchType"] = searchTypeObj;
    properties["limit"] = limitObj;
    
    QJsonObject schema;
    schema["type"] = "object";
    schema["properties"] = properties;
    QJsonArray requiredFields;
    requiredFields.append("query");
    schema["required"] = requiredFields;
    
    return schema;
}

static QJsonObject makeError(const QString &error)
{
    QJsonObject obj;
    obj["success"] = false;
    obj["error"] = error;
    return obj;
}

static QJsonObject makeSuccess(const QJsonObject &data)
{
    QJsonObject obj = data;
    obj["success"] = true;
    return obj;
}

QJsonObject CodebaseSearchTool::execute(const QJsonObject &args)
{
    if (!m_indexer) {
        return makeError(i18n("Codebase indexer not available"));
    }
    
    QString query = args["query"].toString();
    QString searchType = args["searchType"].toString("symbol");
    int limit = args["limit"].toInt(20);
    
    if (query.isEmpty()) {
        return makeError(i18n("Search query is required"));
    }
    
    // Trigger indexing if not already done
    if (!m_indexer->isIndexing() && m_indexer->symbolCount() == 0) {
        m_indexer->startIndexing();
        QJsonObject result;
        result["message"] = i18n("Starting codebase indexing. Please try again in a moment.");
        result["indexing"] = true;
        return makeSuccess(result);
    }
    
    QVector<CodeSnippet> results;
    
    if (searchType == "symbol") {
        results = m_indexer->searchSymbols(query);
    } else if (searchType == "content") {
        results = m_indexer->searchContent(query);
    } else if (searchType == "file") {
        QVector<QString> files = m_indexer->listFiles();
        QString lowerQuery = query.toLower();
        for (const QString &file : files) {
            if (file.contains(lowerQuery, Qt::CaseInsensitive)) {
                CodeSnippet snippet;
                snippet.filePath = file;
                snippet.lineNumber = 0;
                results.append(snippet);
            }
        }
    }
    
    if (results.size() > limit) {
        results.resize(limit);
    }
    
    QJsonArray jsonResults;
    for (const CodeSnippet &snippet : results) {
        QJsonObject obj;
        obj["filePath"] = snippet.filePath;
        obj["lineNumber"] = snippet.lineNumber;
        obj["content"] = snippet.content;
        obj["symbolName"] = snippet.symbolName;
        obj["symbolType"] = snippet.symbolType;
        jsonResults.append(obj);
    }
    
    QJsonObject result;
    result["query"] = query;
    result["searchType"] = searchType;
    result["count"] = results.size();
    result["results"] = jsonResults;
    return makeSuccess(result);
}
