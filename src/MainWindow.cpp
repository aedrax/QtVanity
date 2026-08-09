#include "MainWindow.h"

#include "editor/CodeEditor.h"
#include "editor/QssEditor.h"
#include "editor/SettingsManager.h"
#include "editor/StyleManager.h"
#include "editor/ThemeManager.h"
#include "editor/VariableManager.h"
#include "editor/VariablePanel.h"
#include "gallery/WidgetGallery.h"
#include "plugins/PluginManager.h"

#include <QDir>
#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_variablePanelDock(nullptr)
    , m_galleryDock(nullptr)
    , m_gallery(nullptr)
    , m_editor(nullptr)
    , m_styleManager(nullptr)
    , m_themeManager(nullptr)
    , m_variableManager(nullptr)
    , m_variablePanel(nullptr)
    , m_settingsManager(nullptr)
    , m_pluginManager(nullptr)
    , m_fileMenu(nullptr)
    , m_editMenu(nullptr)
    , m_viewMenu(nullptr)
    , m_helpMenu(nullptr)
    , m_templatesMenu(nullptr)
    , m_themeMenu(nullptr)
    , m_recentProjectsMenu(nullptr)
    , m_loadAction(nullptr)
    , m_saveAction(nullptr)
    , m_exitAction(nullptr)
    , m_applyAction(nullptr)
    , m_toggleStyleAction(nullptr)
    , m_aboutAction(nullptr)
    , m_aboutQtAction(nullptr)
    , m_themeDarkAction(nullptr)
    , m_themeLightAction(nullptr)
    , m_themeSystemAction(nullptr)
    , m_themeActionGroup(nullptr)
    , m_newProjectAction(nullptr)
    , m_openProjectAction(nullptr)
    , m_saveProjectAction(nullptr)
    , m_saveProjectAsAction(nullptr)
    , m_exportQssAction(nullptr)
    , m_clearRecentAction(nullptr)
    , m_showVariablePanelAction(nullptr)
    , m_showGalleryAction(nullptr)
    , m_refreshPluginsAction(nullptr)
    , m_pluginDirectoryAction(nullptr)
    , m_toolBar(nullptr)
    , m_livePreviewAction(nullptr)
    , m_previewScopeAction(nullptr)
    , m_cursorPositionLabel(nullptr)
    , m_undefinedVariablesLabel(nullptr)
    , m_previewStateLabel(nullptr)
    , m_projectModified(false)
{
    // Create settings manager first
    m_settingsManager = new SettingsManager(this);

    // Create style manager
    m_styleManager = new StyleManager(this);
    
    // Restore saved base style before theme application
    if (m_settingsManager->hasBaseStyle()) {
        QString savedStyle = m_settingsManager->loadBaseStyle();
        // Validate style exists in available styles (case-insensitive)
        QStringList availableStyles = m_styleManager->availableStyles();
        bool styleValid = false;
        for (const QString &style : availableStyles) {
            if (style.compare(savedStyle, Qt::CaseInsensitive) == 0) {
                styleValid = true;
                break;
            }
        }
        if (styleValid) {
            m_styleManager->setStyle(savedStyle);
        }
        // If invalid, StyleManager keeps platform default
    }
    
    // Create theme manager. It themes the application chrome through Qt's
    // colour scheme, independently of the stylesheet the user is editing.
    m_themeManager = new ThemeManager(m_settingsManager, this);

    // Create variable manager
    m_variableManager = new VariableManager(this);

    // Create plugin manager
    m_pluginManager = new PluginManager(this);
    m_pluginManager->setPluginDirectory(m_settingsManager->pluginDirectory());
    
    // Create plugin directory if it doesn't exist
    QString pluginDir = m_settingsManager->pluginDirectory();
    QDir dir(pluginDir);
    if (!dir.exists()) {
        dir.mkpath(pluginDir);
    }
    
    m_pluginManager->loadPlugins();

    // Connect plugin directory changes to trigger rescan
    connect(m_settingsManager, &SettingsManager::pluginDirectoryChanged,
            this, [this]() {
                m_pluginManager->setPluginDirectory(m_settingsManager->pluginDirectory());
                // refreshPlugins(), not loadPlugins(): the latter leaves the old
                // loaders alive and the old interfaces registered, leaking one
                // QPluginLoader per plugin on every directory change.
                m_pluginManager->refreshPlugins();
            });

    // Setup UI components
    setupCentralWidget();
    setupMenuBar();
    setupToolBar();
    setupConnections();
    clearDockCloseShortcuts();

    // Restore saved dock state if available (must be after setupCentralWidget and setupMenuBar)
    if (m_settingsManager->hasDockState()) {
        restoreState(m_settingsManager->loadDockState());
    }

    // Set window properties
    setWindowTitle(QString("QtVanity - Qt %1").arg(qVersion()));
    resize(1200, 800);

    // Restore saved window geometry if available
    if (m_settingsManager->hasWindowGeometry()) {
        restoreGeometry(m_settingsManager->loadWindowGeometry());
    }

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    // Qt handles child widget deletion
}

