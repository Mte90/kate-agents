#include <QtTest/QtTest>
#include "../src/openaiprovider.h"

class TestOpenAIProviderDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstructor()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "test-key");
        QVERIFY(provider.m_baseUrl == "http://localhost:11434/v1");
        QVERIFY(provider.m_apiKey == "test-key");
    }

    void testIsAvailableTrue()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "test-key");
        QVERIFY(provider.isAvailable() == true);
    }

    void testIsAvailableFalseEmptyUrl()
    {
        OpenAIProvider provider("", "test-key");
        QVERIFY(provider.isAvailable() == false);
    }

    void testIsAvailableFalseEmptyKey()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "");
        QVERIFY(provider.isAvailable() == false);
    }

    void testNameNotEmpty()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "key");
        QVERIFY(!provider.name().isEmpty());
    }

    void testSetBaseUrl()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "key");
        provider.setBaseUrl("http://new:8080/v1");
        QVERIFY(provider.m_baseUrl == "http://new:8080/v1");
    }

    void testSetApiKey()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "key");
        provider.setApiKey("new-key");
        QVERIFY(provider.m_apiKey == "new-key");
    }

    void testDestructor()
    {
        OpenAIProvider *provider = new OpenAIProvider("http://localhost:11434/v1", "key");
        delete provider;
    }

    void testAvailableModelsReturnsList()
    {
        OpenAIProvider provider("", "");
        QStringList models = provider.availableModels();
        QVERIFY(!models.isEmpty());
    }

    void testNetworkManager()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "key");
        QVERIFY(provider.m_nam != nullptr);
    }
};

QTEST_MAIN(TestOpenAIProviderDetailed)
#include "test_openai_provider_detailed.moc"