#ifndef INPUTBAR_H
#define INPUTBAR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QString>
#include <QLabel>

class FileMentionPopup;
class AgentLoop;

class InputBar : public QWidget
{
    Q_OBJECT

public:
    explicit InputBar(QWidget *parent = nullptr);
    ~InputBar() override;

    QString text() const { return m_inputEdit->text(); }
    void clear() { m_inputEdit->clear(); }
    void setEnabled(bool enabled) { m_inputEdit->setEnabled(enabled); m_sendButton->setEnabled(enabled); }
    
    void setModels(const QStringList &models);
    QString currentModel() const { return m_modelCombo->currentText(); }
    void setRunningState(bool running);
    void setSystemPrompt(const QString &prompt);
    void setAgentLoop(AgentLoop *agentLoop) { m_agentLoop = agentLoop; }

signals:
    void sendMessage(const QString &message);
    void stopRequested();
    void modelChanged(const QString &model);
    void systemPromptChanged(const QString &prompt);

private slots:
    void onSendClicked();
    void onReturnPressed();
    void onTextChanged(const QString &text);
    void insertFilePath(const QString &filePath);
    void onInputTextChanged(const QString &text);
    void onProfileChanged(int index);

private:
    bool eventFilter(QObject *obj, QEvent *event) override;
    QLineEdit *m_inputEdit;
    QPushButton *m_sendButton;
    QComboBox *m_modelCombo;
    QComboBox *m_profileCombo;
    QLabel *m_statusLabel;
    FileMentionPopup *m_filePopup = nullptr;
    AgentLoop *m_agentLoop = nullptr;
    bool m_isRunning = false;
    QString m_systemPrompt;
};

#endif
