#include <QtTest/QtTest>
#include "../src/ghosttextprovider.h"

class TestGhostTextProvider : public QObject
{
    Q_OBJECT

private slots:

    void testSetSuggestion()
    {
        GhostTextProvider provider;
        
        provider.setSuggestion("test suggestion", 5, 10);
        QVERIFY(provider.m_hasSuggestion == true);
        QVERIFY(provider.m_ghostText == "test suggestion");
        QVERIFY(provider.m_suggestionLine == 5);
        QVERIFY(provider.m_suggestionColumn == 10);
    }

    void testSetSuggestionSameValue()
    {
        GhostTextProvider provider;
        
        provider.setSuggestion("same", 1, 1);
        provider.setSuggestion("same", 1, 1);
        QVERIFY(provider.m_hasSuggestion == true);
    }

    void testClearSuggestion()
    {
        GhostTextProvider provider;
        
        provider.setSuggestion("text", 0, 0);
        QVERIFY(provider.m_hasSuggestion == true);
        
        provider.clearSuggestion();
        QVERIFY(provider.m_hasSuggestion == false);
        QVERIFY(provider.m_ghostText.isEmpty());
    }

    void testSuggestionPosition()
    {
        GhostTextProvider provider;
        
        provider.setSuggestion("test", 3, 15);
        auto pos = provider.suggestionPosition();
        QVERIFY(pos.line() == 3);
        QVERIFY(pos.column() == 15);
    }

    void testInlineNotes()
    {
        GhostTextProvider provider;
        
        provider.setSuggestion("test", 2, 5);
        
        QList<int> notes = provider.inlineNotes(2);
        QVERIFY(notes.contains(5));
        
        QList<int> notesWrongLine = provider.inlineNotes(1);
        QVERIFY(notesWrongLine.isEmpty());
    }

    void testInlineNoteSizeEmpty()
    {
        GhostTextProvider provider;
        
        QSize size = provider.inlineNoteSize(nullptr);
        QVERIFY(size.isEmpty());
    }

    void testInlineNoteSizeWithText()
    {
        GhostTextProvider provider;
        
        provider.setSuggestion("test", 0, 0);
        
        QFont font;
        QFontMetrics fm(font);
        
        struct MockNote {
            const QFont& font() const { return font; }
        } mockNote;
        
        QSize size = provider.inlineNoteSize(mockNote);
        QVERIFY(size.width() > 0);
        QVERIFY(size.height() > 0);
    }
};

QTEST_MAIN(TestGhostTextProvider)
#include "test_ghosttextprovider.moc"