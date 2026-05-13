#ifndef FILEMENTIONPOPUP_H
#define FILEMENTIONPOPUP_H

#include <QWidget>
#include <QKeyEvent>
#include <QListView>
#include <QStringListModel>
#include <QDir>

class FileMentionPopup : public QWidget
{
    Q_OBJECT

public:
    explicit FileMentionPopup(QWidget *parent = nullptr);
    ~FileMentionPopup() override;

    void populateFromDirectory(const QString &projectDir);
    void addTools(const QStringList &tools);
    void showAt(const QPoint &pos);
    void hidePopup();

signals:
    void fileSelected(const QString &filePath);

private slots:
    void onCurrentChanged(const QModelIndex &current);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI();
    void addAllRecursively(const QDir &dir, int depth);
    void resizePopup();

public:
    void filterPaths(const QString &filterText);

    QListView *m_listView;
    QStringListModel *m_model;
    QStringList m_allPaths;
    QStringList m_filteredPaths;
    QStringList m_tools;
};

#endif // FILEMENTIONPOPUP_H
