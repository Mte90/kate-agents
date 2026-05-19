#include "inputbar.h"
#include "filementionpopup.h"
#include "agentloop.h"
#include <KLocalizedString>
#include <QEvent>
#include <QKeyEvent>
#include <QDir>
#include <QMetaEnum>

InputBar::InputBar(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // Model selector (Ollama models)
    m_modelCombo = new QComboBox(this);
    m_modelCombo->addItem("Loading models...", -1);
    m_modelCombo->setToolTip(i18n("Select the Ollama model to use for chat"));
    mainLayout->addWidget(m_modelCombo);
    connect(m_modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { 
                if (m_modelCombo->currentIndex() >= 0) {
                    emit modelChanged(m_modelCombo->currentText()); 
                }
            });
    
    // Text input area (multiline textarea)
    m_inputEdit = new QTextEdit(this);
    m_inputEdit->setPlaceholderText(i18n("Scrivi un messaggio all'agente... (@ per tool)"));
    m_inputEdit->setMinimumHeight(60);
    m_inputEdit->setAcceptRichText(false);
    connect(m_inputEdit, &QTextEdit::textChanged, this, &InputBar::onTextChanged);
    mainLayout->addWidget(m_inputEdit, 1);
    
    // Bottom row: send button and profile selector
    QHBoxLayout *bottomRow = new QHBoxLayout();
    
    m_sendButton = new QPushButton(i18n("Invia"), this);
    connect(m_sendButton, &QPushButton::clicked, this, &InputBar::onSendClicked);
    bottomRow->addWidget(m_sendButton);
    
    // Profile selector (Write/Ask/Minimal)
    m_profileCombo = new QComboBox(this);
    m_profileCombo->addItem(i18n("Write"));
    m_profileCombo->addItem(i18n("Ask"));
    m_profileCombo->addItem(i18n("Minimal"));
    connect(m_profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &InputBar::onProfileChanged);
    bottomRow->addWidget(m_profileCombo);
    
    mainLayout->addLayout(bottomRow);
    
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
    QString text = m_inputEdit->toPlainText().trimmed();
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

void InputBar::onTextChanged()
{
    m_sendButton->setEnabled(!m_inputEdit->toPlainText().trimmed().isEmpty());
    
    QString text = m_inputEdit->toPlainText();
    QTextCursor cursor = m_inputEdit->textCursor();
    int cursorPos = cursor.position();
    
    int atIndex = text.lastIndexOf('@', cursorPos - 1);
    if (atIndex >= 0 && atIndex < cursorPos - 1) {
        QString afterAt = text.mid(atIndex + 1, cursorPos - atIndex - 1);
        if (!afterAt.contains(' ') && !afterAt.contains('\n')) {
            showAutocompletePopup(atIndex);
            return;
        }
    }
    
    m_filePopup->hide();
}

void InputBar::showAutocompletePopup(int atIndex)
{
    QString text = m_inputEdit->toPlainText();
    QString filterText = text.mid(atIndex + 1);
    
    QStringList items;
    
    for (const QString &tool : m_availableTools) {
        items.append(tool);
    }
    
    QDir projectDir = QDir::current();
    if (projectDir.exists()) {
        QStringList files;
        findFilesRecursive(projectDir, "", files, 2);
        items.append(files);
    }
    
    QStringList filtered;
    for (const QString &item : items) {
        if (item.contains(filterText, Qt::CaseInsensitive)) {
            filtered.append(item);
        }
    }
    
    m_filePopup->m_filteredPaths = filtered;
    m_filePopup->m_model->setStringList(filtered);
    
    if (!filtered.isEmpty()) {
        // Position popup below the input edit, aligned to the left
        int inputHeight = m_inputEdit->height();
        int inputY = m_inputEdit->mapToGlobal(QPoint(0, inputHeight)).y();
        int inputX = m_inputEdit->mapToGlobal(QPoint(0, 0)).x();
        
        // Adjust for font height to position just below
        int yPos = inputY + m_inputEdit->fontMetrics().lineSpacing();
        QPoint globalPos(inputX, yPos);
        
        m_filePopup->showAt(globalPos);
    } else {
        m_filePopup->hide();
    }
}

void InputBar::findFilesRecursive(const QDir &dir, const QString &prefix, QStringList &result, int depth)
{
    if (depth <= 0 || result.count() >= 200) {
        return;
    }
    
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : fileInfoList) {
        if (!fileInfo.exists()) {
            continue;
        }
        
        QString path = prefix.isEmpty() ? fileInfo.fileName() : prefix + "/" + fileInfo.fileName();
        result.append(path);
        
        if (fileInfo.isDir() && depth > 1) {
            findFilesRecursive(fileInfo.dir(), path, result, depth - 1);
        }
    }
}

void InputBar::insertFilePath(const QString &filePath)
{
    QString text = m_inputEdit->toPlainText();
    int atIndex = text.lastIndexOf('@');
    
    if (atIndex >= 0) {
        QString before = text.left(atIndex);
        QString after = text.mid(atIndex + 1);
        
        int endPos = 0;
        while (endPos < after.length() && !after[endPos].isSpace()) {
            endPos++;
        }
        
        after = after.mid(endPos);
        QString newText = before + "@" + filePath + " " + after;
        
        m_inputEdit->setPlainText(newText);
        QTextCursor cursor = m_inputEdit->textCursor();
        cursor.setPosition(atIndex + 1 + filePath.length() + 1);
        m_inputEdit->setTextCursor(cursor);
        m_filePopup->hidePopup();
    }
}

void InputBar::setRunningState(bool running)
{
    m_isRunning = running;
    if (running) {
        m_sendButton->setText("⬛ Stop");
        m_inputEdit->setEnabled(false);
    } else {
        m_sendButton->setText("Invia");
        m_inputEdit->setEnabled(true);
        m_inputEdit->setFocus();
        m_filePopup->hidePopup();
    }
}

void InputBar::onProfileChanged(int index)
{
    Q_UNUSED(index)
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
