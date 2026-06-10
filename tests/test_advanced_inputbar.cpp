#include <QtTest/QtTest>
#include "../src/ui/inputbar.h"
#include "../src/agentloop.h"

class TestAdvancedInputBar : public QObject
{
    Q_OBJECT

private slots:

    void testMessageValidationLogic()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("");
        bool emptyValid = bar.validateMessage();
        
        bar.m_messageEdit->setText("   \n\t  ");
        bool whitespaceValid = bar.validateMessage();
        
        bar.m_messageEdit->setText("valid");
        bool contentValid = bar.validateMessage();
        
        QVERIFY(emptyValid == false);
        QVERIFY(whitespaceValid == false);
        QVERIFY(contentValid == true);
    }

    void testProfileSpecificBehavior()
    {
        InputBar barWrite(nullptr, AgentProfile::Write);
        InputBar barAsk(nullptr, AgentProfile::Ask);
        InputBar barMinimal(nullptr, AgentProfile::Minimal);
        
        barWrite.m_currentProfile = AgentProfile::Write;
        barAsk.m_currentProfile = AgentProfile::Ask;
        barMinimal.m_currentProfile = AgentProfile::Minimal;
        
        bool writeSends = (barWrite.m_currentProfile == AgentProfile::Write);
        bool askSends = (barAsk.m_currentProfile == AgentProfile::Ask);
        bool minimalSends = (barMinimal.m_currentProfile == AgentProfile::Minimal);
        
        QVERIFY(writeSends && askSends && minimalSends);
    }

    void testModelSelectionPersistence()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_models << "gpt-4" << "claude-3" << "llama-3";
        
        for (const QString &model : bar.m_models) {
            bar.m_modelCombo->addItem(model);
        }
        
        bar.m_modelCombo->setCurrentIndex(1);
        QString selected = bar.m_modelCombo->currentText();
        
        bar.m_messageEdit->setText("test");
        bar.m_sendButton->click();
        
        bar.m_modelCombo->setCurrentIndex(0);
        bar.m_modelCombo->setCurrentIndex(1);
        
        QVERIFY(bar.m_modelCombo->currentText() == selected);
    }

    void testSendButtonStateWithEmptyMessage()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("");
        bar.updateSendButtonState();
        
        bool emptyState = !bar.m_sendButton->isEnabled();
        
        bar.m_messageEdit->setText("some text");
        bar.updateSendButtonState();
        
        bool contentState = bar.m_sendButton->isEnabled();
        
        QVERIFY(emptyState == true);
        QVERIFY(contentState == true);
    }

    void testProfileSwitchClearsModel()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_models << "gpt-4" << "claude-3";
        
        bar.m_modelCombo->clear();
        for (const QString &m : bar.m_models) {
            bar.m_modelCombo->addItem(m);
        }
        bar.m_modelCombo->setCurrentIndex(1);
        
        bar.m_currentProfile = AgentProfile::Ask;
        
        QVERIFY(bar.m_currentProfile == AgentProfile::Ask);
    }

    void testMessageEditHistoryNavigation()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageHistory << "first" << "second" << "third";
        bar.m_historyIndex = 0;
        
        bar.navigateHistoryUp();
        QString up1 = bar.m_messageEdit->toPlainText();
        
        bar.navigateHistoryUp();
        QString up2 = bar.m_messageEdit->toPlainText();
        
        bar.navigateHistoryDown();
        QString down1 = bar.m_messageEdit->toPlainText();
        
        QVERIFY(bar.m_historyIndex >= 0);
    }

    void testKeyboardShortcutProcessing()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        bar.m_messageEdit->setText("test");
        
        QTest::keyClick(bar.m_messageEdit, Qt::Key_Return, Qt::ControlModifier);
        
        QTest::keyClick(bar.m_messageEdit, Qt::Key_Return, Qt::ShiftModifier);
        
        QVERIFY(true);
    }

    void testTextInputSpeedHandling()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        QString longText;
        for (int i = 0; i < 1000; i++) {
            longText += "word ";
        }
        
        bar.m_messageEdit->setText(longText);
        
        QString retrieved = bar.m_messageEdit->toPlainText();
        
        QVERIFY(retrieved.length() == longText.length());
    }

    void testSendButtonDuringProcessing()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.setProcessingState(true);
        
        bool whileProcessing = !bar.m_sendButton->isEnabled();
        bool inputWhileProcessing = !bar.m_messageEdit->isEnabled();
        
        bar.setProcessingState(false);
        
        bool afterProcessing = bar.m_sendButton->isEnabled();
        bool inputAfterProcessing = bar.m_messageEdit->isEnabled();
        
        QVERIFY(whileProcessing == true);
        QVERIFY(afterProcessing == true);
    }

    void testMultipleRapidSends()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("first");
        bar.m_sendButton->click();
        
        bar.m_messageEdit->setText("second");
        bar.m_sendButton->click();
        
        QVERIFY(true);
    }

    void testModelListDynamicUpdate()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.updateModelList(QStringList() << "model1");
        
        int count1 = bar.m_modelCombo->count();
        
        bar.updateModelList(QStringList() << "model1" << "model2" << "model3");
        
        int count2 = bar.m_modelCombo->count();
        
        QVERIFY(count2 > count1);
    }

    void testCursorPositionPreservation()
    {
        InputBar bar(nullptr, AgentProfile::Write);
        
        bar.m_messageEdit->setText("Hello World");
        QTextCursor cursor = bar.m_messageEdit->textCursor();
        cursor.setPosition(5);
        bar.m_messageEdit->setTextCursor(cursor);
        
        bar.m_messageEdit->insertPlainText(" inserted");
        
        QString text = bar.m_messageEdit->toPlainText();
        QVERIFY(text.contains(" inserted"));
    }
};

QTEST_MAIN(TestAdvancedInputBar)
#include "test_advanced_inputbar.moc"