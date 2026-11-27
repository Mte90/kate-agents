#ifndef WEBSearchTOOL_H
#define WEBSearchTOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QTimer>
#include <QUrl>

class WebSearchTool : public AgentTool
{
    Q_OBJECT

private:
        static constexpr qint64 kMaxDownloadBytes = 5 * 1024 * 1024; // 5 MiB
        static constexpr int kMaxSummaryChars = 5000;
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

    bool isSafeResultUrl(const QString &rawUrl) const {
        const QUrl parsed(rawUrl.trimmed());
        if (!parsed.isValid() || parsed.isEmpty()) {
            return false;
        }
        const QString scheme = parsed.scheme().toLower();
        return scheme == "http" || scheme == "https";
    }

    QString resolveResultUrl(const QUrl &baseUrl, const QString &rawHref) const {
        QString trimmed = rawHref.trimmed();
        if (trimmed.isEmpty()) {
            return QString();
        }
        if (trimmed.startsWith("//")) {
            trimmed.prepend(baseUrl.scheme() + ":");
        }
        QUrl resolved = baseUrl.resolved(QUrl(trimmed));
        if (!resolved.isValid() || resolved.isEmpty()) {
            return QString();
        }

        // Unwrap DuckDuckGo redirect links (/l/?uddg=...) when present
        if (resolved.host().contains("duckduckgo.com", Qt::CaseInsensitive) &&
            resolved.path().startsWith("/l/")) {
            const QUrlQuery query(resolved);
            const QString uddg = query.queryItemValue("uddg", QUrl::FullyDecoded);
            if (!uddg.isEmpty()) {
                resolved = QUrl(uddg);
            }
        }

        const QString scheme = resolved.scheme().toLower();
        if (scheme != "http" && scheme != "https") {
            return QString();
        }
        return resolved.toString();
    }

public:
    explicit WebSearchTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "web_search"; }
    QString description() const override {
        return i18n("Search the web - takes a query, returns results with summaries");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;

        QJsonObject queryProp;
        queryProp["type"] = "string";
        queryProp["description"] = i18n("Search query (e.g., 'latest Python 3.12 features')");
        properties["query"] = queryProp;

        QJsonObject numProp;
        numProp["type"] = "integer";
        numProp["description"] = i18n("Number of results (default: 5, max: 10)");
        numProp["default"] = 5;
        properties["num_results"] = numProp;

        schema["properties"] = properties;
        schema["required"] = QJsonArray{"query"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString query = args["query"].toString().trimmed();
        int numResults = qBound(1, args["num_results"].toInt(5), 10);
        
        if (query.isEmpty()) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("Empty search query")}
            };
        }

        // Use DuckDuckGo HTML scraping (no API key required)
        QNetworkAccessManager manager;
        QString encodedQuery = QUrl::toPercentEncoding(query);
        const QUrl searchUrl(QString("https://html.duckduckgo.com/html/?q=%1").arg(encodedQuery));
        
        QNetworkRequest request(searchUrl);
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36");
        request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QEventLoop loop;
        QNetworkReply *reply = manager.get(request);

        QByteArray buffer;
        bool sizeExceeded = false;
        bool timedOut = false;
        
        QObject::connect(reply, &QNetworkReply::readyRead, reply, [&]() {
            if (sizeExceeded) {
                return;
            }
            const QVariant lenHeader = reply->header(QNetworkRequest::ContentLengthHeader);
            if (lenHeader.isValid() && lenHeader.toLongLong() > kMaxDownloadBytes) {
                sizeExceeded = true;
                reply->abort();
                return;
            }
            const QByteArray chunk = reply->readAll();
            if (buffer.size() + chunk.size() > kMaxDownloadBytes) {
                sizeExceeded = true;
                reply->abort();
                return;
            }
            buffer.append(chunk);
        });

        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, [&]() {
            timedOut = true;
            reply->abort();
            loop.quit();
        });

        timeoutTimer.start(30000);
        if (!reply->isFinished()) {
            loop.exec();
        }
        timeoutTimer.stop();

        if (!sizeExceeded) {
            const QByteArray remaining = reply->readAll();
            if (buffer.size() + remaining.size() > kMaxDownloadBytes) {
                sizeExceeded = true;
            } else {
                buffer.append(remaining);
            }
        }

        const QNetworkReply::NetworkError netErr = reply->error();
        const QString errorMsg = reply->errorString();
        reply->deleteLater();

        if (sizeExceeded) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("Search response exceeded maximum allowed size")}
            };
        }

        if (timedOut) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("Search request timed out")}
            };
        }

        if (netErr != QNetworkReply::NoError) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("Network error: %1", errorMsg)}
            };
        }

        const QString html = QString::fromUtf8(buffer);
        QStringList results;

        const QRegularExpression linkRegex(
            "<a\\b[^>]*\\bclass=[\"'][^\"']*result__a[^\"']*[\"'][^>]*\\bhref=[\"']([^\"']+)[\"'][^>]*>(.*?)</a>",
            QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
        const QRegularExpression snippetRegex(
            "<(?:a|td|div|span)\\b[^>]*\\bclass=[\"'][^\"']*result__snippet[^\"']*[\"'][^>]*>(.*?)</(?:a|td|div|span)>",
            QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);

        QStringList snippets;
        QRegularExpressionMatchIterator snipIt = snippetRegex.globalMatch(html);
        while (snipIt.hasNext()) {
            snippets.append(extractTextFromHtml(snipIt.next().captured(1)));
        }

        QRegularExpressionMatchIterator linkIt = linkRegex.globalMatch(html);
        int count = 0;
        int snippetIndex = 0;
        QString summaryText;

        while (linkIt.hasNext() && count < numResults) {
            const QRegularExpressionMatch match = linkIt.next();
            const QString resolvedUrl = resolveResultUrl(searchUrl, match.captured(1));
            const QString title = extractTextFromHtml(match.captured(2));

            if (resolvedUrl.isEmpty() || title.isEmpty()) {
                continue;
            }

            const QString snippet = (snippetIndex < snippets.size())
                ? snippets.at(snippetIndex++)
                : QString();

            const QString entry = snippet.isEmpty()
                ? QString("[%1] %2\n  URL: %3").arg(count + 1).arg(title).arg(resolvedUrl)
                : QString("[%1] %2\n  URL: %3\n  %4").arg(count + 1).arg(title).arg(resolvedUrl).arg(snippet);

            results.append(entry);
            summaryText += entry + "\n\n";
            count++;
        }

        if (results.isEmpty()) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("No results found")},
                {"results", QJsonArray{}}
            };
        }

        if (summaryText.length() > kMaxSummaryChars) {
            summaryText = summaryText.left(kMaxSummaryChars) + "\n... (truncated)";
        }

        QJsonArray resultsArray;
        for (const QString &entry : results) {
            resultsArray.append(entry);
        }

        return QJsonObject{
            {"success", true},
            {"query", query},
            {"results", resultsArray},
            {"summary", summaryText},
            {"count", count}
        };
    }

    bool requiresPermission() const override { return true; }
};

#endif // WEBSearchTOOL_H
