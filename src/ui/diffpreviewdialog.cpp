#include "diffpreviewdialog.h"
#include <QFileDialog>
#include <QDir>
#include <QApplication>
#include <QEvent>

DiffPreviewDialog::DiffPreviewDialog(const QString &filePath,
                                     const QString &original,
                                     const QString &modified,
                                     QWidget *parent)
    : QDialog(parent)
    , m_originalLines(original)
    , m_modifiedLines(modified)
{
    setWindowTitle(QString("Diff Preview: %1").arg(QDir::toNativeSeparators(filePath)));
    setMinimumWidth(800);
    setMinimumHeight(400);

    setupUi();

    m_diffHtmlOriginal = computeSimpleDiff(original, modified);
    m_diffHtmlModified = computeSimpleDiff(original, modified);

    applyDiffFormatting(m_originalEdit, m_diffHtmlOriginal, "original");
    applyDiffFormatting(m_modifiedEdit, m_diffHtmlModified, "modified");

    connect(m_originalScroll, &QScrollBar::valueChanged, this, &DiffPreviewDialog::onScrollValueChanged);
    connect(m_modifiedScroll, &QScrollBar::valueChanged, this, &DiffPreviewDialog::onScrollValueChanged);
    connect(m_acceptButton, &QPushButton::clicked, this, &DiffPreviewDialog::onAcceptClicked);
    connect(m_rejectButton, &QPushButton::clicked, this, &DiffPreviewDialog::onRejectClicked);
    connect(m_acceptAllButton, &QPushButton::clicked, this, &DiffPreviewDialog::onAcceptAllClicked);
}

void DiffPreviewDialog::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);

    m_headerLabel = new QLabel();
    m_headerLabel->setObjectName("diffPreviewHeader");
    m_headerLabel->setText(QString("Original: %1 lines").arg(m_originalLines.count('\n') + 1));
    m_mainLayout->addWidget(m_headerLabel);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    m_originalEdit = new QTextEdit();
    m_originalEdit->setReadOnly(true);
    m_originalEdit->setHtml(m_diffHtmlOriginal);
    m_originalScroll = m_originalEdit->verticalScrollBar();

    m_modifiedEdit = new QTextEdit();
    m_modifiedEdit->setReadOnly(true);
    m_modifiedEdit->setHtml(m_diffHtmlModified);
    m_modifiedScroll = m_modifiedEdit->verticalScrollBar();

    splitter->addWidget(m_originalEdit);
    splitter->addWidget(m_modifiedEdit);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    m_mainLayout->addWidget(splitter, 1);

    m_buttonsLayout = new QHBoxLayout();
    m_buttonsLayout->addStretch();

    m_acceptButton = new QPushButton("Accept ✓");
    m_acceptButton->setObjectName("diffActionButton");

    m_rejectButton = new QPushButton("Reject");
    m_rejectButton->setObjectName("diffActionButton");

    m_acceptAllButton = new QPushButton("Accept All");
    m_acceptAllButton->setObjectName("diffActionButton");

    m_buttonsLayout->addWidget(m_acceptButton);
    m_buttonsLayout->addWidget(m_rejectButton, 1);
    m_buttonsLayout->addWidget(m_acceptAllButton);
    m_buttonsLayout->addStretch();

    m_mainLayout->addLayout(m_buttonsLayout);
}

void DiffPreviewDialog::applyDiffFormatting(QTextEdit *edit, const QString &html, const QString &type)
{
    Q_UNUSED(edit)
    Q_UNUSED(html)
    Q_UNUSED(type)
    // Use native KDE styling - no custom formatting
}

QString DiffPreviewDialog::computeSimpleDiff(const QString &original, const QString &modified)
{
    QStringList originalLines = original.split('\n', Qt::SkipEmptyParts);
    QStringList modifiedLines = modified.split('\n', Qt::SkipEmptyParts);

    QString html;

    for (int i = 0; i < qMin(originalLines.size(), modifiedLines.size()); ++i) {
        if (originalLines[i] == modifiedLines[i]) {
            html += QString("<div><span style='background: #666; color: #ddd;'>✓</span></div>")
                   .arg(i + 1)
                   .arg(originalLines[i]);
        } else if (originalLines[i] != modifiedLines[i]) {
            QString originalLine = originalLines[i].isEmpty() ? "&nbsp;" : originalLines[i];
            QString modifiedLine = modifiedLines[i].isEmpty() ? "&nbsp;" : modifiedLines[i];
            
            html += QString("<div style='padding: 1px 4px; background: #ffebee; color: #c62828; '>&nbsp;</div>");
            html += QString("<div style='padding: 1px 4px; border-bottom: 1px solid #eee;'>%1</div>")
                   .arg(originalLine);

            html += QString("<div style='padding: 1px 4px; background: #e8f5e9; color: #2e7d32; '>&nbsp;</div>");
            html += QString("<div style='padding: 1px 4px; border-bottom: 1px solid #eee;'>%1</div>")
                   .arg(modifiedLine);
        }
    }

    if (originalLines.size() < modifiedLines.size()) {
        for (int i = originalLines.size(); i < modifiedLines.size(); ++i) {
            QString line = modifiedLines[i].isEmpty() ? "&nbsp;" : modifiedLines[i];
            html += QString("<div style='padding: 1px 4px; background: #e8f5e9; color: #2e7d32; '>&nbsp;</div>");
            html += QString("<div style='padding: 1px 4px; border-bottom: 1px solid #eee;'>%1</div>")
                   .arg(line);
        }
    }

    if (originalLines.size() > modifiedLines.size()) {
        for (int i = modifiedLines.size(); i < originalLines.size(); ++i) {
            QString line = originalLines[i].isEmpty() ? "&nbsp;" : originalLines[i];
            html += QString("<div style='padding: 1px 4px; background: #ffebee; color: #c62828; '>&nbsp;</div>");
            html += QString("<div style='padding: 1px 4px; border-bottom: 1px solid #eee;'>%1</div>")
                   .arg(line);
        }
    }

    html += QString("<div style='margin-top: 5px; padding-top: 5px; border-top: solid #ddd; '>&nbsp;</div>");

    return html;
}

void DiffPreviewDialog::onScrollValueChanged(int value)
{
    Q_UNUSED(value)
    if (m_originalScroll->value() != m_modifiedScroll->value()) {
        m_modifiedScroll->setValue(m_originalScroll->value());
    }
}

void DiffPreviewDialog::onAcceptClicked()
{
    accept();
}

void DiffPreviewDialog::onRejectClicked()
{
    reject();
}

void DiffPreviewDialog::onAcceptAllClicked()
{
    accept();
}
