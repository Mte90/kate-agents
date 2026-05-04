#include "inputbar.h"
#include "filementionpopup.h"
#include "agentloop.h"
#include <QEvent>
#include <QKeyEvent>
#include <QDir>
#include <QMetaEnum>

InputBar::InputBar(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    m_modelCombo = new QComboBox(this);
    m_modelCombo->setMinimumWidth(150);
    m_modelCombo->addItem("Write");
    m_modelCombo->addItem("Ask");
    m_modelCombo->addItem("Minimal");
    connect(m_modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { emit modelChanged(m_modelCombo->currentText()); });
    
    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setPlaceholderText("Scrivi un messaggio all'agente... (@ per tool)");
    m_inputEdit->setMinimumHeight(36);
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &InputBar::onReturnPressed);
    connect(m_inputEdit, &QLineEdit::textChanged, this, &InputBar::onTextChanged);
    
    m_sendButton = new QPushButton("Invia", this);
    m_sendButton->setMinimumWidth(80);
    m_sendButton->setMinimumHeight(36);
    connect(m_sendButton, &QPushButton::clicked, this, &InputBar::onSendClicked);
    
    m_statusLabel = new QLabel(this);
    m_statusLabel->setMinimumWidth(60);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    
    mainLayout->addWidget(m_modelCombo);
    mainLayout->addWidget(m_inputEdit, 1);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addWidget(m_sendButton);
    
    m_profileCombo = new QComboBox(this);
    m_profileCombo->setMinimumWidth(120);
    m_profileCombo->addItem("Write");
    m_profileCombo->addItem("Ask");
    m_profileCombo->addItem("Minimal");
    connect(m_profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &InputBar::onProfileChanged);
    mainLayout->addWidget(m_profileCombo);
    
    setStyleSheet(R"(
        InputBar {
            background: #2d2d2d;
            border-top: 1px solid #424242;
        }
        QLineEdit {
            background: #1e1e1e;
            color: #ffffff;
            border: 1px solid #424242;
            border-radius: 4px;
            padding: 6px;
            font-size: 13px;
        }
        QLineEdit:focus {
            border: 1px solid #2196f3;
        }
        QPushButton {
            background: #1976d2;
            color: white;
            border: none;
            border-radius: 4px;
            font-weight: bold;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background: #1e88e5;
        }
        QPushButton:pressed {
            background: #1565c0;
        }
        QComboBox {
            background: #1e1e1e;
            color: #ffffff;
            border: 1px solid #424242;
            border-radius: 4px;
            padding: 4px;
        }
    )");
    
    // Initialize FileMentionPopup
    m_filePopup = new FileMentionPopup(this);
    connect(m_filePopup, &FileMentionPopup::fileSelected, this, &InputBar::insertFilePath);
    
    // Install event filter for Tab key handling
    m_inputEdit->installEventFilter(this);
    
// Set default system prompt based on selected profile
    QString profile = m_profileCombo->currentText();
    AgentProfile profileEnum = stringToProfile(profile);
    QString prompt = systemPromptForProfile(profileEnum);
    m_systemPrompt = prompt;
    emit systemPromptChanged(m_systemPrompt);
}InputBar::~InputBar() = default;

void InputBar::setModels(const QStringList &models)
{
    QString current = m_modelCombo->currentText();
    m_modelCombo->clear();
    m_modelCombo->addItems(models);
    if (models.contains(current)) {
        m_modelCombo->setCurrentText(current);
    }
}

void InputBar::onSendClicked()
{
    QString text = m_inputEdit->text().trimmed();
    if (!text.isEmpty()) {
        emit sendMessage(text);
    }
}

void InputBar::onReturnPressed()
{
    if (!m_isRunning) {
        onSendClicked();
    }
}

void InputBar::onTextChanged(const QString &text)
{
    m_sendButton->setEnabled(!text.trimmed().isEmpty());
}

void InputBar::insertFilePath(const QString &filePath)
{
    QString text = m_inputEdit->text();
    int atIndex = text.lastIndexOf('@');
    
    if (atIndex >= 0) {
        QString before = text.left(atIndex);
        QString after = text.mid(atIndex + 1);
        
        int endPos = 0;
        while (endPos < after.length() && !after[endPos].isSpace()) {
            endPos++;
        }
        
        after = after.mid(endPos);
        QString newText = before + filePath + " " + after;
        
        m_inputEdit->setText(newText);
        m_inputEdit->setCursorPosition(atIndex + filePath.length() + 1);
        m_filePopup->hide();
    }
}

void InputBar::setRunningState(bool running)
{
    m_isRunning = running;
    if (running) {
        m_sendButton->setText("⬛ Stop");
        m_inputEdit->setEnabled(false);
        m_statusLabel->setText("🔄");
    } else {
        m_sendButton->setText("Invia");
        m_inputEdit->setEnabled(true);
        m_inputEdit->setFocus();
        m_statusLabel->setText("✅");
        m_filePopup->hidePopup();
    }
}

void InputBar::onInputTextChanged(const QString &text)
{
    if (!text.contains('@') || text.mid(text.indexOf('@') + 1).isEmpty()) {
        m_filePopup->hidePopup();
        return;
    }

    m_filePopup->populateFromDirectory("/home/archimede/Desktop/projects/kate-agents");
    m_filePopup->filterPaths(text.mid(text.indexOf('@') + 1));

    int cursorPos = m_inputEdit->cursorPosition();
    QPoint pos = m_inputEdit->mapToGlobal(QPoint(0, m_inputEdit->height()));
    m_filePopup->showAt(pos);
}

void InputBar::onProfileChanged(int index)
{
    QString profile = m_profileCombo->currentText();
    AgentProfile profileEnum = stringToProfile(profile);
    QString prompt = systemPromptForProfile(profileEnum);
    m_systemPrompt = prompt;
    emit systemPromptChanged(m_systemPrompt);
}

bool InputBar::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_inputEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab) {
            // Tab pressed - accept ghost text if available
            if (m_agentLoop && m_agentLoop->hasGhostText()) {
                m_agentLoop->acceptGhostText();
                return true; // Event handled
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