void MainWindow::setupCentralWidget()
{
    // Create QSS editor as central widget
    m_editor = new QssEditor(this);
    setCentralWidget(m_editor);

    // Create Variable Panel dock widget
    m_variablePanel = new VariablePanel(this);
    m_variablePanel->setVariableManager(m_variableManager);
    
    m_variablePanelDock = new QDockWidget(tr("Variables"), this);
    m_variablePanelDock->setObjectName("VariablePanelDock");
    m_variablePanelDock->setWidget(m_variablePanel);
    m_variablePanelDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, m_variablePanelDock);

    // Create Widget Gallery dock widget
    m_gallery = new WidgetGallery(this);
    m_gallery->setPluginManager(m_pluginManager);
    
    m_galleryDock = new QDockWidget(tr("Widget Gallery"), this);
    m_galleryDock->setObjectName("WidgetGalleryDock");
    m_galleryDock->setWidget(m_gallery);
    m_galleryDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_galleryDock);
}

void MainWindow::setupToolBar()
{
    // The primary controls used to sit in a row underneath the text area - the
    // last place the eye lands. A toolbar makes the app's state visible at rest.
    m_toolBar = addToolBar(tr("Main"));
    m_toolBar->setObjectName(QStringLiteral("MainToolBar"));
    m_toolBar->setMovable(false);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_toolBar->addAction(m_applyAction);

    // "Live preview" states what it does. The old button was labelled with a
    // mode name ("Custom"/"Default"), which reads equally as the mode you are
    // in and the mode you would switch to.
    m_livePreviewAction = new QAction(tr("Live preview"), this);
    m_livePreviewAction->setCheckable(true);
    m_livePreviewAction->setChecked(m_settingsManager->autoApply());
    m_livePreviewAction->setStatusTip(tr("Apply edits automatically as you type"));
    connect(m_livePreviewAction, &QAction::toggled, this, [this](bool enabled) {
        m_editor->setAutoApplyEnabled(enabled);
        m_settingsManager->saveAutoApply(enabled);
        updateStatusIndicators();
    });
    m_toolBar->addAction(m_livePreviewAction);

    m_toolBar->addSeparator();

    // Preview scope. Off by default: a stylesheet under test should not be
    // able to make the editor and menus unreadable, which leaves the user no
    // visible way back.
    m_previewScopeAction = new QAction(tr("Style whole app"), this);
    m_previewScopeAction->setCheckable(true);
    m_previewScopeAction->setChecked(m_settingsManager->previewAppliesToApplication());
    m_previewScopeAction->setStatusTip(
        tr("Apply the stylesheet to the entire application rather than only the "
           "Widget Gallery. Needed to preview QMainWindow, QDockWidget, QMenuBar "
           "and QStatusBar rules."));
    connect(m_previewScopeAction, &QAction::toggled, this, [this](bool enabled) {
        m_styleManager->setApplyToApplication(enabled);
        m_settingsManager->savePreviewAppliesToApplication(enabled);
        updateStatusIndicators();
    });
    m_toolBar->addAction(m_previewScopeAction);

    m_toolBar->addSeparator();
    m_toolBar->addAction(m_themeMenu->menuAction());
}

void MainWindow::setupMenuBar()
{
    setupFileMenu();
    setupEditMenu();
    setupViewMenu();
    setupHelpMenu();
}

