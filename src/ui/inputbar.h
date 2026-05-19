#ifndef INPUTBAR_H
#define INPUTBAR_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QString>
#include <QDir>

class FileMentionPopup;
class AgentLoop;

class InputBar : public QWidget
{
    Q_OBJECT

public:
    explicit InputBar(QWidget *parent = nullptr);
    ~InputBar() override;

    QString text() const { return m_inputEdit->toPlainText(); }
    void clear() { m_inputEdit->clear(); }
    void setEnabled(bool enabled) { m_inputEdit->setEnabled(enabled); m_sendButton->setEnabled(enabled); }
    
    void setModels(const QStringList &models);
    QString currentModel() const { return m_modelCombo->currentText(); }
    void setCurrentModel(const QString &model) { 
        if (m_modelCombo->findText(model) >= 0) {
            m_modelCombo->setCurrentText(model);
        }
    }
    int findModel(const QString &model) { return m_modelCombo->findText(model); }
    void setCurrentModelIndex(int index) { if (index >= 0 && index < m_modelCombo->count()) m_modelCombo->setCurrentIndex(index); }
    void setRunningState(bool running);
    void setSystemPrompt(const QString &prompt);
    QString currentProfile() const { return m_profileCombo->currentText(); }
    QSize minimumSizeHint() const override { return QSize(0, 0); }
    void setAgentLoop(AgentLoop *agentLoop) { m_agentLoop = agentLoop; }

signals:
    void sendMessage(const QString &message);
    void stopRequested();
    void modelChanged(const QString &model);
    void systemPromptChanged(const QString &prompt);

private slots:
    void onSendClicked();
    void onReturnPressed();
    void onTextChanged();
    void insertFilePath(const QString &filePath);
    void onProfileChanged(int index);

private:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void showAutocompletePopup(int atIndex);
    void findFilesRecursive(const QDir &dir, const QString &prefix, QStringList &result, int depth);
    
    QTextEdit *m_inputEdit;
    QPushButton *m_sendButton;
    QComboBox *m_modelCombo;
    QComboBox *m_profileCombo;
    FileMentionPopup *m_filePopup = nullptr;
    AgentLoop *m_agentLoop = nullptr;
    bool m_isRunning = false;
    QString m_systemPrompt;
    QStringList m_availableTools = {"read_file", "edit_file", "grep", "terminal", "web_search", "url_fetch", "diagnostics", "find_path", "list_directory", "create_directory"};
};

#endif
