#include <QTest>
#include <QApplication>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QDebug>

#include "ui/filementionpopup.h"

void testFuzzyFilterCaseInsensitive()
{
    FileMentionPopup popup(nullptr);
    popup.m_allPaths << "Readme.md"
                     << "src/Main.cpp"
                     << "src/agentloop.cpp";
    
    popup.filterPaths("");
    QCOMPARE(popup.m_filteredPaths.count(), 3);
    
    popup.filterPaths("README");
    QCOMPARE(popup.m_filteredPaths.count(), 1);
    QCOMPARE(popup.m_filteredPaths.first(), "Readme.md");
}

void testFuzzyFilterEmptyResults()
{
    FileMentionPopup popup(nullptr);
    popup.m_allPaths << "file.txt";
    
    popup.filterPaths("nonexistent");
    QCOMPARE(popup.m_filteredPaths.count(), 0);
}

void testFuzzyFilterMultipleTerms()
{
    FileMentionPopup popup(nullptr);
    popup.m_allPaths << "src/Main.cpp"
                     << "src/agentloop.cpp"
                     << "main.cpp";
    
    popup.filterPaths("src/");
    QCOMPARE(popup.m_filteredPaths.count(), 2);
}

void testFuzzyFilterStartsWith()
{
    FileMentionPopup popup(nullptr);
    popup.m_allPaths << "test.cpp"
                     << "testing.cpp"
                     << "test_helper.h";
    
    popup.filterPaths("test");
    QCOMPARE(popup.m_filteredPaths.count(), 3);
}

void testFuzzyKeyEvents()
{
    QWidget *parent = new QWidget();
    QTextEdit *inputEdit = new QTextEdit(parent);
    inputEdit->setPlainText("test");
    
    FileMentionPopup popup(inputEdit);
    
    QString contentBefore = inputEdit->toPlainText();
    QCOMPARE(contentBefore, "test");
    
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_a, Qt::NoModifier, "a", false, 1);
    QCoreApplication::sendEvent(inputEdit, event);
    
    QCOMPARE(inputEdit->toPlainText(), "testa");
}

void testFuzzyBackspace()
{
    QWidget *parent = new QWidget();
    QTextEdit *inputEdit = new QWidget(parent);
    inputEdit->setPlainText("test");
    
    FileMentionPopup popup(inputEdit);
    
    QCOMPARE(inputEdit->toPlainText(), "test");
    
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Back, Qt::NoModifier, "", false, 0);
    QCoreApplication::sendEvent(inputEdit, event);
    
    QCOMPARE(inputEdit->toPlainText(), "te");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug() << "=== Fuzzy Picker Unit Tests ===";
    
    testFuzzyFilterCaseInsensitive();
    testFuzzyFilterEmptyResults();
    testFuzzyFilterMultipleTerms();
    testFuzzyFilterStartsWith();
    testFuzzyKeyEvents();
    testFuzzyBackspace();
    
    qDebug() << "=== All Fuzzy Picker Unit Tests Complete ===";
    return 0;
}