void MainWindow::setupFileMenu()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    // New Project action
    m_newProjectAction = new QAction(tr("&New Project"), this);
    m_newProjectAction->setShortcut(QKeySequence::New);
    m_newProjectAction->setStatusTip(tr("Create a new project with empty variables and template"));
    connect(m_newProjectAction, &QAction::triggered, this, &MainWindow::onNewProject);
    m_fileMenu->addAction(m_newProjectAction);

    // Open Project action
    m_openProjectAction = new QAction(tr("&Open Project..."), this);
    m_openProjectAction->setShortcut(QKeySequence::Open);
    m_openProjectAction->setStatusTip(tr("Open a QtVanity project file (.qvp)"));
    connect(m_openProjectAction, &QAction::triggered, this, &MainWindow::onOpenProject);
    m_fileMenu->addAction(m_openProjectAction);

    // Recent Projects submenu
    setupRecentProjectsMenu();

    // Save Project action
    m_saveProjectAction = new QAction(tr("&Save Project"), this);
    m_saveProjectAction->setShortcut(QKeySequence::Save);
    m_saveProjectAction->setStatusTip(tr("Save the current project"));
    connect(m_saveProjectAction, &QAction::triggered, this, &MainWindow::onSaveProject);
    m_fileMenu->addAction(m_saveProjectAction);

    // Save Project As action
    m_saveProjectAsAction = new QAction(tr("Save Project &As..."), this);
    m_saveProjectAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveProjectAsAction->setStatusTip(tr("Save the current project to a new file"));
    connect(m_saveProjectAsAction, &QAction::triggered, this, &MainWindow::onSaveProjectAs);
    m_fileMenu->addAction(m_saveProjectAsAction);

    m_fileMenu->addSeparator();

    // Export QSS action
    m_exportQssAction = new QAction(tr("&Export QSS..."), this);
    m_exportQssAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_exportQssAction->setStatusTip(tr("Export the resolved stylesheet to a .qss file"));
    connect(m_exportQssAction, &QAction::triggered, this, &MainWindow::onExportQss);
    m_fileMenu->addAction(m_exportQssAction);

    m_fileMenu->addSeparator();

    // Load Style action (for importing plain .qss files)
    m_loadAction = new QAction(tr("&Import QSS..."), this);
    m_loadAction->setStatusTip(tr("Import a QSS stylesheet as template (no variables)"));
    connect(m_loadAction, &QAction::triggered, this, &MainWindow::onLoadStyle);
    m_fileMenu->addAction(m_loadAction);

    // Save Style action (legacy - save raw QSS)
    m_saveAction = new QAction(tr("Save &Raw QSS..."), this);
    m_saveAction->setStatusTip(tr("Save the raw stylesheet (with variable references) to file"));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSaveStyle);
    m_fileMenu->addAction(m_saveAction);

    m_fileMenu->addSeparator();

    // Templates submenu
    setupTemplatesSubmenu(m_fileMenu);

    m_fileMenu->addSeparator();

    // Plugin Directory action
    m_pluginDirectoryAction = new QAction(tr("Plugin &Directory..."), this);
    m_pluginDirectoryAction->setStatusTip(tr("Select the directory where custom widget plugins are located"));
    connect(m_pluginDirectoryAction, &QAction::triggered, this, &MainWindow::onPluginDirectory);
    m_fileMenu->addAction(m_pluginDirectoryAction);

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

