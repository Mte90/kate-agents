#include "agent-worker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QString>
#include <QDebug>

AgentWorker::AgentWorker(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &AgentWorker::onNetworkReply);
}

void AgentWorker::sendRequest(const QString &endpoint, const QJsonObject &payload)
{
    QUrl url(endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(payload);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    QNetworkReply *reply = m_networkManager->post(request, data);
    m_pendingReplies.insert(reply, payload);
}

void AgentWorker::onNetworkReply(QNetworkReply *reply)
{
    if (m_pendingReplies.contains(reply)) {
        QJsonObject originalPayload = m_pendingReplies.take(reply);
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            emit requestCompleted(originalPayload, responseData, false);
        } else {
            emit requestCompleted(originalPayload, reply->errorString(), true);
        }
    }
    
    reply->deleteLater();
}
