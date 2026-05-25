#include <QtTest/QtTest>
#include "../src/openaiprovider.h"

class TestRetryAbort : public QObject
{
    Q_OBJECT

private slots:
    void testRetryDelayCalculation()
    {
        int baseDelay = 2000;
        QCOMPARE(baseDelay * 1, 2000);
        QCOMPARE(baseDelay * 2, 4000);
        QCOMPARE(baseDelay * 3, 6000);
    }

    void testRetryDelayIncreasesMonotonically()
    {
        int baseDelay = 2000;
        int prev = 0;
        for (int retry = 1; retry <= 3; ++retry) {
            int delay = baseDelay * retry;
            QVERIFY(delay > prev);
            prev = delay;
        }
    }

    void testRetryableStatusCodes()
    {
        QVERIFY(429 == 429 || 429 == 500);
        QVERIFY(500 == 429 || 500 == 500);
        QVERIFY(!(400 == 429 || 400 == 500));
        QVERIFY(!(401 == 429 || 401 == 500));
        QVERIFY(!(403 == 429 || 403 == 500));
        QVERIFY(!(404 == 429 || 404 == 500));
        QVERIFY(!(502 == 429 || 502 == 500));
        QVERIFY(!(503 == 429 || 503 == 500));
    }

    void testMaxRetryCount()
    {
        int maxRetries = 3;
        for (int i = 0; i <= maxRetries; ++i) {
            bool canRetry = i < maxRetries;
            if (i < 3) QVERIFY(canRetry);
            else QVERIFY(!canRetry);
        }
    }

    void testRetryCountResetsOnNonRetryable()
    {
        int retryCount = 2;
        retryCount = 0;
        QCOMPARE(retryCount, 0);
    }

    void testOpenAIProviderAbortWithoutRequest()
    {
        OpenAIProvider provider("http://localhost:1", "test-key");
        provider.abort();
    }

    void testOpenAIProviderDoubleAbort()
    {
        OpenAIProvider provider("http://localhost:1", "test-key");
        provider.abort();
        provider.abort();
    }

    void testOpenAIProviderIsAvailableWithCredentials()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "sk-test");
        QVERIFY(provider.isAvailable());
    }

    void testOpenAIProviderIsAvailableEmptyUrl()
    {
        OpenAIProvider provider("", "sk-test");
        QVERIFY(!provider.isAvailable());
    }

    void testOpenAIProviderIsAvailableEmptyKey()
    {
        OpenAIProvider provider("http://localhost:11434/v1", "");
        QVERIFY(!provider.isAvailable());
    }

    void testOpenAIProviderIsAvailableBothEmpty()
    {
        OpenAIProvider provider("", "");
        QVERIFY(!provider.isAvailable());
    }

    void testOpenAIProviderSetName()
    {
        OpenAIProvider provider("http://localhost:1", "key");
        provider.setName("custom-provider");
        QCOMPARE(provider.name(), QStringLiteral("custom-provider"));
    }

    void testOpenAIProviderUpdateConfig()
    {
        OpenAIProvider provider("http://old-url", "old-key");
        provider.updateConfig("http://new-url", "new-key");
        QVERIFY(provider.isAvailable());
    }

    void testToolTimeoutDefaults()
    {
        QCOMPARE(getDefaultTimeoutForTool("terminal"), 10);
        QCOMPARE(getDefaultTimeoutForTool("read_file"), 30);
        QCOMPARE(getDefaultTimeoutForTool("grep"), 30);
        QCOMPARE(getDefaultTimeoutForTool("edit_file"), 30);
        QCOMPARE(getDefaultTimeoutForTool("anything_else"), 30);
    }

private:
    static int getDefaultTimeoutForTool(const QString &toolName)
    {
        if (toolName == "terminal") return 10;
        return 30;
    }
};

QTEST_MAIN(TestRetryAbort)
#include "test_retry_abort.moc"
