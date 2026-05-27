#include "filementionpopup.h"
#include <QTextEdit>

FileMentionPopup::FileMentionPopup(QWidget *parent)
    : QWidget(parent)
    , m_listView(nullptr)
    , m_model(nullptr)
{
    setupUI();
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
}

FileMentionPopup::~FileMentionPopup()
{
    if (m_model) {
        delete m_model;
    }
    if (m_listView) {
        delete m_listView;
    }
}

void FileMentionPopup::populateFromDirectory(const QString &projectDir)
{
    m_allPaths.clear();
    m_filteredPaths.clear();

    if (!QDir(projectDir).exists()) {
        return;
    }

    addAllRecursively(QDir(projectDir), 0);
    resizePopup();
    m_model->setStringList(m_allPaths);
}

void FileMentionPopup::addAllRecursively(const QDir &dir, int depth)
{
    if (m_allPaths.count() >= 500 || depth > 3) {
        return;
    }

    if (!dir.exists() || !dir.isReadable()) {
        return;
    }

    QStringList ignorePatterns;
    QFile gitignore(dir.filePath(".gitignore"));
    if (gitignore.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&gitignore);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty() && !line.startsWith('#')) {
                ignorePatterns.append(line);
            }
        }
        gitignore.close();
    }

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
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
        m_allPaths << absPath;
        
        if (fileInfo.isDir()) {
            QDir subDir(absPath);
            if (subDir.exists() && subDir.isReadable()) {
                addAllRecursively(subDir, depth + 1);
            }
        }
    }
}

void FileMentionPopup::addTools(const QStringList &tools)
{
    m_tools = tools;
    // Re-apply current filter to include tools
    if (!m_filteredPaths.isEmpty()) {
        filterPaths(m_filteredPaths.first().split('@').last());
    }
}

void FileMentionPopup::showAt(const QPoint &pos)
{
    if (!m_listView || !m_model) {
        return;
    }

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setParent(nullptr);
    setGeometry(pos.x(), pos.y(), m_listView->width(), m_listView->height());
    show();
}

void FileMentionPopup::hidePopup()
{
    QWidget::hide();
}

void FileMentionPopup::setInputEdit(QTextEdit *edit)
{
    m_inputEdit = edit;
}

void FileMentionPopup::setupUI()
{
    m_listView = new QListView(this);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setContextMenuPolicy(Qt::NoContextMenu);
    m_listView->setDragEnabled(false);
    m_listView->setUniformItemSizes(true);

    m_model = new QStringListModel(this);
    m_model->setStringList(QStringList());
    m_listView->setModel(m_model);

    connect(m_listView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &FileMentionPopup::onCurrentChanged);
connect(m_listView, &QListView::clicked, this, [this]() {
    if (!m_model || !m_listView->currentIndex().isValid()) {
        return;
    }
    emit fileSelected(m_filteredPaths[m_listView->currentIndex().row()]);
    hidePopup();
});}

void FileMentionPopup::onCurrentChanged(const QModelIndex &current)
{
    if (current.isValid()) {
        m_listView->setCurrentIndex(current);
    }
}

void FileMentionPopup::resizePopup()
{
    if (!m_listView || !m_model) {
        return;
    }

    int maxWidth = qMin(500, m_listView->width());
    int rowHeight = m_listView->fontMetrics().height() + 6;
    int visibleRowCount = qMin(m_model->rowCount(), 20);

    int totalHeight = m_listView->frameWidth() +
                     (visibleRowCount > 0 ? rowHeight * visibleRowCount : 30) + 10;

    setFixedSize(maxWidth, totalHeight);
}

void FileMentionPopup::filterPaths(const QString &filterText)
{
    if (!m_model || filterText.isEmpty()) {
        return;
    }
    QStringList filtered;
    for (const QString &path : m_allPaths) {
        if (path.contains(filterText, Qt::CaseInsensitive)) {
            filtered.append(path);
        }
    }
    m_filteredPaths = filtered;
    m_model->setStringList(m_filteredPaths);
}

void FileMentionPopup::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (m_listView->currentIndex().isValid()) {
            emit fileSelected(m_filteredPaths[m_listView->currentIndex().row()]);
            hidePopup();
            return;
        }
    } else if (event->key() == Qt::Key_Escape) {
        hidePopup();
        return;
    } else if (event->key() == Qt::Key_Up) {
        if (m_listView->selectionModel()) {
            QModelIndex current = m_listView->selectionModel()->currentIndex();
            if (current.isValid() && current.row() > 0) {
                m_listView->selectionModel()->setCurrentIndex(
                    m_listView->model()->index(current.row() - 1, 0),
                    QItemSelectionModel::SelectCurrent
                );
            } else if (!current.isValid() && m_model->rowCount() > 0) {
                m_listView->selectionModel()->setCurrentIndex(
                    m_model->index(0, 0),
                    QItemSelectionModel::SelectCurrent
                );
            }
        }
        return;
    } else if (event->key() == Qt::Key_Down) {
        if (m_listView->selectionModel()) {
            QModelIndex current = m_listView->selectionModel()->currentIndex();
            if (current.isValid() && current.row() < m_model->rowCount() - 1) {
                m_listView->selectionModel()->setCurrentIndex(
                    m_listView->model()->index(current.row() + 1, 0),
                    QItemSelectionModel::SelectCurrent
                );
            }
        }
        return;
    }
    
    
    if (m_inputEdit) {
        QKeyEvent keyEvent(QEvent::KeyPress, event->key(), event->modifiers(), event->text(), event->isAutoRepeat(), event->count());
        QCoreApplication::sendEvent(m_inputEdit, &keyEvent);
    }
}
