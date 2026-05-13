#ifndef URLFETCHTOOL_H
#define URLFETCHTOOL_H

#include <KLocalizedString>

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QRegularExpression>

class URLFetchTool : public AgentTool
{
    Q_OBJECT

private:
    QString extractTextFromHtml(const QString &html) const {
        QString text = html;
        text.remove(QRegularExpression("<script[^>]*>.*?</script>", QRegularExpression::DotMatchesEverythingOption));
        text.remove(QRegularExpression("<style[^>]*>.*?</style>", QRegularExpression::DotMatchesEverythingOption));
        text.remove(QRegularExpression("<nav[^>]*>.*?</nav>", QRegularExpression::DotMatchesEverythingOption));
        text.remove(QRegularExpression("<header[^>]*>.*?</header>", QRegularExpression::DotMatchesEverythingOption));
        text.remove(QRegularExpression("<footer[^>]*>.*?</footer>", QRegularExpression::DotMatchesEverythingOption));
        text.remove(QRegularExpression("<[^>]+>"));
        text.replace("&nbsp;", " ");
        text.replace("&amp;", "&");
        text.replace("&lt;", "<");
        text.replace("&gt;", ">");
        text.replace("&quot;", "\"");
        text.replace("&#39;", "'");
        text.replace("\n\n", "\n");
        while (text.contains("  ")) {
            text.replace("  ", " ");
        }
        return text.trimmed();
    }

public:
    explicit URLFetchTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "url_fetch"; }
    QString description() const override {
        return i18n("Retrieves and extracts text content from a URL");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject urlProp;
        urlProp["type"] = "string";
        urlProp["description"] = i18n("URL to fetch (http/https)");
        properties["url"] = urlProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"url"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString urlString = args["url"].toString();
        
        if (urlString.isEmpty()) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("Empty URL")}
            };
        }
        
        // Validate URL scheme
        QUrl url(urlString);
        if (!url.isValid() || (url.scheme() != "http" && url.scheme() != "https")) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("Invalid URL or unsupported scheme (http/https only)")}
            };
        }
        
        // Block internal URLs
        QString host = url.host().toLower();
        if (host == "localhost" || host == "127.0.0.1" || host.startsWith("192.168.") || host.startsWith("10.")) {
            return QJsonObject{
                {"success", false},
                {"error", i18n("Access to internal URLs blocked for security")}
            };
        }

        QNetworkAccessManager manager;
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, 
            "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
        
        QEventLoop loop;
        QNetworkReply *reply = manager.get(request);
        
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        
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
                {"error", i18n("Network error: %1").arg(errorMsg)}
            };
        }
        
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode != 200) {
            reply->deleteLater();
            return QJsonObject{
                {"success", false},
                {"error", QString("HTTP %1").arg(statusCode)}
            };
        }
        
        QByteArray rawData = reply->readAll();
        reply->deleteLater();
        
        QString html = QString::fromUtf8(rawData);
        QString text = extractTextFromHtml(html);
        
        // Truncate if too long
        if (text.length() > 10000) {
            text = text.left(10000) + "\n... (truncated, content too long)";
        }
        
        return QJsonObject{
            {"success", true},
            {"url", urlString},
            {"title", url.fileName().isEmpty() ? url.host() : url.fileName()},
            {"content", text},
            {"length", text.length()}
        };
    }

    bool requiresPermission() const override { return false; }
};

#endif // URLFETCHTOOL_H
