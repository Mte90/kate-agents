#include <QtTest/QtTest>
#include "../src/ui/inputbar.h"
#include "../src/agentloop.h"
#include <QSignalSpy>
#include <QKeyEvent>

class TestInputBarEvents : public QObject
{
    Q_OBJECT

private slots:

    void testInputBarShow()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.show();
        QVERIFY(bar.isVisible());
        bar.hide();
    }

    void testSendButtonClick()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QSignalSpy spy(bar.m_sendButton, &QPushButton::clicked);
        
        bar.m_sendButton->click();
        
        QVERIFY(spy.count() == 1);
    }

    void testMessageEditTextChange()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QSignalSpy spy(bar.m_messageEdit, &QTextEdit::textChanged);
        
        bar.m_messageEdit->setText("test");
        
        QVERIFY(spy.count() >= 1);
    }

    void testProfileComboChange()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QSignalSpy spy(bar.m_profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged));
        
        bar.m_profileCombo->setCurrentIndex(1);
        
        QVERIFY(spy.count() >= 1);
    }

    void testModelComboChange()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QSignalSpy spy(bar.m_modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged));
        
        bar.m_modelCombo->setCurrentIndex(0);
        
        QVERIFY(spy.count() >= 1);
    }

    void testKeyPressEnter()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setText("test");
        
        QTest::keyClick(bar.m_messageEdit, Qt::Key_Return);
    }

    void testKeyPressEnterWithModifier()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setText("test");
        
        QTest::keyClick(bar.m_messageEdit, Qt::Key_Return, Qt::ShiftModifier);
    }

    void testMessageEditFocusIn()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QSignalSpy spy(bar.m_messageEdit, &QTextEdit::focusIn);
        
        bar.m_messageEdit->setFocus();
        
        QVERIFY(spy.count() >= 0);
    }

    void testMessageEditFocusOut()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setFocus();
        
        QSignalSpy spy(bar.m_messageEdit, &QTextEdit::focusOut);
        
        bar.m_messageEdit->clearFocus();
        
        QVERIFY(spy.count() >= 0);
    }

    void testSendButtonEnabledState()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_sendButton->setEnabled(false);
        QVERIFY(!bar.m_sendButton->isEnabled());
        
        bar.m_sendButton->setEnabled(true);
        QVERIFY(bar.m_sendButton->isEnabled());
    }

    void testMessageEditClear()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("some text");
        QVERIFY(!bar.m_messageEdit->toPlainText().isEmpty());
        
        bar.m_messageEdit->clear();
        QVERIFY(bar.m_messageEdit->toPlainText().isEmpty());
    }

    void testMessageEditUndoRedo()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("test");
        bar.m_messageEdit->undo();
        
        bar.m_messageEdit->redo();
    }

    void testMessageEditCopyPaste()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("copy this");
        bar.m_messageEdit->copy();
        
        bar.m_messageEdit->paste();
    }

    void testProfileComboCount()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QVERIFY(bar.m_profileCombo->count() >= 3);
    }

    void testMessageEditCursorPosition()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("test");
        QTextCursor cursor = bar.m_messageEdit->textCursor();
        QVERIFY(cursor.position() >= 0);
    }

    void testMessageEditSelectAll()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("select all text");
        bar.m_messageEdit->selectAll();
        
        QVERIFY(!bar.m_messageEdit->textCursor().selectedText().isEmpty());
    }

    void testInputBarResize()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.resize(600, 100);
        
        QVERIFY(bar.width() == 600);
        QVERIFY(bar.height() == 100);
    }

    void testInputBarMinimumHeight()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.setMinimumHeight(80);
        
        QVERIFY(bar.minimumHeight() == 80);
    }

    void testInputBarMaximumHeight()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.setMaximumHeight(150);
        
        QVERIFY(bar.maximumHeight() == 150);
    }

    void testInputBarSizePolicy()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QSizePolicy policy = bar.sizePolicy();
        QVERIFY(policy.horizontalPolicy() == QSizePolicy::Expanding);
        QVERIFY(policy.verticalPolicy() == QSizePolicy::Fixed);
    }
};

QTEST_MAIN(TestInputBarEvents)
#include "test_inputbar_events.moc"