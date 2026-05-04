#ifndef EDITCONFIRMATIONDIALOG_H
#define EDITCONFIRMATIONDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QList>
#include <QPushButton>
#include <QTextEdit>
#include <QStackedWidget>

class DiffPreviewDialog;

struct EditOperation {
    QString filePath;
    QString original;
    QString modified;
    EditOperation(const QString &f, const QString &o, const QString &m)
        : filePath(f), original(o), modified(m) {}
};

class EditConfirmationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditConfirmationDialog(const QList<EditOperation> &edits, QWidget *parent = nullptr);
    ~EditConfirmationDialog() override = default;

signals:
    void applySelected(const QList<int> &indices);
    void applyAll();
    void rejected();

private slots:
    void onFileClicked(int row);
    void onApplySelected();
    void onApplyAll();
    void onCancel();
    void onCheckStateChanged(int row, bool checked);

private:
    void setupUi();
    void populateTreeWidget();
    void onTreeCurrentChanged(int current, int previous);

    QList<EditOperation> m_edits;
    QTreeWidget *m_treeWidget;
    QStackedWidget *m_stack;
    QTextEdit *m_detailsTextEdit;
    QPushButton *m_applySelectionBtn;
    QPushButton *m_applyAllBtn;
    QPushButton *m_cancelBtn;
};

#endif
