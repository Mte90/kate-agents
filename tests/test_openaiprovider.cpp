#include <QtTest/QtTest>
#include "../src/openaiprovider.h"

class TestOpenAIProvider : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "test-key");
        QVERIFY(provider.m_baseUrl == "http://localhost:11434/v1");
        QVERIFY(provider.m_apiKey == "test-key");
    }

    void testIsAvailable()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "test-key");
        QVERIFY(provider.isAvailable() == true);
    }

    void testIsNotAvailableEmptyUrl()
    {
        OpenAIProvider provider("", "test-key");
        QVERIFY(provider.isAvailable() == false);
    }

    void testIsNotAvailableEmptyKey()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "");
        QVERIFY(provider.isAvailable() == false);
    }

    void testName()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "test-key");
        QVERIFY(!provider.name().isEmpty());
    }

    void testSetBaseUrl()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "test-key");
        provider.setBaseUrl("http://new-url.com/v1");
        QVERIFY(provider.m_baseUrl == "http://new-url.com/v1");
    }

    void testSetApiKey()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "test-key");
        provider.setApiKey("new-key");
        QVERIFY(provider.m_apiKey == "new-key");
    }

    void testAvailableModelsDefault()
    {
        OpenAIProvider provider("", "");
        QStringList models = provider.availableModels();
        QVERIFY(!models.isEmpty());
    }
};

QTEST_MAIN(TestOpenAIProvider)
#include "test_openaiprovider.moc"