#pragma once

#include <QObject>
#include <QList>
#include <QUrl>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QDebug>

// Forward declarations
namespace KTextEditor {
    class View;
    class Document;
    class InlineNoteProvider;
}

namespace KTextEditor {

/**
 * @brief Mock InlineNoteProvider per testare il ciclo di vita dei provider
 * 
 * Simula KTextEditor::InlineNoteProvider permettendo di verificare
 * quando viene registrato/deregistrato e se i metodi vengono chiamati.
 */
class InlineNoteProvider : public QObject {
    Q_OBJECT
public:
    explicit InlineNoteProvider(QObject *parent = nullptr) : QObject(parent) {
        m_registered = false;
        m_showCallCount = 0;
        m_clearCallCount = 0;
    }
    
    virtual ~InlineNoteProvider() {
        if (m_registered) {
            qWarning() << "InlineNoteProvider distrutto mentre è ancora registrato!";
        }
    }
    
    // Metodi virtuali da sovrascrivere
    virtual void showInlineNote(View *view, const QString &text) {
        m_showCallCount++;
        m_lastView = view;
        m_lastText = text;
    }
    
    virtual void clearInlineNotes(View *view) {
        m_clearCallCount++;
        m_lastClearView = view;
    }
    
    // Stato per i test
    bool isRegistered() const { return m_registered; }
    void setRegistered(bool registered) { m_registered = registered; }
    
    int showCallCount() const { return m_showCallCount; }
    int clearCallCount() const { return m_clearCallCount; }
    
    View* lastView() const { return m_lastView; }
    View* lastClearView() const { return m_lastClearView; }
    QString lastText() const { return m_lastText; }
    
    // Segnali per testare le notifiche
signals:
    void inlineNotesReset();
    void inlineNotesChanged();
    
private:
    bool m_registered;
    int m_showCallCount;
    int m_clearCallCount;
    View* m_lastView = nullptr;
    View* m_lastClearView = nullptr;
    QString m_lastText;
};

/**
 * @brief Mock Document per simulare documenti KTextEditor
 */
class Document : public QObject {
    Q_OBJECT
public:
    explicit Document(const QUrl &url = QUrl(), QObject *parent = nullptr) 
        : QObject(parent), m_url(url), m_text("") {
    }
    
    ~Document() override {
        emit destroyed(this);
    }
    
    QUrl url() const { return m_url; }
    QString text() const { return m_text; }
    void setText(const QString &text) { 
        m_text = text; 
        emit modified();
    }
    
    // Simula il segnale destroyed di QObject
signals:
    void destroyed(Document *doc);
    void saved();
    void modified();
    
private:
    QUrl m_url;
    QString m_text;
};

/**
 * @brief Mock View per simulare le view KTextEditor
 * 
 * Questa è la classe chiave per testare i crash legati al ciclo di vita
 * delle view e alla registrazione dei provider.
 */
class View : public QObject {
    Q_OBJECT
public:
    explicit View(Document *doc, QObject *parent = nullptr) 
        : QObject(parent), m_doc(doc), m_inlineNoteProviders() {
        if (doc) {
            connect(doc, &Document::destroyed, this, [this]() {
                m_doc = nullptr;
            });
        }
    }
    
    ~View() override {
        // Simula il comportamento di KTextEditor: quando la view viene distrutta,
        // dovrebbe deregistrare tutti i provider. Se non lo fa, i provider
        // diventano dangling pointer e causano crash.
        
        // In KTextEditor reale, questo non avviene automaticamente - 
        // il plugin deve chiamare unregisterInlineNoteProvider() esplicitamente
        if (!m_inlineNoteProviders.isEmpty()) {
            qWarning() << "View distrutta con" << m_inlineNoteProviders.size() 
                      << "provider ancora registrati! Potenziale crash.";
        }
        
        emit destroyed(this);
    }
    
    Document* document() const { return m_doc; }
    
    // Simula registerInlineNoteProvider di KTextEditor::View
    void registerInlineNoteProvider(InlineNoteProvider *provider) {
        if (!provider) return;
        
        if (!m_inlineNoteProviders.contains(provider)) {
            m_inlineNoteProviders.append(provider);
            provider->setRegistered(true);
            qDebug() << "InlineNoteProvider registrato su view" << this;
        }
    }
    
    // Simula unregisterInlineNoteProvider di KTextEditor::View
    void unregisterInlineNoteProvider(InlineNoteProvider *provider) {
        if (!provider) return;
        
        if (m_inlineNoteProviders.removeAll(provider) > 0) {
            provider->setRegistered(false);
            qDebug() << "InlineNoteProvider deregistrato da view" << this;
        }
    }
    
    // Verifica se un provider è ancora registrato
    bool isProviderRegistered(InlineNoteProvider *provider) const {
        return m_inlineNoteProviders.contains(provider);
    }
    
    int providerCount() const { return m_inlineNoteProviders.size(); }
    
signals:
    void destroyed(View *view);
    
private:
    Document *m_doc;
    QList<InlineNoteProvider*> m_inlineNoteProviders;
};

/**
 * @brief Mock MainWindow per simulare KTextEditor::MainWindow
 * 
 * Gestisce la creazione e distruzione delle view, emettendo i segnali
 * viewCreated e viewDestroyed come fa Kate reale.
 */
class MainWindow : public QObject {
    Q_OBJECT
public:
    explicit MainWindow(QObject *parent = nullptr) : QObject(parent) {
    }
    
    ~MainWindow() override {
        // Distruggi tutte le view create
        qDeleteAll(m_views);
        m_views.clear();
    }
    
    // Crea una nuova view e simula il segnale viewCreated
    View* createView(Document *doc) {
        View *view = new View(doc, this);
        m_views.append(view);
        
        emit viewCreated(view);
        qDebug() << "View creata:" << view;
        
        return view;
    }
    
    // Distrugge una view e simula il segnale viewDestroyed
    void destroyView(View *view) {
        if (!view || !m_views.contains(view)) {
            qWarning() << "Tentativo di distruggere view non valida o non esistente";
            return;
        }
        
        emit viewDestroyed(view);
        qDebug() << "View distrutta:" << view;
        
        m_views.removeAll(view);
        view->deleteLater();
    }
    
    QList<View*> views() const { return m_views; }
    int viewCount() const { return m_views.size(); }
    
signals:
    void viewCreated(View *view);
    void viewDestroyed(View *view);
    
private:
    QList<View*> m_views;
};

} // namespace KTextEditor
