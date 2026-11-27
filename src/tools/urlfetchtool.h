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
#include <QEventLoop>
#include <QHostAddress>
#include <QHostInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTimer>

class URLFetchTool : public AgentTool
{
    Q_OBJECT

private:
    static constexpr qint64 kMaxDownloadBytes = 5*1024 * 1024; // 5 MiB
    static constexpr int kMaxOutputChars = 10000;
    static constexpr int kMaxRedirects = 5;

    bool isBlockedAddress(const QHostAddress &addr) const {
        if (addr.isNull()) {
            return true;
        }

        QHostAddress ipv4 = addr;
        if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
            bool ok = false;
            const quint32 mappedIp = addr.toIPv4Address(&ok);
            if (ok) {
                ipv4 = QHostAddress(mappedIp);
            } else {
                // IPv6 loopback (::1), unspecified (::), link-local (fe80::/10), unique local (fc00::/7)
                if (addr.isLoopback() || addr == QHostAddress::AnyIPv6) {
                    return true;
                }
                const quint8 *bytes = addr.toIPv6Address().c;
                if ((bytes[0] & 0xfe) == 0xfc) { // fc00::/7
                    return true;
                }
                if (bytes[0] == 0xfe && (bytes[1] & 0xc0) == 0x80) { // fe80::/10
                    return true;
                }
                return false;
            }
        }

        const quint32 ip = ipv4.toIPv4Address();
        auto inRange = [ip](quint32 base, quint32 mask) {
            return (ip & mask) == (base & mask);
        };

        return ipv4.isLoopback()                        // 127.0.0.0/8
            || inRange(0x00000000, 0xff000000)          // 0.0.0.0/8
            || inRange(0x0a000000, 0xff000000)          // 10.0.0.0/8
            || inRange(0x64400000, 0xffc00000)          // 100.64.0.0/10 (CGNAT)
            || inRange(0xa9fe0000, 0xffff0000)          // 169.254.0.0/16 (link-local / metadata)
            || inRange(0xac100000, 0xfff00000)          // 172.16.0.0/12
            || inRange(0xc0a80000, 0xffff0000);         // 192.168.0.0/16
    }

    bool validateUrlAndResolve(const QUrl &url, QString *errorOut) const {
        if (!url.isValid() || url.isEmpty() || (url.scheme() != "http" && url.scheme() != "https")) {
            if (errorOut) {
                *errorOut = i18n("Invalid URL or unsupported scheme (http/https only)");
            }
            return false;
        }

        const QString host = url.host().trimmed().toLower();
        if (host.isEmpty() || host == "localhost" || host.endsWith(".localhost")) {
            if (errorOut) {
                *errorOut = i18n("Access to internal or local URLs blocked for security");
            }
            return false;
        }

        QHostAddress literalAddr(host);
        if (!literalAddr.isNull()) {
            if (isBlockedAddress(literalAddr)) {
                if (errorOut) {
                    *errorOut = i18n("Access to internal or local IP addresses blocked for security");
                }
                return false;
            }
            return true;
        }

        const QHostInfo info = QHostInfo::fromName(host);
        if (info.error() != QHostInfo::NoError || info.addresses().isEmpty()) {
            if (errorOut) {
                *errorOut = i18n("Failed to resolve host: %1", host);
            }
            return false;
        }

        for (const QHostAddress &addr : info.addresses()) {
            if (isBlockedAddress(addr)) {
                if (errorOut) {
                    *errorOut = i18n("Resolved address for %1 is blocked for security", host);
                }
                return false;
            }
        }

        return true;
    }

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
        return i18n("Fetch a webpage - takes a URL and returns the text content");
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
        
        // Block illegal URLs
        QUrl currentUrl(urlString);
        QString validationError;
        if (!validateUrlAndResolve(currentUrl, &validationError)) {
            return QJsonObject{
                {"success", false},
                {"error", validationError}
            };
        }

        QNetworkAccessManager manager;
        QByteArray buffer;
        bool sizeExceeded = false;
        int statusCode = 0;
        for (int hop = 0; hop <= kMaxRedirects; ++hop) {
            QNetworkRequest request(currentUrl);
            const QByteArray customVerb = request.attribute(QNetworkRequest::CustomVerbAttribute).toByteArray().toUpper();
            if (!customVerb.isEmpty() && customVerb != "GET") {
                return QJsonObject{
                    {"success", false},
                    {"error", i18n("Only GET requests are allowed")}
                };
}
            request.setHeader(QNetworkRequest::UserAgentHeader,
                "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
            request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

            QEventLoop loop;
            QTimer timeoutTimer;
            timeoutTimer.setSingleShot(true);

            bool timedOut = false;
            QNetworkReply *reply = manager.get(request);

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

            statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            const QVariant redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            const QNetworkReply::NetworkError netErr = reply->error();
            const QString errString = reply->errorString();
            reply->deleteLater();

            if (sizeExceeded) {
                return QJsonObject{
                    {"success", false},
                    {"error", i18n("Response exceeded maximum allowed size")}
                };
            }

            if (timedOut) {
                return QJsonObject{
                    {"success", false},
                    {"error", i18n("Request timed out")}
                };
            }

            if (redirectTarget.isValid()) {
                if (hop == kMaxRedirects) {
                    return QJsonObject{
                        {"success", false},
                        {"error", i18n("Too many redirects")}
                    };
                }
                // Optional: block 307/308 method-preserving redirects outright
                if (statusCode == 307 || statusCode == 308) {
                    return QJsonObject{
                        {"success", false},
                        {"error", i18n("Redirect method not allowed")}
                    };
                }
                QUrl nextUrl = currentUrl.resolved(redirectTarget.toUrl());
                buffer.clear();
                if (!validateUrlAndResolve(nextUrl, &validationError)) {
                    return QJsonObject{
                        {"success", false},
                        {"error", validationError}
                    };
                }
                currentUrl = nextUrl;
                continue;
            }

            if (netErr != QNetworkReply::NoError) {
                return QJsonObject{
                    {"success", false},
                    {"error", i18n("Network error: %1", errString)}
                };
            }

            if (statusCode < 200 || statusCode >= 300) {
                return QJsonObject{
                    {"success", false},
                    {"error", i18n("HTTP %1", statusCode)}
                };
            }

            break;
        }

        QString text = extractTextFromHtml(QString::fromUtf8(buffer));
        if (text.length() > kMaxOutputChars) {
            text = text.left(kMaxOutputChars) + "\n... (truncated, content too long)";
        }

        return QJsonObject{
            {"success", true},
            {"url", currentUrl.toString()},
            {"title", currentUrl.fileName().isEmpty() ? currentUrl.host() : currentUrl.fileName()},
            {"content", text},
            {"length", text.length()}
        };
    }

    bool requiresPermission() const override { return true; }
};

#endif // URLFETCHTOOL_H
