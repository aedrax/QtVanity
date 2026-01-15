#include "MainWindow.h"

#include "editor/QssEditor.h"
#include "editor/StyleManager.h"
#include "gallery/WidgetGallery.h"

#include <QSplitter>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_splitter(nullptr)
    , m_gallery(nullptr)
    , m_editor(nullptr)
    , m_styleManager(nullptr)
    , m_fileMenu(nullptr)
    , m_editMenu(nullptr)
    , m_helpMenu(nullptr)
    , m_templatesMenu(nullptr)
    , m_loadAction(nullptr)
    , m_saveAction(nullptr)
    , m_exitAction(nullptr)
    , m_applyAction(nullptr)
    , m_toggleStyleAction(nullptr)
    , m_aboutAction(nullptr)
    , m_aboutQtAction(nullptr)
{
    // Create style manager first
    m_styleManager = new StyleManager(this);

    // Setup UI components
    setupCentralWidget();
    setupMenuBar();
    setupConnections();

    // Set window properties
    setWindowTitle(QString("QtVanity - Qt %1").arg(qVersion()));
    resize(1200, 800);

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    // Qt handles child widget deletion
}

void MainWindow::setupCentralWidget()
{
    // Create splitter for split-pane layout
    m_splitter = new QSplitter(Qt::Horizontal, this);

    // Create QSS editor (left panel)
    m_editor = new QssEditor(m_splitter);

    // Create widget gallery (right panel)
    m_gallery = new WidgetGallery(m_splitter);

    // Add widgets to splitter
    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_gallery);

    // Set initial splitter sizes (40% editor, 60% gallery)
    m_splitter->setSizes({400, 600});

    // Set splitter as central widget
    setCentralWidget(m_splitter);
}

void MainWindow::setupMenuBar()
{
    setupFileMenu();
    setupEditMenu();
    setupHelpMenu();
}

void MainWindow::setupFileMenu()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    // Load Style action
    m_loadAction = new QAction(tr("&Load Style..."), this);
    m_loadAction->setShortcut(QKeySequence::Open);
    m_loadAction->setStatusTip(tr("Load a QSS stylesheet from file"));
    connect(m_loadAction, &QAction::triggered, this, &MainWindow::onLoadStyle);
    m_fileMenu->addAction(m_loadAction);

    // Save Style action
    m_saveAction = new QAction(tr("&Save Style..."), this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip(tr("Save the current stylesheet to file"));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSaveStyle);
    m_fileMenu->addAction(m_saveAction);

    m_fileMenu->addSeparator();

    // Templates submenu
    setupTemplatesSubmenu(m_fileMenu);

    m_fileMenu->addSeparator();

    // Exit action
    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Exit the application"));
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);
    m_fileMenu->addAction(m_exitAction);
}

void MainWindow::setupEditMenu()
{
    m_editMenu = menuBar()->addMenu(tr("&Edit"));

    // Apply Style action
    m_applyAction = new QAction(tr("&Apply Style"), this);
    m_applyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return));
    m_applyAction->setStatusTip(tr("Apply the current stylesheet to the application"));
    connect(m_applyAction, &QAction::triggered, this, &MainWindow::onApplyStyle);
    m_editMenu->addAction(m_applyAction);

    // Toggle Style Mode action
    m_toggleStyleAction = new QAction(tr("&Toggle Style Mode"), this);
    m_toggleStyleAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    m_toggleStyleAction->setStatusTip(tr("Toggle between custom QSS and default Qt styling (Ctrl+T)"));
    connect(m_toggleStyleAction, &QAction::triggered, this, &MainWindow::onToggleStyle);
    m_editMenu->addAction(m_toggleStyleAction);
}

