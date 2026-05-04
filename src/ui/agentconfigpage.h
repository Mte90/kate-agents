#ifndef AGENTCONFIGPAGE_H
#define AGENTCONFIGPAGE_H

#include <KTextEditor/ConfigPage>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QNetworkAccessManager>

class KateAgentPlugin;
class OpenAIProvider;

class AgentConfigPage : public KTextEditor::ConfigPage
{
    Q_OBJECT

public:
    explicit AgentConfigPage(QWidget *parent, KateAgentPlugin *plugin);
    
    QString name() const override;
    QString fullName() const override;
    QIcon icon() const override;
    
    void apply() override;
    void defaults() override;
    void reset() override;
    
private slots:
    void fetchModelList();

private:
    void loadSettings();
    void changed();
    void updateSettings();

    KateAgentPlugin *m_plugin;
    QLineEdit *m_apiKeyEdit;
    QLineEdit *m_baseUrlEdit;
    QComboBox *m_modelComboBox;
    QTextEdit *m_systemPromptEdit;
    QCheckBox *m_bufferContextCheckbox;
    QLabel *m_infoLabel;
    QNetworkAccessManager *m_networkManager;
};

#endif