#ifndef DIFFPREVIEWDIALOG_H
#define DIFFPREVIEWDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollBar>
#include <QSplitter>

class DiffPreviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiffPreviewDialog(const QString &filePath,
                               const QString &original,
                               const QString &modified,
                               QWidget *parent = nullptr);
    ~DiffPreviewDialog() override = default;

signals:
    void accepted();
    void rejected();

private slots:
    void onScrollValueChanged(int value);
    void onAcceptClicked();
    void onRejectClicked();
    void onAcceptAllClicked();

private:
    void setupUi();
    void applyDiffFormatting(QTextEdit *edit, const QString &text, const QString &type);
    QString computeSimpleDiff(const QString &original, const QString &modified);
    QString escapeHtml(const QString &text) const;

    QString m_originalLines;
    QString m_modifiedLines;
    QString m_diffHtmlOriginal;
    QString m_diffHtmlModified;

    QTextEdit *m_originalEdit;
    QTextEdit *m_modifiedEdit;
    QScrollBar *m_originalScroll;
    QScrollBar *m_modifiedScroll;

    QLabel *m_headerLabel;
    QPushButton *m_acceptButton;
    QPushButton *m_rejectButton;
    QPushButton *m_acceptAllButton;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonsLayout;
};

#endif // DIFFPREVIEWDIALOG_H