void MainWindow::setupHelpMenu()
{
    m_helpMenu = menuBar()->addMenu(tr("&Help"));

    // About action
    m_aboutAction = new QAction(tr("&About QtVanity"), this);
    m_aboutAction->setStatusTip(tr("Show information about QtVanity"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    m_helpMenu->addAction(m_aboutAction);

    // About Qt action
    m_aboutQtAction = new QAction(tr("About &Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Show information about Qt"));
    connect(m_aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    m_helpMenu->addAction(m_aboutQtAction);
}


void MainWindow::setupTemplatesSubmenu(QMenu *parentMenu)
{
    m_templatesMenu = parentMenu->addMenu(tr("Load &Template"));
    m_templatesMenu->setStatusTip(tr("Load a predefined style template"));

    // Get available templates from StyleManager
    QStringList templates = m_styleManager->availableTemplates();

    if (templates.isEmpty()) {
        // Add placeholder if no templates found
        QAction *noTemplates = m_templatesMenu->addAction(tr("(No templates available)"));
        noTemplates->setEnabled(false);
    } else {
        // Add action for each template
        for (const QString &templateName : templates) {
            QAction *templateAction = m_templatesMenu->addAction(templateName);
            connect(templateAction, &QAction::triggered, this, [this, templateName]() {
                onLoadTemplate(templateName);
            });
        }
    }
}

void MainWindow::setupConnections()
{
    // Connect editor apply request to style manager
    connect(m_editor, &QssEditor::applyRequested,
            m_styleManager, &StyleManager::applyStyleSheet);

    // Connect editor default style request to style manager
    connect(m_editor, &QssEditor::defaultStyleRequested,
            m_styleManager, &StyleManager::clearStyleSheet);

    // Connect editor style mode changed to update status bar
    connect(m_editor, &QssEditor::styleModeChanged,
            this, &MainWindow::onStyleModeChanged);

    // Connect style manager signals
    connect(m_styleManager, &StyleManager::styleApplied,
            this, &MainWindow::onStyleApplied);
    connect(m_styleManager, &StyleManager::styleCleared,
            this, &MainWindow::onStyleCleared);
    connect(m_styleManager, &StyleManager::loadError,
            this, &MainWindow::onLoadError);
    connect(m_styleManager, &StyleManager::saveError,
            this, &MainWindow::onSaveError);

    // Connect editor unsaved changes signal
    connect(m_editor, &QssEditor::unsavedChangesChanged,
            this, &MainWindow::onUnsavedChangesChanged);

    // Connect QStyle selector signals
    connect(m_editor, &QssEditor::styleChangeRequested,
            m_styleManager, &StyleManager::setStyle);
    connect(m_styleManager, &StyleManager::styleChanged,
            m_editor, &QssEditor::setCurrentStyle);
    connect(m_styleManager, &StyleManager::styleChangeError,
            this, &MainWindow::onStyleChangeError);

    // Initialize style selector with available styles
    m_editor->setDefaultStyleMarker(m_styleManager->defaultStyle());
    m_editor->setAvailableStyles(m_styleManager->availableStyles());
    m_editor->setCurrentStyle(m_styleManager->currentStyle());
}

void MainWindow::updateWindowTitle()
{
    QString title = QString("QtVanity - Qt %1").arg(qVersion());
    
    if (!m_currentFilePath.isEmpty()) {
        QFileInfo fileInfo(m_currentFilePath);
        title = QString("%1 - %2").arg(fileInfo.fileName(), title);
    }
    
    if (m_editor && m_editor->hasUnsavedChanges()) {
        title = QString("*%1").arg(title);
    }
    
    setWindowTitle(title);
}

bool MainWindow::maybeSave()
{
    if (!m_editor || !m_editor->hasUnsavedChanges()) {
        return true;
    }

    QMessageBox::StandardButton result = QMessageBox::warning(
        this,
        tr("Unsaved Changes"),
        tr("The stylesheet has been modified.\n"
           "Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    switch (result) {
    case QMessageBox::Save:
        onSaveStyle();
        return !m_editor->hasUnsavedChanges(); // Return true if save succeeded
    case QMessageBox::Discard:
        return true;
    case QMessageBox::Cancel:
    default:
        return false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::onLoadStyle()
{
    // Check for unsaved changes first
    if (!maybeSave()) {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Load Style"),
        QString(),
        tr("Qt Style Sheets (*.qss);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString content = m_styleManager->loadFromFile(filePath);
    if (!content.isNull()) {
        m_editor->setStyleSheet(content);
        m_currentFilePath = filePath;
        updateWindowTitle();
    }
}

void MainWindow::onSaveStyle()
{
    QString filePath = m_currentFilePath;
    
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getSaveFileName(
            this,
            tr("Save Style"),
            QString(),
            tr("Qt Style Sheets (*.qss);;All Files (*)")
        );
    }

    if (filePath.isEmpty()) {
        return;
    }

    // Verify .qss extension
    if (!filePath.endsWith(".qss", Qt::CaseInsensitive)) {
        filePath += ".qss";
    }

    if (m_styleManager->saveToFile(filePath, m_editor->styleSheet())) {
        m_currentFilePath = filePath;
        m_editor->markAsSaved();
        updateWindowTitle();
    }
}

void MainWindow::onApplyStyle()
{
    m_editor->apply();
}

void MainWindow::onToggleStyle()
{
    m_editor->toggleStyleMode();
}

void MainWindow::onLoadTemplate(const QString &templateName)
{
    // Check for unsaved changes first
    if (!maybeSave()) {
        return;
    }

    QString content = m_styleManager->loadTemplate(templateName);
    if (!content.isEmpty()) {
        m_editor->setStyleSheet(content);
        m_currentFilePath.clear(); // Template is not a saved file
        updateWindowTitle();
        
        // Optionally apply the template immediately
        m_editor->apply();
    }
}

void MainWindow::onStyleApplied()
{
    // Style was successfully applied
    // Could show status bar message here
    statusBar()->showMessage(tr("Style applied"), 2000);
}

void MainWindow::onStyleCleared()
{
    // Default style was applied
    statusBar()->showMessage(tr("Default Qt style applied"), 2000);
}

void MainWindow::onStyleModeChanged(bool customActive)
{
    if (customActive) {
        statusBar()->showMessage(tr("Custom style mode"), 2000);
    } else {
        statusBar()->showMessage(tr("Default style mode"), 2000);
    }
}

void MainWindow::onLoadError(const QString &error)
{
    QMessageBox::critical(
        this,
        tr("Load Error"),
        tr("Failed to load stylesheet:\n%1").arg(error)
    );
}

void MainWindow::onSaveError(const QString &error)
{
    QMessageBox::critical(
        this,
        tr("Save Error"),
        tr("Failed to save stylesheet:\n%1").arg(error)
    );
}

void MainWindow::onStyleChangeError(const QString &error)
{
    QMessageBox::warning(
        this,
        tr("Style Change Error"),
        tr("Failed to change style:\n%1").arg(error)
    );
}

void MainWindow::onUnsavedChangesChanged(bool hasChanges)
{
    Q_UNUSED(hasChanges)
    updateWindowTitle();
}

void MainWindow::onAbout()
{
    QMessageBox::about(
        this,
        tr("About QtVanity"),
        tr("<h3>QtVanity</h3>"
           "<p>Version 1.0</p>"
           "<p>A QSS (Qt Style Sheets) editor and widget demonstration tool "
           "for Qt developers and designers.</p>"
           "<p>Built with Qt %1</p>").arg(qVersion())
    );
}

QSplitter* MainWindow::splitter() const
{
    return m_splitter;
}

WidgetGallery* MainWindow::gallery() const
{
    return m_gallery;
}

QssEditor* MainWindow::editor() const
{
    return m_editor;
}

StyleManager* MainWindow::styleManager() const
{
    return m_styleManager;
}