void MainWindow::setupViewMenu()
{
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    // Theme submenu
    m_themeMenu = m_viewMenu->addMenu(tr("&Theme"));
    m_themeMenu->setStatusTip(tr("Select application theme"));

    // Create theme actions
    m_themeDarkAction = new QAction(tr("&Dark"), this);
    m_themeDarkAction->setCheckable(true);
    m_themeDarkAction->setStatusTip(tr("Use dark theme"));
    connect(m_themeDarkAction, &QAction::triggered, this, &MainWindow::onThemeDark);
    m_themeMenu->addAction(m_themeDarkAction);

    m_themeLightAction = new QAction(tr("&Light"), this);
    m_themeLightAction->setCheckable(true);
    m_themeLightAction->setStatusTip(tr("Use light theme"));
    connect(m_themeLightAction, &QAction::triggered, this, &MainWindow::onThemeLight);
    m_themeMenu->addAction(m_themeLightAction);

    m_themeSystemAction = new QAction(tr("&System"), this);
    m_themeSystemAction->setCheckable(true);
    m_themeSystemAction->setStatusTip(tr("Follow system theme"));
    connect(m_themeSystemAction, &QAction::triggered, this, &MainWindow::onThemeSystem);
    m_themeMenu->addAction(m_themeSystemAction);

    // Make actions mutually exclusive using QActionGroup
    m_themeActionGroup = new QActionGroup(this);
    m_themeActionGroup->addAction(m_themeDarkAction);
    m_themeActionGroup->addAction(m_themeLightAction);
    m_themeActionGroup->addAction(m_themeSystemAction);
    m_themeActionGroup->setExclusive(true);

    // Initialize action states from current theme mode
    updateThemeActions();

    m_viewMenu->addSeparator();

    // Add dock widget toggle actions
    m_showVariablePanelAction = m_variablePanelDock->toggleViewAction();
    m_showVariablePanelAction->setText(tr("&Variables Panel"));
    m_viewMenu->addAction(m_showVariablePanelAction);

    m_showGalleryAction = m_galleryDock->toggleViewAction();
    m_showGalleryAction->setText(tr("Widget &Gallery"));
    m_viewMenu->addAction(m_showGalleryAction);

    m_viewMenu->addSeparator();

    // Refresh Plugins action
    m_refreshPluginsAction = new QAction(tr("&Refresh Plugins"), this);
    m_refreshPluginsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    m_refreshPluginsAction->setStatusTip(tr("Rescan the plugin directory and reload all plugins"));
    connect(m_refreshPluginsAction, &QAction::triggered,
            m_pluginManager, &PluginManager::refreshPlugins);
    m_viewMenu->addAction(m_refreshPluginsAction);
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

void MainWindow::setupRecentProjectsMenu()
{
    m_recentProjectsMenu = m_fileMenu->addMenu(tr("Recent &Projects"));
    m_recentProjectsMenu->setStatusTip(tr("Open a recently used project"));

    // Populate the menu
    updateRecentProjectsMenu();

    // Connect to settings manager signal for updates
    connect(m_settingsManager, &SettingsManager::recentProjectsChanged,
            this, &MainWindow::updateRecentProjectsMenu);
}

void MainWindow::updateRecentProjectsMenu()
{
    m_recentProjectsMenu->clear();

    QStringList projects = m_settingsManager->recentProjects();

    if (projects.isEmpty()) {
        // Show disabled placeholder when list is empty
        QAction *noRecent = m_recentProjectsMenu->addAction(tr("(No recent projects)"));
        noRecent->setEnabled(false);
    } else {
        // Add action for each recent project
        for (const QString &filePath : projects) {
            QFileInfo fileInfo(filePath);
            QAction *action = m_recentProjectsMenu->addAction(fileInfo.fileName());
            action->setToolTip(filePath);  // Full path as tooltip
            action->setStatusTip(filePath);
            connect(action, &QAction::triggered, this, [this, filePath]() {
                onOpenRecentProject(filePath);
            });
        }
    }

    // Add separator and Clear action
    m_recentProjectsMenu->addSeparator();
    m_clearRecentAction = m_recentProjectsMenu->addAction(tr("Clear Recent Projects"));
    m_clearRecentAction->setEnabled(!projects.isEmpty());
    connect(m_clearRecentAction, &QAction::triggered, this, &MainWindow::onClearRecentProjects);
}

void MainWindow::setupConnections()
{
    // Confine the preview to the gallery unless the user asks for more. This
    // is the difference between a bad stylesheet spoiling the specimen and a
    // bad stylesheet making the program unusable.
    m_styleManager->setPreviewTarget(m_gallery);
    m_styleManager->setApplyToApplication(m_settingsManager->previewAppliesToApplication());
    m_editor->setAutoApplyEnabled(m_settingsManager->autoApply());

    // Persistent status bar indicators, so the app's state is legible at rest
    // rather than only in 2-second flashes.
    m_cursorPositionLabel = new QLabel(this);
    m_undefinedVariablesLabel = new QLabel(this);
    m_previewStateLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_undefinedVariablesLabel);
    statusBar()->addPermanentWidget(m_cursorPositionLabel);
    statusBar()->addPermanentWidget(m_previewStateLabel);

    connect(m_editor->textEdit(), &CodeEditor::cursorPositionChanged,
            this, &MainWindow::updateStatusIndicators);
    connect(m_editor, &QssEditor::contentsChanged,
            this, &MainWindow::updateStatusIndicators);
    updateStatusIndicators();

    // Connect editor apply request to style manager (with variable substitution)
    connect(m_editor, &QssEditor::applyRequested,
            this, [this](const QString &qss) {
                QString resolvedQss = m_variableManager->substitute(qss);
                m_styleManager->applyStyleSheet(resolvedQss);
            });

    // Connect editor default style request to style manager
    connect(m_editor, &QssEditor::defaultStyleRequested,
            m_styleManager, &StyleManager::clearStyleSheet);

    // Connect editor style mode changed to update status bar
    connect(m_editor, &QssEditor::styleModeChanged,
            this, &MainWindow::onStyleModeChanged);

    // Surface Qt's stylesheet parse errors in the editor rather than on stderr.
    connect(m_styleManager, &StyleManager::diagnosticsChanged,
            m_editor, &QssEditor::setProblems);

    // Connect style manager signals
    connect(m_styleManager, &StyleManager::styleApplied,
            this, &MainWindow::onStyleApplied);
    connect(m_styleManager, &StyleManager::styleApplied,
            m_editor, &QssEditor::refreshColorSwatches);
    connect(m_styleManager, &StyleManager::styleApplied,
            m_variablePanel, &VariablePanel::refreshColorSwatches);
    connect(m_styleManager, &StyleManager::styleCleared,
            this, &MainWindow::onStyleCleared);
    connect(m_styleManager, &StyleManager::styleCleared,
            m_editor, &QssEditor::refreshColorSwatches);
    connect(m_styleManager, &StyleManager::styleCleared,
            m_variablePanel, &VariablePanel::refreshColorSwatches);
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
    
    // Save base style when it changes
    connect(m_styleManager, &StyleManager::styleChanged,
            this, [this](const QString &styleName) {
                m_settingsManager->saveBaseStyle(styleName);
            });

    // Initialize style selector with available styles
    m_editor->setDefaultStyleMarker(m_styleManager->defaultStyle());
    m_editor->setAvailableStyles(m_styleManager->availableStyles());
    m_editor->setCurrentStyle(m_styleManager->currentStyle());

    // Connect theme manager signals
    connect(m_themeManager, &ThemeManager::themeModeChanged,
            this, [this](ThemeManager::ThemeMode) { onThemeModeChanged(); });

    // Keep the syntax highlighter in step with the chrome. Without this the
    // highlighter stays on its dark palette forever, rendering pale tokens on
    // a white background under a light theme.
    connect(m_themeManager, &ThemeManager::effectiveThemeChanged,
            this, [this](ThemeManager::ThemeMode effective) {
                m_editor->setDarkColorScheme(effective == ThemeManager::ThemeMode::Dark);
            });
    m_editor->setDarkColorScheme(
        m_themeManager->effectiveTheme() == ThemeManager::ThemeMode::Dark);

    // Connect variable manager signals for live preview
    connect(m_variableManager, &VariableManager::variableChanged,
            this, &MainWindow::onVariableChanged);
    connect(m_variableManager, &VariableManager::variableRemoved,
            this, &MainWindow::onVariableRemoved);
    connect(m_variableManager, &VariableManager::variablesCleared,
            this, &MainWindow::onVariablesCleared);

    // Keep the editor's completer aware of the project's own variables so
    // ${...} references complete alongside the static QSS vocabulary.
    auto refreshCompletionVariables = [this]() {
        m_editor->setCompletionVariables(m_variableManager->variableNames());
    };
    connect(m_variableManager, &VariableManager::variableChanged,
            this, [refreshCompletionVariables](const QString &, const QString &) {
                refreshCompletionVariables();
            });
    connect(m_variableManager, &VariableManager::variableRemoved,
            this, [refreshCompletionVariables](const QString &) {
                refreshCompletionVariables();
            });
    connect(m_variableManager, &VariableManager::variablesCleared,
            this, refreshCompletionVariables);
    connect(m_variableManager, &VariableManager::projectLoaded,
            this, refreshCompletionVariables);

    // Connect variable manager project signals
    connect(m_variableManager, &VariableManager::projectLoaded,
            this, &MainWindow::onProjectLoaded);
    connect(m_variableManager, &VariableManager::projectSaved,
            this, &MainWindow::onProjectSaved);
    connect(m_variableManager, &VariableManager::loadError,
            this, &MainWindow::onProjectLoadError);
    connect(m_variableManager, &VariableManager::saveError,
            this, &MainWindow::onProjectSaveError);

    // Editing marks the project dirty. It deliberately does not apply the
    // style: doing that here restyled every widget in the process on every
    // keystroke, and bypassed both the Auto-Apply checkbox and its debounce
    // timer, so turning Auto-Apply off did not actually stop applying.
    // The editor drives application through applyRequested instead.
    connect(m_editor, &QssEditor::contentsChanged,
            this, [this]() {
                setProjectModified(true);
            });

    // Connect variable insertion from panel to editor
    // The signal emits the formatted reference (e.g., "${name}"), so we insert it directly
    connect(m_variablePanel, &VariablePanel::variableInsertRequested,
            this, [this](const QString &reference) {
                // Insert the reference directly at cursor position
                QTextCursor cursor = m_editor->textEdit()->textCursor();
                cursor.insertText(reference);
                m_editor->textEdit()->setFocus();
            });
}

