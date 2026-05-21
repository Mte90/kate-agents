#include <QtTest/QtTest>
#include <QObject>
#include <QSignalSpy>
#include <QTimer>

class MockThreadView : public QObject
{
    Q_OBJECT
public:
    MockThreadView() : m_messageCount(0) {}
    
    void appendUserMessage(const QString &content) {
        m_messages.append(content);
        m_messageCount++;
    }
    
    void showStreamingChunk(const QString &chunk) {
        m_streamingContent += chunk;
    }
    
    void endStreaming() {
        if (!m_streamingContent.isEmpty()) {
            m_messages.append(m_streamingContent);
            m_streamingContent.clear();
            m_messageCount++;
        }
    }
    
    int messageCount() const { return m_messageCount; }
    QStringList messages() const { return m_messages; }
    QString lastMessage() const { return m_messages.last(); }
    
private:
    QStringList m_messages;
    QString m_streamingContent;
    int m_messageCount;
};

class MockAgentLoop : public QObject
{
    Q_OBJECT
public:
    MockAgentLoop(QObject *parent = nullptr) : QObject(parent) {}
    
    void simulateTurn(const QString &response) {
        QStringList chunks = response.split(' ', Qt::SkipEmptyParts);
        for (const QString &chunk : chunks) {
            emit responseChunk(chunk + " ");
        }
        emit turnCompleted("test-thread");
    }
    
signals:
    void responseChunk(const QString &chunk);
    void turnCompleted(const QString &threadId);
};

class TestChatUIFlow : public QObject
{
    Q_OBJECT
    
private slots:
    void testUserMessageAppearsInUI()
    {
        MockThreadView mockView;
        MockAgentLoop mockAgent;
        
        QString userMessage = "Ciao, come stai?";
        
        mockView.appendUserMessage(userMessage);
        
        QCOMPARE(mockView.messageCount(), 1);
        QCOMPARE(mockView.lastMessage(), userMessage);
    }
    
    void testAIMessageStreamingAppearsInUI()
    {
        MockThreadView mockView;
        MockAgentLoop mockAgent;
        
        QString aiResponse = "Ciao! Come posso aiutarti oggi?";
        
        QObject::connect(&mockAgent, &MockAgentLoop::responseChunk, 
            [&mockView](const QString &chunk) {
                mockView.showStreamingChunk(chunk);
            });
        
        QObject::connect(&mockAgent, &MockAgentLoop::turnCompleted,
            [&mockView]() {
                mockView.endStreaming();
            });
        
        mockAgent.simulateTurn(aiResponse);
        
        QCOMPARE(mockView.messageCount(), 1);
        QVERIFY(mockView.lastMessage().contains("Ciao"));
        QVERIFY(mockView.lastMessage().contains("aiutarti"));
    }
    
    void testMultipleMessagesPersist()
    {
        MockThreadView mockView;
        
        mockView.appendUserMessage("Messaggio 1");
        mockView.appendUserMessage("Messaggio 2");
        mockView.appendUserMessage("Messaggio 3");
        
        QCOMPARE(mockView.messageCount(), 3);
        QCOMPARE(mockView.messages()[0], QString("Messaggio 1"));
        QCOMPARE(mockView.messages()[1], QString("Messaggio 2"));
        QCOMPARE(mockView.messages()[2], QString("Messaggio 3"));
    }
    
    void testStreamingChunksConcatenateCorrectly()
    {
        MockThreadView mockView;
        
        mockView.showStreamingChunk("Ciao");
        mockView.showStreamingChunk(" ");
        mockView.showStreamingChunk("mondo");
        mockView.endStreaming();
        
        QCOMPARE(mockView.lastMessage(), QString("Ciao mondo"));
    }
    
    void testEmptyChunksAreHandled()
    {
        MockThreadView mockView;
        
        mockView.showStreamingChunk("");
        mockView.showStreamingChunk("Test");
        mockView.showStreamingChunk("");
        mockView.endStreaming();
        
        QCOMPARE(mockView.messageCount(), 1);
        QCOMPARE(mockView.lastMessage(), QString("Test"));
    }
};

QTEST_MAIN(TestChatUIFlow)
#include "test_chatuiflow.moc"
