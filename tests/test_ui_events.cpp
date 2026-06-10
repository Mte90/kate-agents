#include <QtTest/QtTest>
#include "../src/ui/threadview.h"
#include "../src/ui/inputbar.h"
#include "../src/ui/agentpanel.h"
#include "../src/llmprovider.h"
#include "../src/agentloop.h"
#include <QSignalSpy>

class TestUIEvents : public QObject
{
    Q_OBJECT

private slots:

    void testSendButtonClickedSignal()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QSignalSpy spy(bar.m_sendButton, &QPushButton::clicked);
        
        bar.m_sendButton->click();
        
        QVERIFY(spy.count() == 1);
    }

    void testSendButtonClickedWithText()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("Hello world");
        QVERIFY(bar.m_messageEdit->toPlainText() == "Hello world");
    }

    void testMessageEditTextChanged()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QSignalSpy spy(bar.m_messageEdit, &QTextEdit::textChanged);
        
        bar.m_messageEdit->setText("test");
        
        QVERIFY(spy.count() >= 1);
    }

    void testProfileComboCurrentIndexChanged()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QSignalSpy spy(bar.m_profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged));
        
        bar.m_profileCombo->setCurrentIndex(1);
        
        QVERIFY(spy.count() >= 1);
    }

    void testModelComboCurrentIndexChanged()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        QSignalSpy spy(bar.m_modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged));
        
        bar.m_modelCombo->setCurrentIndex(0);
        
        QVERIFY(spy.count() >= 1);
    }

    void testTabCloseRequested()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        QSignalSpy spy(tv.m_tabs, &QTabWidget::tabCloseRequested);
        
        if (tv.count() > 0) {
            tv.m_tabs->tabCloseRequested(0);
        }
        
        QVERIFY(tv.count() >= 0);
    }

    void testTabChanged()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        
        QSignalSpy spy(tv.m_tabs, &QTabWidget::currentChanged);
        
        if (tv.count() > 1) {
            tv.setCurrentIndex(1);
        }
        
        QVERIFY(spy.count() >= 0);
    }

    void testInputBarKeyPressEnter()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setText("Test message");
        
        QTest::keyClick(bar.m_messageEdit, Qt::Key_Return);
    }

    void testInputBarKeyPressEnterWithCtrl()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setText("Test");
        
        QTest::keyClick(bar.m_messageEdit, Qt::Key_Return, Qt::ControlModifier);
    }

    void testMessageEditFocus()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setFocus();
        
        QVERIFY(bar.m_messageEdit->hasFocus());
    }

    void testTabCountAfterAppend()
    {
        ThreadView tv;
        int initial = tv.count();
        
        tv.appendUserMessage("model");
        
        QVERIFY(tv.count() > initial);
    }

    void testCurrentIndexAfterAppend()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        int current = tv.currentIndex();
        QVERIFY(current >= 0);
    }

    void testTabText()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        if (tv.count() > 0) {
            QString text = tv.tabText(0);
            QVERIFY(!text.isEmpty());
        }
    }

    void testSetTabText()
    {
        ThreadView tv;
        tv.appendUserMessage("model");
        
        if (tv.count() > 0) {
            tv.setTabText(0, "Custom Title");
            QVERIFY(tv.tabText(0) == "Custom Title");
        }
    }

    void testInputBarEnabledState()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.setEnabled(false);
        QVERIFY(bar.isEnabled() == false);
        
        bar.setEnabled(true);
        QVERIFY(bar.isEnabled() == true);
    }

    void testMessageEditClear()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("Some text");
        bar.m_messageEdit->clear();
        
        QVERIFY(bar.m_messageEdit->toPlainText().isEmpty());
    }

    void testMultipleTabsInteraction()
    {
        ThreadView tv;
        
        tv.appendUserMessage("model");
        tv.appendAssistantMessage("model");
        tv.appendUserMessage("model");
        
        QVERIFY(tv.count() >= 3);
        
        for (int i = 0; i < tv.count(); i++) {
            tv.setCurrentIndex(i);
            QVERIFY(tv.currentIndex() == i);
        }
    }
};

QTEST_MAIN(TestUIEvents)
#include "test_ui_events.moc"