void MainWindow::updateStatusIndicators()
{
    if (!m_cursorPositionLabel) {
        return;
    }

    const QTextCursor cursor = m_editor->textEdit()->textCursor();
    m_cursorPositionLabel->setText(tr("Ln %1, Col %2")
                                   .arg(cursor.blockNumber() + 1)
                                   .arg(cursor.positionInBlock() + 1));

    // Undefined references pass through substitution untouched and reach the
    // stylesheet as literal "${name}" text, where Qt silently drops the rule.
    const QStringList undefined =
        m_variableManager->findUndefinedReferences(m_editor->qssText());
    if (undefined.isEmpty()) {
        m_undefinedVariablesLabel->clear();
        m_undefinedVariablesLabel->setToolTip(QString());
    } else {
        m_undefinedVariablesLabel->setText(
            tr("%n undefined variable(s)", "", undefined.size()));
        m_undefinedVariablesLabel->setToolTip(undefined.join(QStringLiteral(", ")));
    }

    const bool live = m_editor->isAutoApplyEnabled();
    const bool wholeApp = m_styleManager->applyToApplication();
    m_previewStateLabel->setText(live ? tr("Live") : tr("Paused"));
    m_previewStateLabel->setToolTip(wholeApp
        ? tr("Preview covers the whole application")
        : tr("Preview covers the Widget Gallery only"));
}

