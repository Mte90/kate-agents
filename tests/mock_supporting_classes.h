#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QTimer>
#include <QDebug>

// Mock PermissionManager
class MockPermissionManager : public QObject {
    Q_OBJECT
public:
    enum PermissionBehavior {
        AutoGrant,      // Automaticamente concede tutte le permission
        AutoDeny,       // Automaticamente nega tutte le permission
        Manual          // Richiede intervento del test
    };
    
    explicit MockPermissionManager(QObject *parent = nullptr) : QObject(parent) {
        m_behavior = Manual;
        m_grantCount = 0;
        m_denyCount = 0;
    }
    
    void setBehavior(PermissionBehavior behavior) { m_behavior = behavior; }
    PermissionBehavior behavior() const { return m_behavior; }
    
    int grantCount() const { return m_grantCount; }
    int denyCount() const { return m_denyCount; }
    
    // Simula requestPermission - ritorna subito se AutoGrant/AutoDeny
    bool requestPermission(const QString &toolName) {
        qDebug() << "Permission richiesta per tool:" << toolName;
        
        if (m_behavior == AutoGrant) {
            m_grantCount++;
            return true;
        } else if (m_behavior == AutoDeny) {
            m_denyCount++;
            return false;
        }
        
        // Manual mode - il test deve chiamare grantPermission/denyPermission
        return false;
    }
    
public slots:
    void grantPermission(const QString &toolName) {
        qDebug() << "Permission concessa per tool:" << toolName;
        m_grantCount++;
        emit permissionGranted(toolName);
    }
    
    void denyPermission(const QString &toolName) {
        qDebug() << "Permission negata per tool:" << toolName;
        m_denyCount++;
        emit permissionDenied(toolName);
    }

signals:
    void permissionGranted(const QString &toolName);
    void permissionDenied(const QString &toolName);
    
private:
    PermissionBehavior m_behavior;
    int m_grantCount;
    int m_denyCount;
};

// Mock LLMProvider
class MockLLMProvider : public QObject {
    Q_OBJECT
public:
    explicit MockLLMProvider(QObject *parent = nullptr) : QObject(parent) {
        m_responseDelay = 0;
        m_streamingEnabled = false;
        m_chunkDelay = 50;  // 50ms tra chunk
        m_errorOnNextCall = false;
        m_callCount = 0;
    }
    
    int callCount() const { return m_callCount; }
    void setResponseDelay(int ms) { m_responseDelay = ms; }
    void setStreamingEnabled(bool enabled) { m_streamingEnabled = enabled; }
    void setChunkDelay(int ms) { m_chunkDelay = ms; }
    
    void setErrorOnNextCall() { m_errorOnNextCall = true; }

signals:
    void responseStarted(const QString &threadId);
    void chunkReceived(const QString &threadId, const QString &chunk, bool isThinking);
    void responseCompleted(const QString &threadId, const QString &fullResponse);
    void error(const QString &error);
    
public slots:
    void chatStream(const QString &threadId, const QString &prompt) {
        m_callCount++;
        
        if (m_errorOnNextCall) {
            m_errorOnNextCall = false;
            emit error("Mock error on purpose");
            return;
        }
        
        emit responseStarted(threadId);
        
        QString response = "Mock response for: " + prompt;
        
        if (m_responseDelay > 0) {
            QTimer::singleShot(m_responseDelay, this, [this, threadId, response]() {
                if (m_streamingEnabled) {
                    // Simula streaming
                    QStringList words = response.split(" ");
                    for (const QString &word : words) {
                        QTimer::singleShot(m_chunkDelay, this, [this, threadId, word]() {
                            emit chunkReceived(threadId, word + " ", false);
                        });
                    }
                    
                    QTimer::singleShot(words.size() * m_chunkDelay + 100, this, [this, threadId, response]() {
                        emit responseCompleted(threadId, response);
                    });
                } else {
                    // Simula risposta completa
                    QTimer::singleShot(100, this, [this, threadId, response]() {
                        emit responseCompleted(threadId, response);
                    });
                }
            });
        } else {
            if (m_streamingEnabled) {
                QStringList words = response.split(" ");
                for (const QString &word : words) {
                    emit chunkReceived(threadId, word + " ", false);
                }
                emit responseCompleted(threadId, response);
            } else {
                emit responseCompleted(threadId, response);
            }
        }
    }
    
private:
    int m_responseDelay;
    bool m_streamingEnabled;
    int m_chunkDelay;
    bool m_errorOnNextCall;
    int m_callCount;
};

// Mock ThreadStorage
class MockThreadStorage : public QObject {
    Q_OBJECT
public:
    struct ConversationThread {
        QString id;
        QString title;
        QString currentModel;
        QList<QJsonObject> messages;
    };
    
    explicit MockThreadStorage(QObject *parent = nullptr) : QObject(parent) {
        m_saveCount = 0;
        m_loadCount = 0;
        m_deleteCount = 0;
    }
    
    int saveCount() const { return m_saveCount; }
    int loadCount() const { return m_loadCount; }
    int deleteCount() const { return m_deleteCount; }
    
    bool saveThread(const ConversationThread &thread) {
        m_saveCount++;
        m_threads[thread.id] = thread;
        qDebug() << "Thread salvato:" << thread.id;
        return true;
    }
    
    QMap<QString, ConversationThread> loadAllThreads() {
        m_loadCount++;
        qDebug() << "Thread caricati:" << m_threads.size();
        return m_threads;
    }
    
    ConversationThread loadThread(const QString &threadId) {
        m_loadCount++;
        return m_threads.value(threadId);
    }
    
    bool deleteThread(const QString &threadId) {
        m_deleteCount++;
        m_threads.remove(threadId);
        qDebug() << "Thread eliminato:" << threadId;
        return true;
    }
    
private:
    QMap<QString, ConversationThread> m_threads;
    int m_saveCount;
    int m_loadCount;
    int m_deleteCount;
};

// Mock ConfigManager
class MockConfigManager : public QObject {
    Q_OBJECT
public:
    explicit MockConfigManager(QObject *parent = nullptr) : QObject(parent) {
        m_defaultModel = "gpt-4";
        m_enableStreaming = true;
        m_enableCodebaseIndexing = false;
    }
    
    QString defaultModel() const { return m_defaultModel; }
    bool enableStreaming() const { return m_enableStreaming; }
    bool enableCodebaseIndexing() const { return m_enableCodebaseIndexing; }
    
    void setDefaultModel(const QString &model) { m_defaultModel = model; }
    void setEnableStreaming(bool enable) { m_enableStreaming = enable; }
    void setEnableCodebaseIndexing(bool enable) { m_enableCodebaseIndexing = enable; }

signals:
    void configChanged();
    
private:
    QString m_defaultModel;
    bool m_enableStreaming;
    bool m_enableCodebaseIndexing;
};
