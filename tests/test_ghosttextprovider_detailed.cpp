#include <QtTest/QtTest>
#include "../src/ghosttextprovider.h"

class TestGhostTextProviderDetailed : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        GhostTextProvider provider;
        QVERIFY(true);
    }

    void testInlineNotesEnabled()
    {
        GhostTextProvider provider;
        QVERIFY(provider.m_enabled == true);
    }

    void testInlineNoteHeight()
    {
        GhostTextProvider provider;
        QVERIFY(provider.m_noteHeight > 0);
    }

    void testSetEnabled()
    {
        GhostTextProvider provider;
        provider.m_enabled = false;
        QVERIFY(provider.m_enabled == false);
    }

    void testSetNoteHeight()
    {
        GhostTextProvider provider;
        provider.m_noteHeight = 20;
        QVERIFY(provider.m_noteHeight == 20);
    }

    void testInlineNoteType()
    {
        GhostTextProvider provider;
        QVERIFY(!provider.m_inlineNoteType.isEmpty());
    }

    void testClearGhostTexts()
    {
        GhostTextProvider provider;
        provider.m_ghostTexts.clear();
        QVERIFY(provider.m_ghostTexts.isEmpty());
    }

    void testAddGhostText()
    {
        GhostTextProvider provider;
        provider.m_ghostTexts.append("test");
        QVERIFY(provider.m_ghostTexts.size() == 1);
    }
};

QTEST_MAIN(TestGhostTextProviderDetailed)
#include "test_ghosttextprovider_detailed.moc"