void MainWindow::clearDockCloseShortcuts()
{
    const QKeySequence closeSequence(QKeySequence::Close);
    const QList<QDockWidget*> docks = findChildren<QDockWidget*>();

    for (QDockWidget *dock : docks) {
        const QList<QAction*> actions = dock->findChildren<QAction*>();
        for (QAction *action : actions) {
            if (action->shortcut() == closeSequence) {
                action->setShortcut(QKeySequence());
            }
        }
    }
}

void MainWindow::updateWindowTitle()
{
    QString title = QString("QtVanity - Qt %1").arg(qVersion());
    
    // Show project name if we have one
    if (!m_currentProjectPath.isEmpty()) {
        QFileInfo fileInfo(m_currentProjectPath);
        title = QString("%1 - %2").arg(fileInfo.fileName(), title);
    } else if (!m_currentFilePath.isEmpty()) {
        QFileInfo fileInfo(m_currentFilePath);
        title = QString("%1 - %2").arg(fileInfo.fileName(), title);
    }
    
    // Show modified indicator
    if (m_projectModified || (m_editor && m_editor->hasUnsavedChanges())) {
        title = QString("*%1").arg(title);
    }
    
    setWindowTitle(title);
}

void MainWindow::reportError(const QString &title, const QString &message,
                             ErrorSeverity severity)
{
    // A modal dialog in a headless run blocks forever waiting for a click that
    // never comes, so degrade to a non-blocking report instead.
    if (qApp->property("qtvanity.headless").toBool()) {
        qWarning().noquote() << title << "-" << message;
        statusBar()->showMessage(QString("%1: %2").arg(title, message), 5000);
        return;
    }

    if (severity == ErrorSeverity::Critical) {
        QMessageBox::critical(this, title, message);
    } else {
        QMessageBox::warning(this, title, message);
    }
}

