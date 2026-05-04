#ifndef WEBSearchTOOL_H
#define WEBSearchTOOL_H

#include "../toolregistry.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QRegularExpression>

class WebSearchTool : public AgentTool
{
    Q_OBJECT

private:
    // Extract text from HTML (simple regex-based)
    QString extractTextFromHtml(const QString &html) const {
        QString text = html;
        text.remove(QRegularExpression("<script[^>]*>.*?</script>", QRegularExpression::DotMatchesEverythingOption));
        text.remove(QRegularExpression("<style[^>]*>.*?</style>", QRegularExpression::DotMatchesEverythingOption));
        text.remove(QRegularExpression("<[^>]+>"));
        text.replace("&nbsp;", " ");
        text.replace("&amp;", "&");
        text.replace("&lt;", "<");
        text.replace("&gt;", ">");
        text.replace("&quot;", "\"");
        text.replace("&#39;", "'");
        text.replace("\n", " ");
        text.replace("\r", "");
        text.replace("\t", " ");
        while (text.contains("  ")) {
            text.replace("  ", " ");
        }
        return text.trimmed();
    }

public:
    explicit WebSearchTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "web_search"; }
    QString description() const override {
        return "Cerca sul web e restituisce risultati rilevanti con summary";
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject queryProp;
        queryProp["type"] = "string";
        queryProp["description"] = "Query di ricerca (es. 'latest Python 3.12 features')";
        properties["query"] = queryProp;
        
        QJsonObject numProp;
        numProp["type"] = "integer";
        numProp["description"] = "Numero di risultati (default: 5, max: 10)";
        numProp["default"] = 5;
        properties["num_results"] = numProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"query"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString query = args["query"].toString();
        int numResults = qMin(args["num_results"].toInt(5), 10);
        
        if (query.isEmpty()) {
            return QJsonObject{
                {"success", false},
                {"error", "Query di ricerca vuota"}
            };
        }

        // Use DuckDuckGo HTML scraping (no API key required)
        QNetworkAccessManager manager;
        QString encodedQuery = QUrl::toPercentEncoding(query);
        QUrl url(QString("https://lite.duckduckgo.com/lite/?q=%1").arg(encodedQuery));
        
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, 
            "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
        
        QEventLoop loop;
        QNetworkReply *reply = manager.get(request);
        
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        
        // Timeout after 30 seconds
        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        timeoutTimer.start(30000);
        
        connect(&timeoutTimer, &QTimer::timeout, [&]() {
            reply->abort();
            loop.quit();
        });
        
        loop.exec();
        
        if (timeoutTimer.isActive()) {
            timeoutTimer.stop();
        }
        
        if (reply->error() != QNetworkReply::NoError) {
            QString errorMsg = reply->errorString();
            reply->deleteLater();
            return QJsonObject{
                {"success", false},
                {"error", QString("Network error: %1").arg(errorMsg)}
            };
        }
        
        QString html = QString::fromUtf8(reply->readAll());
        reply->deleteLater();
        
        // Parse results from DuckDuckGo HTML
        QStringList results;
        
        // Simple regex to extract result blocks - escaped properly
        QRegularExpression resultRegex("<a href=\"([^\"]+)\"[^>]*>([^<]+)</a>.*?<a class=\"result__snippet\"[^>]*>([^<]+)</a>", 
            QRegularExpression::DotMatchesEverythingOption);
        
        QRegularExpressionMatchIterator i = resultRegex.globalMatch(html);
        int count = 0;
        QString summaryText;
        
        while (i.hasNext() && count < numResults) {
            QRegularExpressionMatch match = i.next();
            QString url = match.captured(1);
            QString title = extractTextFromHtml(match.captured(2));
            QString snippet = extractTextFromHtml(match.captured(3));
            
            if (!title.isEmpty() && !url.startsWith("javascript")) {
                QString result = QString("[%1] %2\n  URL: %3\n  %4")
                    .arg(count + 1)
                    .arg(title)
                    .arg(url)
                    .arg(snippet);
                
                results.append(result);
                summaryText += result + "\n\n";
                count++;
            }
        }
        
        // Fallback: if no results found, return error
        if (results.isEmpty()) {
            QJsonArray emptyArray;
            return QJsonObject{
                {"success", false},
                {"error", "Nessun risultato trovato"},
                {"results", emptyArray}
            };
        }
        
        // Truncate summary if too long
        if (summaryText.length() > 5000) {
            summaryText = summaryText.left(5000) + "\n... (truncated)";
        }
        
        QJsonArray resultsArray;
        for (const QString &result : results) {
            resultsArray.append(result);
        }
        
        return QJsonObject{
            {"success", true},
            {"query", query},
            {"results", resultsArray},
            {"summary", summaryText},
            {"count", count}
        };
    }

    bool requiresPermission() const override { return false; }
};

#endif // WEBSearchTOOL_H
