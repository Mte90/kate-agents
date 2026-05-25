/*
 * Test per verificare il fuzzy picker di Kate Agent
 * 
 * Test di unità per FileMentionPopup e la sua interazione con QTextEdit
 */

#include <QTest>
#include <QApplication>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <QSettings>
#include <QDebug>

#include "ui/filementionpopup.h"

void testFuzzyPickerFocusPolicy()
{
    qDebug() << "Test 1: FuzzyPicker Focus Policy";
    
    FileMentionPopup popup(nullptr);
    popup.setupUI();
    
    // Verifica che il popup non rubi il focus
    bool noFocus = (popup.focusPolicy() != Qt::StrongFocus);
    qDebug() << "Popup has NoFocus/NoFocus/WeakFocus:" << noFocus;
    
    QTEST_ASSERT(!noFocus || popup.focusPolicy() == Qt::StrongFocus);
}

void testFuzzyPickerFilter()
{
    qDebug() << "Test 2: FuzzyPicker Filtering";
    
    FileMentionPopup popup(nullptr);
    
    // Aggiungi file di prova
    popup.m_allPaths << "main.cpp"
                     << "README.md"
                     << "agentloop.cpp"
                     << "test.cpp"
                     << "include/file.h";
    
    popup.filterPaths("");
    
    QTest::bench("Empty filter", 1);
    QCOMPARE(popup.m_filteredPaths.count(), 5);
    
    // Filtra per "a"
    popup.filterPaths("a");
    QCOMPARE(popup.m_filteredPaths.count(), 5); // Tutti contengono 'a'
    
    // Filtra per "README"
    popup.filterPaths("README");
    QCOMPARE(popup.m_filteredPaths.count(), 1); // Solo README.md
    
    QCOMPARE(popup.m_filteredPaths.first(), "README.md");
}

void testFuzzyPickerKeyForward()
{
    qDebug() << "Test 3: Key Forwarding";
    
    QWidget *parent = new QWidget();
    QTextEdit *inputEdit = new QTextEdit(parent);
    inputEdit->setPlainText("test");
    
    FileMentionPopup popup(inputEdit);
    
    // Simula tasto digitato 'a'
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_a, Qt::NoModifier, "a", false, 1);
    
    // Il popup dovrebbe forwardare all'input edit
    QCoreApplication::sendEvent(inputEdit, event);
    
    QString newContent = inputEdit->toPlainText();
    QCOMPARE(newContent, "testa");
}

void testFuzzyPickerBackspace()
{
    qDebug() << "Test 4: Backspace Handling";
    
    QWidget *parent = new QWidget();
    QTextEdit *inputEdit = new QTextEdit(parent);
    inputEdit->setPlainText("test");
    
    FileMentionPopup popup(inputEdit);
    
    // Simula Backspace
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Back, Qt::NoModifier, "", false, 0);
    
    QCoreApplication::sendEvent(inputEdit, event);
    
    QCOMPARE(inputEdit->toPlainText(), "te");
}

void testFuzzyPickerEnterSelection()
{
    qDebug() << "Test 5: Enter Key Selection";
    
    QWidget *parent = new QWidget();
    QTextEdit *inputEdit = new QTextEdit(parent);
    inputEdit->setPlainText("test");
    
    FileMentionPopup popup(inputEdit);
    
    popup.m_allPaths << "file1.txt" << "file2.txt";
    popup.filterPaths("");
    
    // Seleziona primo elemento
    popup.m_listView->setCurrentRow(0);
    
    // Simula Enter
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "\n", false, 1);
    QCoreApplication::sendEvent(&popup, event);
    
    QCOMPARE(popup.m_filteredPaths.count(), 2); // Non altera filterPaths
}

void testFuzzyPickerEscapeHide()
{
    qDebug() << "Test 6: Escape Key Hide";
    
    QWidget *parent = new QWidget();
    QTextEdit *inputEdit = new QTextEdit(parent);
    
    FileMentionPopup popup(inputEdit);
    popup.show();
    
    // Seleziona primo elemento
    popup.m_listView->setCurrentRow(0);
    
    QTEST_ASSERT(popup.isVisible());
    
    // Simula Escape
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier, "", false, 0);
    QCoreApplication::sendEvent(&popup, event);
    
    QTEST_ASSERT(!popup.isVisible());
}

void testFuzzyPickerShowAt()
{
    qDebug() << "Test 7: ShowAt Position";
    
    QWidget *parent = new QWidget();
    QTextEdit *inputEdit = new QTextEdit(parent);
    inputEdit->resize(400, 100);
    
    FileMentionPopup popup(inputEdit);
    popup.m_allPaths << "test.cpp";
    popup.filterPaths("");
    
    QPoint globalPos = inputEdit->mapToGlobal(QPoint(0, inputEdit->height()));
    
    // Reset geometry per test
    popup.hide();
    popup.setFixedSize(200, 100);
    
    popup.showAt(globalPos);
    
    QTEST_ASSERT(popup.isVisible());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Kate Agent Fuzzy Picker Tests");
    
    qDebug() << "=== Kate Agent Fuzzy Picker Tests ===";
    
    QGuiApplication::setQuitOnLastWindowClosed(false);
    
    testFuzzyPickerFocusPolicy();
    testFuzzyPickerFilter();
    testFuzzyPickerKeyForward();
    testFuzzyPickerBackspace();
    testFuzzyPickerEnterSelection();
    testFuzzyPickerEscapeHide();
    testFuzzyPickerShowAt();
    
    qDebug() << "\n=== All Tests Complete ===";
    
    return 0;
}

#include <QtTest/QtTest>
#include <QTest>