bool MainWindow::maybeSaveProject()
{
    if (!m_projectModified && (!m_editor || !m_editor->hasUnsavedChanges())) {
        return true;
    }

    // Never block an automated run on a modal prompt; discard and continue.
    if (qApp->property("qtvanity.headless").toBool()) {
        return true;
    }

    QMessageBox::StandardButton result = QMessageBox::warning(
        this,
        tr("Unsaved Project Changes"),
        tr("The project has been modified.\n"
           "Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    switch (result) {
    case QMessageBox::Save:
        onSaveProject();
        // Return true if save succeeded (project is no longer modified)
        return !m_projectModified && (!m_editor || !m_editor->hasUnsavedChanges());
    case QMessageBox::Discard:
        return true;
    case QMessageBox::Cancel:
    default:
        return false;
    }
}

void MainWindow::setProjectModified(bool modified)
{
    if (m_projectModified != modified) {
        m_projectModified = modified;
        updateWindowTitle();
    }
}

void MainWindow::clearProject()
{
    // Clear the editor before the variables. The old order regenerated the
    // style from the still-populated editor with the variables already gone,
    // leaving the application showing a stylesheet full of unresolved ${...}
    // references, and the final setStyleSheet() blocks signals so nothing ever
    // corrected it.
    m_editor->setQssText(QString());
    m_variableManager->clearVariables();

    m_currentProjectPath.clear();
    m_currentFilePath.clear();
    m_projectModified = false;
    m_editor->markAsSaved();

    // Push the now-empty stylesheet so the preview matches the empty project.
    onRegenerateStyle();
    updateWindowTitle();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSaveProject()) {
        // Save window geometry and dock state before closing
        m_settingsManager->saveWindowGeometry(saveGeometry());
        m_settingsManager->saveDockState(saveState());
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::onLoadStyle()
{
    // Check for unsaved project changes first
    if (!maybeSaveProject()) {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Import QSS"),
        QString(),
        tr("Qt Style Sheets (*.qss);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString content = m_styleManager->loadFromFile(filePath);
    if (!content.isNull()) {
        // Clear variables when importing plain QSS
        m_variableManager->clearVariables();
        m_editor->setQssText(content);
        m_currentFilePath = filePath;
        m_currentProjectPath.clear();
        m_projectModified = false;
        m_editor->markAsSaved();
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

    if (m_styleManager->saveToFile(filePath, m_editor->qssText())) {
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
    // Only prompt to save if there are actual unsaved changes
    if (m_projectModified) {
        if (!maybeSaveProject()) {
            return;
        }
    }

    // Construct path to .qvp file
    QString templatePath = m_styleManager->templatesPath() + "/" + templateName + ".qvp";
    
    // Check if file exists
    if (!QFile::exists(templatePath)) {
        reportError(tr("Template Error"),
                    tr("Template file not found: %1").arg(templateName));
        return;
    }

    // Load the project file using VariableManager
    QString qssTemplate;
    if (m_variableManager->loadProject(templatePath, qssTemplate)) {
        m_editor->setQssText(qssTemplate);
        m_currentProjectPath.clear();  // Templates don't set a project path
        m_currentFilePath.clear();
        m_projectModified = false;
        m_editor->markAsSaved();
        updateWindowTitle();
        
        // Apply the loaded style
        onRegenerateStyle();
        
        statusBar()->showMessage(tr("Template '%1' loaded").arg(templateName), 2000);
    }
    // Error handling is done via VariableManager signals (onProjectLoadError)
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
    reportError(tr("Load Error"), tr("Failed to load stylesheet:\n%1").arg(error));
}

void MainWindow::onSaveError(const QString &error)
{
    reportError(tr("Save Error"), tr("Failed to save stylesheet:\n%1").arg(error));
}

void MainWindow::onStyleChangeError(const QString &error)
{
    reportError(tr("Style Change Error"), tr("Failed to change style:\n%1").arg(error),
                ErrorSeverity::Warning);
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

void MainWindow::onThemeDark()
{
    m_themeManager->setThemeMode(ThemeManager::ThemeMode::Dark);
}

void MainWindow::onThemeLight()
{
    m_themeManager->setThemeMode(ThemeManager::ThemeMode::Light);
}

void MainWindow::onThemeSystem()
{
    m_themeManager->setThemeMode(ThemeManager::ThemeMode::System);
}

void MainWindow::onThemeModeChanged()
{
    updateThemeActions();
}

void MainWindow::onRegenerateStyle()
{
    // Only regenerate if custom style mode is active
    if (m_editor && m_editor->isCustomStyleActive()) {
        QString qssTemplate = m_editor->qssText();
        QString resolvedQss = m_variableManager->substitute(qssTemplate);
        m_styleManager->applyStyleSheet(resolvedQss);
    }
}

void MainWindow::onVariableChanged(const QString &name, const QString &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    setProjectModified(true);
    onRegenerateStyle();
}

void MainWindow::onVariableRemoved(const QString &name)
{
    Q_UNUSED(name)
    setProjectModified(true);
    onRegenerateStyle();
}

void MainWindow::onVariablesCleared()
{
    // Don't set modified here - this is called during clearProject() and loadProject()
    onRegenerateStyle();
}

void MainWindow::onNewProject()
{
    // Check for unsaved project changes first
    if (!maybeSaveProject()) {
        return;
    }

    clearProject();
    statusBar()->showMessage(tr("New project created"), 2000);
}

void MainWindow::onOpenProject()
{
    // Check for unsaved project changes first
    if (!maybeSaveProject()) {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Project"),
        QString(),
        tr("QtVanity Projects (*.qvp);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString qssTemplate;
    if (m_variableManager->loadProject(filePath, qssTemplate)) {
        m_editor->setQssText(qssTemplate);
        m_currentProjectPath = filePath;
        m_currentFilePath.clear();
        m_projectModified = false;
        m_editor->markAsSaved();
        updateWindowTitle();
        
        // Add to recent projects after successful load
        m_settingsManager->addRecentProject(filePath);
        
        // Apply the loaded style
        onRegenerateStyle();
    }
}

void MainWindow::onSaveProject()
{
    if (m_currentProjectPath.isEmpty()) {
        onSaveProjectAs();
        return;
    }

    if (m_variableManager->saveProject(m_currentProjectPath, m_editor->qssText())) {
        m_projectModified = false;
        m_editor->markAsSaved();
        updateWindowTitle();
        
        // Add to recent projects after successful save
        m_settingsManager->addRecentProject(m_currentProjectPath);
    }
}

void MainWindow::onSaveProjectAs()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Project As"),
        QString(),
        tr("QtVanity Projects (*.qvp);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    // Check .qvp extension
    if (!filePath.endsWith(QStringLiteral(".qvp"), Qt::CaseInsensitive)) {
        filePath += QStringLiteral(".qvp");
    }

    if (m_variableManager->saveProject(filePath, m_editor->qssText())) {
        m_currentProjectPath = filePath;
        m_currentFilePath.clear();
        m_projectModified = false;
        m_editor->markAsSaved();
        updateWindowTitle();
        
        // Add to recent projects after successful save
        m_settingsManager->addRecentProject(filePath);
    }
}

void MainWindow::onExportQss()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export QSS"),
        QString(),
        tr("Qt Style Sheets (*.qss);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    // Check .qss extension
    if (!filePath.endsWith(QStringLiteral(".qss"), Qt::CaseInsensitive)) {
        filePath += QStringLiteral(".qss");
    }

    if (m_variableManager->exportResolvedQss(filePath, m_editor->qssText())) {
        statusBar()->showMessage(tr("QSS exported to %1").arg(filePath), 3000);
    }
}

void MainWindow::onProjectLoaded()
{
    statusBar()->showMessage(tr("Project loaded"), 2000);
}

void MainWindow::onProjectSaved()
{
    statusBar()->showMessage(tr("Project saved"), 2000);
}

void MainWindow::onProjectLoadError(const QString &error)
{
    reportError(tr("Project Load Error"), tr("Failed to load project:\n%1").arg(error));
}

void MainWindow::onProjectSaveError(const QString &error)
{
    reportError(tr("Project Save Error"), tr("Failed to save project:\n%1").arg(error));
}

void MainWindow::onOpenRecentProject(const QString &filePath)
{
    // Check if file exists
    if (!QFileInfo::exists(filePath)) {
        reportError(tr("File Not Found"),
                    tr("The project file no longer exists:\n%1").arg(filePath),
                    ErrorSeverity::Warning);
        // Remove from recent projects list
        m_settingsManager->removeRecentProject(filePath);
        return;
    }

    // Check for unsaved project changes first
    if (!maybeSaveProject()) {
        return;
    }

    QString qssTemplate;
    if (m_variableManager->loadProject(filePath, qssTemplate)) {
        m_editor->setQssText(qssTemplate);
        m_currentProjectPath = filePath;
        m_currentFilePath.clear();
        m_projectModified = false;
        m_editor->markAsSaved();
        updateWindowTitle();
        
        // Add to recent projects (moves to front if already exists)
        m_settingsManager->addRecentProject(filePath);
        
        // Apply the loaded style
        onRegenerateStyle();
    }
}

void MainWindow::onClearRecentProjects()
{
    m_settingsManager->clearRecentProjects();
}

void MainWindow::onPluginDirectory()
{
    QString currentDir = m_settingsManager->pluginDirectory();
    
    QString newDir = QFileDialog::getExistingDirectory(
        this,
        tr("Select Plugin Directory"),
        currentDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!newDir.isEmpty() && newDir != currentDir) {
        m_settingsManager->setPluginDirectory(newDir);
        // The signal connection in the constructor will automatically trigger the rescan
    }
}

void MainWindow::updateThemeActions()
{
    if (!m_themeManager) {
        return;
    }

    ThemeManager::ThemeMode mode = m_themeManager->currentMode();
    
    // Block signals to prevent recursive calls
    m_themeDarkAction->blockSignals(true);
    m_themeLightAction->blockSignals(true);
    m_themeSystemAction->blockSignals(true);

    m_themeDarkAction->setChecked(mode == ThemeManager::ThemeMode::Dark);
    m_themeLightAction->setChecked(mode == ThemeManager::ThemeMode::Light);
    m_themeSystemAction->setChecked(mode == ThemeManager::ThemeMode::System);

    m_themeDarkAction->blockSignals(false);
    m_themeLightAction->blockSignals(false);
    m_themeSystemAction->blockSignals(false);
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

ThemeManager* MainWindow::themeManager() const
{
    return m_themeManager;
}

VariableManager* MainWindow::variableManager() const
{
    return m_variableManager;
}

VariablePanel* MainWindow::variablePanel() const
{
    return m_variablePanel;
}

SettingsManager* MainWindow::settingsManager() const
{
    return m_settingsManager;
}

PluginManager* MainWindow::pluginManager() const
{
    return m_pluginManager;
}
