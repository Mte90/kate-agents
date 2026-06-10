#include <QtTest/QtTest>
#include "../src/ui/inputbar.h"

class TestInputBar : public QObject
{
    Q_OBJECT

private slots:

    void testConstruction()
    {
        InputBar inputBar;
        QVERIFY(inputBar.m_inputEdit != nullptr);
        QVERIFY(inputBar.m_modelCombo != nullptr);
        QVERIFY(inputBar.m_profileCombo != nullptr);
        QVERIFY(inputBar.m_sendButton != nullptr);
    }

    void testSetModels()
    {
        InputBar inputBar;
        
        QStringList models = {"gpt-4", "claude-3", "llama-3"};
        inputBar.setModels(models);
        
        QVERIFY(inputBar.m_modelCombo->count() == 3);
        QVERIFY(inputBar.m_modelCombo->itemText(0) == "gpt-4");
    }

    void testSetCurrentModel()
    {
        InputBar inputBar;
        
        inputBar.setModels({"model1", "model2", "model3"});
        inputBar.setCurrentModel("model2");
        
        QVERIFY(inputBar.m_modelCombo->currentText() == "model2");
    }

    void testSetCurrentModelNotExists()
    {
        InputBar inputBar;
        
        inputBar.setModels({"model1", "model2"});
        inputBar.setCurrentModel("nonexistent");
        
        QVERIFY(inputBar.m_modelCombo->currentIndex() == 0);
    }

    void testSetProfile()
    {
        InputBar inputBar;
        
        inputBar.setProfile(AgentProfile::Ask);
        QVERIFY(inputBar.m_profileCombo->currentText() == "Ask");
        
        inputBar.setProfile(AgentProfile::Minimal);
        QVERIFY(inputBar.m_profileCombo->currentText() == "Minimal");
        
        inputBar.setProfile(AgentProfile::Write);
        QVERIFY(inputBar.m_profileCombo->currentText() == "Write");
    }

    void testCurrentProfile()
    {
        InputBar inputBar;
        
        inputBar.m_profileCombo->setCurrentText("Ask");
        QVERIFY(inputBar.currentProfile() == "Ask");
        
        inputBar.m_profileCombo->setCurrentText("Minimal");
        QVERIFY(inputBar.currentProfile() == "Minimal");
    }

    void testCurrentModel()
    {
        InputBar inputBar;
        
        inputBar.m_modelCombo->addItem("test-model");
        QVERIFY(inputBar.currentModel() == "test-model");
    }

    void testInsertFilePath()
    {
        InputBar inputBar;
        
        inputBar.m_inputEdit->setPlainText("Hello ");
        inputBar.insertFilePath("/path/to/file.cpp");
        
        QString text = inputBar.m_inputEdit->toPlainText();
        QVERIFY(text.contains("/path/to/file.cpp"));
    }

    void testSendMessage_data()
    {
        QTest::addColumn<QString>("message");
        
        QTest::addRow("normal") << "Hello world";
        QTest::addRow("empty") << "";
        QTest::addRow("multiline") << "Line 1\nLine 2";
    }

    void testSendMessage()
    {
        QFETCH(QString, message);
        
        InputBar inputBar;
        QSignalSpy spy(&inputBar, &InputBar::sendMessage);
        
        inputBar.m_inputEdit->setPlainText(message);
        inputBar.onSendClicked();
        
        if (!message.isEmpty()) {
            QVERIFY(spy.count() == 1);
        }
    }

    void testPlaceholderText()
    {
        InputBar inputBar;
        QString placeholder = inputBar.m_inputEdit->placeholderText();
        QVERIFY(placeholder.contains("agent"));
    }
};

QTEST_MAIN(TestInputBar)
#include "test_inputbar.moc"