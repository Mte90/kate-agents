#include "inputbar.h"
#include "filementionpopup.h"
#include "agentloop.h"
#include <KLocalizedString>
#include <QEvent>
#include <QKeyEvent>
#include <QDir>
#include <QMetaEnum>
#include <QAbstractTextDocumentLayout>
#include <QProcess>

InputBar::InputBar(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumWidth(0);
    setMinimumHeight(0);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    m_inputEdit = new QTextEdit(this);
    m_inputEdit->setPlaceholderText(i18n("Type a message to the agent... (@ for tools)"));
    m_inputEdit->setAcceptRichText(false);
    m_inputEdit->setMinimumWidth(0);
    m_inputEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_inputEdit, &QTextEdit::textChanged, this, &InputBar::onTextChanged);
    mainLayout->addWidget(m_inputEdit, 1);
    
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(4);
    
    m_modelCombo = new QComboBox(this);
    m_modelCombo->setMinimumWidth(120);
    m_modelCombo->setToolTip(i18n("Select the model to use for chat"));
    bottomRow->addWidget(m_modelCombo);
    connect(m_modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { 
                if (m_modelCombo->currentIndex() >= 0) {
                    emit modelChanged(m_modelCombo->currentText()); 
                }
            });
    
    m_profileCombo = new QComboBox(this);
    m_profileCombo->addItem(i18n("Write"));
    m_profileCombo->addItem(i18n("Ask"));
    m_profileCombo->addItem(i18n("Minimal"));
    connect(m_profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &InputBar::onProfileChanged);
    bottomRow->addWidget(m_profileCombo);
    
    bottomRow->addStretch();
    
    m_sendButton = new QPushButton(i18n("Invia"), this);
    connect(m_sendButton, &QPushButton::clicked, this, &InputBar::onSendClicked);
    bottomRow->addWidget(m_sendButton);
    
    mainLayout->addLayout(bottomRow);
    
    // Initialize FileMentionPopup
    m_filePopup = new FileMentionPopup(this);
    connect(m_filePopup, &FileMentionPopup::fileSelected, this, &InputBar::insertFilePath);
    m_filePopup->setInputEdit(m_inputEdit);
    
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
        QProcess gitProcess;
        gitProcess.setWorkingDirectory(projectDir.absolutePath());
        
        gitProcess.start(QStringLiteral("git"), {QStringLiteral("ls-files"), QStringLiteral("--cached"), QStringLiteral("--others"), QStringLiteral("--exclude-standard")});
        if (gitProcess.waitForFinished(3000) && gitProcess.exitCode() == 0) {
            QString output = QString::fromUtf8(gitProcess.readAllStandardOutput());
            QStringList gitFiles = output.split(QChar('\n'), Qt::SkipEmptyParts);
            gitFiles.sort();
            for (const QString &gf : gitFiles) {
                if (!gf.startsWith(QChar('.'))) {
                    files.append(gf);
                }
            }
        } else {
            findFilesRecursive(projectDir, QString(), files, 2);
        }
        if (files.count() > 200) {
            files = files.mid(0, 200);
        }
        items.append(files);
    }
    
    QStringList filtered;
    for (const QString &item : items) {
        if (item.contains(filterText, Qt::CaseInsensitive)) {
            filtered.append(item);
        }
    }
    filtered.sort();
    
    m_filePopup->m_filteredPaths = filtered;
    m_filePopup->m_model->setStringList(filtered);
    
    if (!filtered.isEmpty()) {
        int inputHeight = m_inputEdit->height();
        int inputY = m_inputEdit->mapToGlobal(QPoint(0, inputHeight)).y();
        int inputX = m_inputEdit->mapToGlobal(QPoint(0, 0)).x();
        
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
    
    if (!dir.exists() || !dir.isReadable()) {
        return;
    }
    
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QSet<QString> seenPaths;
    
    for (const QFileInfo &fileInfo : fileInfoList) {
        QString absPath = fileInfo.absoluteFilePath();
        
        if (seenPaths.contains(absPath)) {
            continue;
        }
        
        if (!QFile::exists(absPath)) {
            continue;
        }
        
        if (!fileInfo.isFile() && !fileInfo.isDir()) {
            continue;
        }
        
        if (fileInfo.fileName().startsWith('.')) {
            continue;
        }
        
        seenPaths.insert(absPath);
        
        QString path = prefix.isEmpty() ? fileInfo.fileName() : prefix + "/" + fileInfo.fileName();
        result.append(path);
        
        if (fileInfo.isDir() && depth > 1) {
            QDir subDir(absPath);
            if (subDir.exists() && subDir.isReadable()) {
                findFilesRecursive(subDir, path, result, depth - 1);
            }
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
        m_inputEdit->setFocus();
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

QSize InputBar::minimumSizeHint() const
{
    int editMinHeight = m_inputEdit->fontMetrics().lineSpacing() * 3;
    int bottomRowAndMargins = 55;
    return QSize(0, editMinHeight + bottomRowAndMargins);
}

QSize InputBar::sizeHint() const
{
    int contentHeight = m_inputEdit->document()->documentLayout()->documentSize().height();
    int bottomRowAndMargins = 55;
    int totalHeight = contentHeight + bottomRowAndMargins;
    if (totalHeight > 250) {
        totalHeight = 250;
    }
    QSize min = minimumSizeHint();
    if (totalHeight < min.height()) {
        totalHeight = min.height();
    }
    return QSize(0, totalHeight);
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
        } else if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() & Qt::ControlModifier) {
            // Ctrl+Enter pressed - send message
            onSendClicked();
            return true; // Event handled
        }
    }
    return QWidget::eventFilter(obj, event);
}
