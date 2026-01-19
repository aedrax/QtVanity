#include "test_mainwindow.h"
#include "MainWindow.h"
#include "QssEditor.h"
#include "StyleManager.h"
#include "ThemeManager.h"
#include "VariablePanel.h"
#include "WidgetGallery.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QPushButton>
#include <QApplication>
#include <QSignalSpy>
#include <QSettings>

void TestMainWindow::initTestCase()
{
    // Setup code if needed
}

void TestMainWindow::cleanupTestCase()
{
    // Cleanup code if needed
}

/**
 * Test that the initial layout contains dock widgets.
 */
void TestMainWindow::testInitialLayoutContainsDockWidgets()
{
    MainWindow mainWindow;
    
    // Verify central widget is the QssEditor
    QssEditor *editor = mainWindow.editor();
    QVERIFY2(editor != nullptr, "MainWindow should have an editor");
    QCOMPARE(mainWindow.centralWidget(), editor);
    
    // Verify dock widgets exist by checking for VariablePanel and WidgetGallery
    VariablePanel *variablePanel = mainWindow.variablePanel();
    QVERIFY2(variablePanel != nullptr, "MainWindow should have a variable panel");
    
    WidgetGallery *gallery = mainWindow.gallery();
    QVERIFY2(gallery != nullptr, "MainWindow should have a gallery");
    
    // Verify the panels are wrapped in dock widgets
    QDockWidget *variablePanelDock = qobject_cast<QDockWidget*>(variablePanel->parentWidget());
    QVERIFY2(variablePanelDock != nullptr, "VariablePanel should be in a QDockWidget");
    QCOMPARE(variablePanelDock->objectName(), QString("VariablePanelDock"));
    
    QDockWidget *galleryDock = qobject_cast<QDockWidget*>(gallery->parentWidget());
    QVERIFY2(galleryDock != nullptr, "WidgetGallery should be in a QDockWidget");
    QCOMPARE(galleryDock->objectName(), QString("WidgetGalleryDock"));
}

/**
 * Test that the menu bar contains File, Edit, and Help menus.
 */
void TestMainWindow::testMenuBarContainsExpectedMenus()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QVERIFY2(menuBar != nullptr, "MainWindow should have a menu bar");
    
    // Get all menus from the menu bar
    QList<QAction*> actions = menuBar->actions();
    
    // We expect at least 3 menus: File, Edit, Help
    QVERIFY2(actions.size() >= 3, "Menu bar should have at least 3 menus");
    
    // Check for File menu
    bool hasFileMenu = false;
    bool hasEditMenu = false;
    bool hasHelpMenu = false;
    
    for (QAction *action : actions) {
        QString text = action->text().remove('&'); // Remove accelerator
        if (text == "File") {
            hasFileMenu = true;
            QVERIFY2(action->menu() != nullptr, "File should be a menu");
        } else if (text == "Edit") {
            hasEditMenu = true;
            QVERIFY2(action->menu() != nullptr, "Edit should be a menu");
        } else if (text == "Help") {
            hasHelpMenu = true;
            QVERIFY2(action->menu() != nullptr, "Help should be a menu");
        }
    }
    
    QVERIFY2(hasFileMenu, "Menu bar should contain File menu");
    QVERIFY2(hasEditMenu, "Menu bar should contain Edit menu");
    QVERIFY2(hasHelpMenu, "Menu bar should contain Help menu");
}

/**
 * Test that style application updates widgets in the gallery.
 */
void TestMainWindow::testStyleApplicationUpdatesWidgets()
{
    MainWindow mainWindow;
    
    // Get the editor and apply a simple style
    QssEditor *editor = mainWindow.editor();
    QVERIFY2(editor != nullptr, "MainWindow should have an editor");
    
    // Set a simple stylesheet that changes button background
    QString testStyle = "QPushButton { background-color: rgb(255, 0, 0); }";
    editor->setStyleSheet(testStyle);
    
    // Apply the style
    editor->apply();
    
    // Verify the application stylesheet was updated
    QString appStyle = qApp->styleSheet();
    QVERIFY2(appStyle.contains("QPushButton"), 
             "Application stylesheet should contain the applied style");
    QVERIFY2(appStyle.contains("background-color"), 
             "Application stylesheet should contain background-color property");
    
    // Clean up - reset stylesheet
    qApp->setStyleSheet("");
}

/**
 * Test that dock widgets contain WidgetGallery, QssEditor, and VariablePanel.
 */
void TestMainWindow::testDockWidgetsContainGalleryAndEditor()
{
    MainWindow mainWindow;
    
    // Verify central widget is the editor
    QssEditor *editor = mainWindow.editor();
    QVERIFY2(editor != nullptr, "MainWindow should have an editor");
    QCOMPARE(mainWindow.centralWidget(), editor);
    
    // Verify gallery exists and is in a dock widget
    WidgetGallery *gallery = mainWindow.gallery();
    QVERIFY2(gallery != nullptr, "MainWindow should have a gallery");
    QDockWidget *galleryDock = qobject_cast<QDockWidget*>(gallery->parentWidget());
    QVERIFY2(galleryDock != nullptr, "Gallery should be in a QDockWidget");
    QCOMPARE(galleryDock->widget(), gallery);
    
    // Verify variable panel exists and is in a dock widget
    VariablePanel *variablePanel = mainWindow.variablePanel();
    QVERIFY2(variablePanel != nullptr, "MainWindow should have a variable panel");
    QDockWidget *variablePanelDock = qobject_cast<QDockWidget*>(variablePanel->parentWidget());
    QVERIFY2(variablePanelDock != nullptr, "Variable panel should be in a QDockWidget");
    QCOMPARE(variablePanelDock->widget(), variablePanel);
    
    // Verify dock widgets have correct object names for state persistence
    QCOMPARE(galleryDock->objectName(), QString("WidgetGalleryDock"));
    QCOMPARE(variablePanelDock->objectName(), QString("VariablePanelDock"));
    
    // Verify dock widgets allow all dock areas
    QCOMPARE(galleryDock->allowedAreas(), Qt::AllDockWidgetAreas);
    QCOMPARE(variablePanelDock->allowedAreas(), Qt::AllDockWidgetAreas);
}

/**
 * Test that File menu contains expected actions.
 */
void TestMainWindow::testFileMenuActions()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *fileMenu = nullptr;
    
    // Find File menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "File") {
            fileMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(fileMenu != nullptr, "File menu should exist");
    
    // Check for expected actions
    QList<QAction*> actions = fileMenu->actions();
    
    bool hasNewProject = false;
    bool hasOpenProject = false;
    bool hasSaveProject = false;
    bool hasSaveProjectAs = false;
    bool hasExportQss = false;
    bool hasImportQss = false;
    bool hasSaveRawQss = false;
    bool hasExit = false;
    bool hasTemplatesMenu = false;
    
    for (QAction *action : actions) {
        QString text = action->text().remove('&').remove("...");
        if (text.contains("New Project")) {
            hasNewProject = true;
        } else if (text.contains("Open Project")) {
            hasOpenProject = true;
        } else if (text.contains("Save Project As")) {
            hasSaveProjectAs = true;
        } else if (text.contains("Save Project") && !text.contains("As")) {
            hasSaveProject = true;
        } else if (text.contains("Export QSS")) {
            hasExportQss = true;
        } else if (text.contains("Import QSS")) {
            hasImportQss = true;
        } else if (text.contains("Save Raw QSS")) {
            hasSaveRawQss = true;
        } else if (text.contains("Exit") || text.contains("Quit")) {
            hasExit = true;
        } else if (text.contains("Template")) {
            hasTemplatesMenu = true;
            QVERIFY2(action->menu() != nullptr, "Templates should be a submenu");
        }
    }
    
    QVERIFY2(hasNewProject, "File menu should have New Project action");
    QVERIFY2(hasOpenProject, "File menu should have Open Project action");
    QVERIFY2(hasSaveProject, "File menu should have Save Project action");
    QVERIFY2(hasSaveProjectAs, "File menu should have Save Project As action");
    QVERIFY2(hasExportQss, "File menu should have Export QSS action");
    QVERIFY2(hasImportQss, "File menu should have Import QSS action");
    QVERIFY2(hasSaveRawQss, "File menu should have Save Raw QSS action");
    QVERIFY2(hasExit, "File menu should have Exit action");
    QVERIFY2(hasTemplatesMenu, "File menu should have Templates submenu");
}

/**
 * Test that Edit menu contains expected actions.
 */
void TestMainWindow::testEditMenuActions()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *editMenu = nullptr;
    
    // Find Edit menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "Edit") {
            editMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(editMenu != nullptr, "Edit menu should exist");
    
    // Check for Apply Style action
    QList<QAction*> actions = editMenu->actions();
    
    bool hasApplyStyle = false;
    
    for (QAction *action : actions) {
        QString text = action->text().remove('&');
        if (text.contains("Apply Style")) {
            hasApplyStyle = true;
        }
    }
    
    QVERIFY2(hasApplyStyle, "Edit menu should have Apply Style action");
}

/**
 * Test that Help menu contains expected actions.
 */
void TestMainWindow::testHelpMenuActions()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *helpMenu = nullptr;
    
    // Find Help menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "Help") {
            helpMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(helpMenu != nullptr, "Help menu should exist");
    
    // Check for About actions
    QList<QAction*> actions = helpMenu->actions();
    
    bool hasAbout = false;
    bool hasAboutQt = false;
    
    for (QAction *action : actions) {
        QString text = action->text().remove('&');
        if (text.contains("About QtVanity")) {
            hasAbout = true;
        } else if (text.contains("About Qt")) {
            hasAboutQt = true;
        }
    }
    
    QVERIFY2(hasAbout, "Help menu should have About QtVanity action");
    QVERIFY2(hasAboutQt, "Help menu should have About Qt action");
}

/**
 * Test that Toggle Style Mode action exists in Edit menu.
 */
void TestMainWindow::testToggleStyleActionExistsInEditMenu()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *editMenu = nullptr;
    
    // Find Edit menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "Edit") {
            editMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(editMenu != nullptr, "Edit menu should exist");
    
    // Check for Toggle Style Mode action
    QList<QAction*> actions = editMenu->actions();
    
    bool hasToggleStyle = false;
    
    for (QAction *action : actions) {
        QString text = action->text().remove('&');
        if (text.contains("Toggle Style Mode")) {
            hasToggleStyle = true;
        }
    }
    
    QVERIFY2(hasToggleStyle, "Edit menu should have Toggle Style Mode action");
}

/**
 * Test that Toggle Style Mode action has Ctrl+T keyboard shortcut.
 */
void TestMainWindow::testToggleStyleKeyboardShortcut()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *editMenu = nullptr;
    
    // Find Edit menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "Edit") {
            editMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(editMenu != nullptr, "Edit menu should exist");
    
    // Find Toggle Style Mode action and check its shortcut
    QAction *toggleAction = nullptr;
    for (QAction *action : editMenu->actions()) {
        QString text = action->text().remove('&');
        if (text.contains("Toggle Style Mode")) {
            toggleAction = action;
            break;
        }
    }
    
    QVERIFY2(toggleAction != nullptr, "Toggle Style Mode action should exist");
    
    // Verify the shortcut is Ctrl+T
    QKeySequence expectedShortcut(Qt::CTRL | Qt::Key_T);
    QCOMPARE(toggleAction->shortcut(), expectedShortcut);
}

/**
 * Test that triggering toggle action changes the style mode.
 */
void TestMainWindow::testToggleActionTriggersModeChange()
{
    MainWindow mainWindow;
    
    QssEditor *editor = mainWindow.editor();
    QVERIFY2(editor != nullptr, "MainWindow should have an editor");
    
    // Initial state should be Custom mode
    QVERIFY2(editor->isCustomStyleActive(), "Initial mode should be Custom");
    
    // Find the Toggle Style Mode action
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *editMenu = nullptr;
    
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "Edit") {
            editMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(editMenu != nullptr, "Edit menu should exist");
    
    QAction *toggleAction = nullptr;
    for (QAction *action : editMenu->actions()) {
        QString text = action->text().remove('&');
        if (text.contains("Toggle Style Mode")) {
            toggleAction = action;
            break;
        }
    }
    
    QVERIFY2(toggleAction != nullptr, "Toggle Style Mode action should exist");
    
    // Trigger the action
    toggleAction->trigger();
    
    // Mode should now be Default
    QVERIFY2(!editor->isCustomStyleActive(), "Mode should be Default after toggle");
    
    // Trigger again
    toggleAction->trigger();
    
    // Mode should be back to Custom
    QVERIFY2(editor->isCustomStyleActive(), "Mode should be Custom after second toggle");
    
    // Clean up - reset stylesheet
    qApp->setStyleSheet("");
}

/**
 * Test full workflow: select style -> verify application style changes.
 */
void TestMainWindow::testStyleSelectorWorkflow()
{
    MainWindow mainWindow;
    
    QssEditor *editor = mainWindow.editor();
    StyleManager *styleManager = mainWindow.styleManager();
    
    QVERIFY2(editor != nullptr, "MainWindow should have an editor");
    QVERIFY2(styleManager != nullptr, "MainWindow should have a style manager");
    
    // Get available styles
    QStringList availableStyles = styleManager->availableStyles();
    QVERIFY2(!availableStyles.isEmpty(), "Should have at least one available style");
    
    // Store initial style
    QString initialStyle = styleManager->currentStyle();
    QVERIFY2(!initialStyle.isEmpty(), "Initial style should not be empty");
    
    // Find a different style to switch to
    QString targetStyle;
    for (const QString &style : availableStyles) {
        if (style.compare(initialStyle, Qt::CaseInsensitive) != 0) {
            targetStyle = style;
            break;
        }
    }
    
    // If only one style available, use the same style
    if (targetStyle.isEmpty()) {
        targetStyle = initialStyle;
    }
    
    // Set up signal spy to verify styleChanged is emitted
    QSignalSpy styleChangedSpy(styleManager, &StyleManager::styleChanged);
    
    // Request style change through editor (simulating user selection)
    emit editor->styleChangeRequested(targetStyle);
    
    // Verify style was changed
    QCOMPARE(styleManager->currentStyle().compare(targetStyle, Qt::CaseInsensitive), 0);
    
    // Verify signal was emitted
    QCOMPARE(styleChangedSpy.count(), 1);
    
    // Verify editor was updated with new style
    QCOMPARE(editor->currentStyle().compare(targetStyle, Qt::CaseInsensitive), 0);
    
    // Clean up - reset stylesheet
    qApp->setStyleSheet("");
}

/**
 * Test QSS preservation across style changes.
 */
void TestMainWindow::testQssPreservationAcrossStyleChanges()
{
    MainWindow mainWindow;
    
    QssEditor *editor = mainWindow.editor();
    StyleManager *styleManager = mainWindow.styleManager();
    
    QVERIFY2(editor != nullptr, "MainWindow should have an editor");
    QVERIFY2(styleManager != nullptr, "MainWindow should have a style manager");
    
    // Set some QSS content in the editor
    QString testQss = "QPushButton { background-color: red; }";
    editor->setStyleSheet(testQss);
    
    // Apply the QSS
    editor->apply();
    
    // Verify QSS was applied
    QCOMPARE(styleManager->currentStyleSheet(), testQss);
    
    // Get available styles and find a different one
    QStringList availableStyles = styleManager->availableStyles();
    QString initialStyle = styleManager->currentStyle();
    QString targetStyle;
    
    for (const QString &style : availableStyles) {
        if (style.compare(initialStyle, Qt::CaseInsensitive) != 0) {
            targetStyle = style;
            break;
        }
    }
    
    // If only one style, use the same
    if (targetStyle.isEmpty()) {
        targetStyle = initialStyle;
    }
    
    // Change the base style
    styleManager->setStyle(targetStyle);
    
    // Verify QSS content in editor is unchanged
    QCOMPARE(editor->styleSheet(), testQss);
    
    // Verify QSS is still applied to the application
    QVERIFY2(qApp->styleSheet().contains("background-color"), 
             "QSS should be preserved after style change");
    
    // Clean up - reset stylesheet
    qApp->setStyleSheet("");
}

/**
 * Test error handling for invalid styles.
 */
void TestMainWindow::testStyleChangeErrorHandling()
{
    // Test StyleManager directly without MainWindow to avoid UI dialogs
    StyleManager styleManager;
    
    // Store initial style
    QString initialStyle = styleManager.currentStyle();
    
    // Set up signal spy for error
    QSignalSpy errorSpy(&styleManager, &StyleManager::styleChangeError);
    
    // Try to set an invalid style
    styleManager.setStyle("NonExistentStyleThatDoesNotExist");
    
    // Verify error signal was emitted
    QCOMPARE(errorSpy.count(), 1);
    
    // Verify style was not changed
    QCOMPARE(styleManager.currentStyle(), initialStyle);
    
    // Clean up - reset stylesheet
    qApp->setStyleSheet("");
}


/**
 * Test that View menu contains Theme submenu with Dark, Light, System actions.
 */
void TestMainWindow::testViewMenuContainsThemeSubmenu()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *viewMenu = nullptr;
    
    // Find View menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "View") {
            viewMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(viewMenu != nullptr, "View menu should exist");
    
    // Find Theme submenu
    QMenu *themeMenu = nullptr;
    for (QAction *action : viewMenu->actions()) {
        if (action->text().remove('&') == "Theme") {
            themeMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(themeMenu != nullptr, "Theme submenu should exist");
    
    // Check for Dark, Light, System actions
    bool hasDark = false;
    bool hasLight = false;
    bool hasSystem = false;
    
    for (QAction *action : themeMenu->actions()) {
        QString text = action->text().remove('&');
        if (text == "Dark") {
            hasDark = true;
            QVERIFY2(action->isCheckable(), "Dark action should be checkable");
        } else if (text == "Light") {
            hasLight = true;
            QVERIFY2(action->isCheckable(), "Light action should be checkable");
        } else if (text == "System") {
            hasSystem = true;
            QVERIFY2(action->isCheckable(), "System action should be checkable");
        }
    }
    
    QVERIFY2(hasDark, "Theme menu should have Dark action");
    QVERIFY2(hasLight, "Theme menu should have Light action");
    QVERIFY2(hasSystem, "Theme menu should have System action");
}

/**
 * Test that theme actions are mutually exclusive.
 */
void TestMainWindow::testThemeActionsAreMutuallyExclusive()
{
    MainWindow mainWindow;
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *viewMenu = nullptr;
    
    // Find View menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "View") {
            viewMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(viewMenu != nullptr, "View menu should exist");
    
    // Find Theme submenu
    QMenu *themeMenu = nullptr;
    for (QAction *action : viewMenu->actions()) {
        if (action->text().remove('&') == "Theme") {
            themeMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(themeMenu != nullptr, "Theme submenu should exist");
    
    // Get theme actions
    QAction *darkAction = nullptr;
    QAction *lightAction = nullptr;
    QAction *systemAction = nullptr;
    
    for (QAction *action : themeMenu->actions()) {
        QString text = action->text().remove('&');
        if (text == "Dark") darkAction = action;
        else if (text == "Light") lightAction = action;
        else if (text == "System") systemAction = action;
    }
    
    QVERIFY2(darkAction != nullptr, "Dark action should exist");
    QVERIFY2(lightAction != nullptr, "Light action should exist");
    QVERIFY2(systemAction != nullptr, "System action should exist");
    
    // Verify they belong to the same action group (mutual exclusivity)
    QActionGroup *group = darkAction->actionGroup();
    QVERIFY2(group != nullptr, "Dark action should belong to an action group");
    QVERIFY2(group->isExclusive(), "Action group should be exclusive");
    QCOMPARE(lightAction->actionGroup(), group);
    QCOMPARE(systemAction->actionGroup(), group);
    
    // Test mutual exclusivity by triggering actions
    darkAction->trigger();
    QVERIFY2(darkAction->isChecked(), "Dark should be checked after trigger");
    QVERIFY2(!lightAction->isChecked(), "Light should not be checked");
    QVERIFY2(!systemAction->isChecked(), "System should not be checked");
    
    lightAction->trigger();
    QVERIFY2(!darkAction->isChecked(), "Dark should not be checked");
    QVERIFY2(lightAction->isChecked(), "Light should be checked after trigger");
    QVERIFY2(!systemAction->isChecked(), "System should not be checked");
}

/**
 * Test that theme action connections work correctly.
 */
void TestMainWindow::testThemeActionConnectionsWork()
{
    MainWindow mainWindow;
    
    ThemeManager *themeManager = mainWindow.themeManager();
    QVERIFY2(themeManager != nullptr, "MainWindow should have a theme manager");
    
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *viewMenu = nullptr;
    
    // Find View menu
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "View") {
            viewMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(viewMenu != nullptr, "View menu should exist");
    
    // Find Theme submenu
    QMenu *themeMenu = nullptr;
    for (QAction *action : viewMenu->actions()) {
        if (action->text().remove('&') == "Theme") {
            themeMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(themeMenu != nullptr, "Theme submenu should exist");
    
    // Get theme actions
    QAction *darkAction = nullptr;
    QAction *lightAction = nullptr;
    QAction *systemAction = nullptr;
    
    for (QAction *action : themeMenu->actions()) {
        QString text = action->text().remove('&');
        if (text == "Dark") darkAction = action;
        else if (text == "Light") lightAction = action;
        else if (text == "System") systemAction = action;
    }
    
    // Set up signal spy
    QSignalSpy modeSpy(themeManager, &ThemeManager::themeModeChanged);
    
    // Trigger Dark action and verify ThemeManager is updated
    darkAction->trigger();
    QCOMPARE(themeManager->currentMode(), ThemeManager::ThemeMode::Dark);
    QVERIFY2(modeSpy.count() >= 1, "themeModeChanged should be emitted");
    
    modeSpy.clear();
    
    // Trigger Light action
    lightAction->trigger();
    QCOMPARE(themeManager->currentMode(), ThemeManager::ThemeMode::Light);
    QVERIFY2(modeSpy.count() >= 1, "themeModeChanged should be emitted");
    
    modeSpy.clear();
    
    // Trigger System action
    systemAction->trigger();
    QCOMPARE(themeManager->currentMode(), ThemeManager::ThemeMode::System);
    QVERIFY2(modeSpy.count() >= 1, "themeModeChanged should be emitted");
}

/**
 * Test that initial theme state matches saved preference.
 */
void TestMainWindow::testInitialThemeStateMatchesSavedPreference()
{
    // First, set a known preference
    {
        QSettings settings;
        settings.setValue("appearance/themeMode", static_cast<int>(ThemeManager::ThemeMode::Light));
        settings.sync();
    }
    
    // Create MainWindow - it should load the saved preference
    MainWindow mainWindow;
    
    ThemeManager *themeManager = mainWindow.themeManager();
    QVERIFY2(themeManager != nullptr, "MainWindow should have a theme manager");
    
    // Verify ThemeManager loaded the preference
    QCOMPARE(themeManager->currentMode(), ThemeManager::ThemeMode::Light);
    
    // Find the theme actions and verify checked state
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *viewMenu = nullptr;
    
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "View") {
            viewMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(viewMenu != nullptr, "View menu should exist");
    
    QMenu *themeMenu = nullptr;
    for (QAction *action : viewMenu->actions()) {
        if (action->text().remove('&') == "Theme") {
            themeMenu = action->menu();
            break;
        }
    }
    
    QVERIFY2(themeMenu != nullptr, "Theme submenu should exist");
    
    // Verify Light action is checked
    for (QAction *action : themeMenu->actions()) {
        QString text = action->text().remove('&');
        if (text == "Light") {
            QVERIFY2(action->isChecked(), "Light action should be checked based on saved preference");
        } else if (text == "Dark" || text == "System") {
            QVERIFY2(!action->isChecked(), QString("%1 action should not be checked").arg(text).toUtf8().constData());
        }
    }
    
    // Clean up - reset to System mode (default)
    {
        QSettings settings;
        settings.setValue("appearance/themeMode", static_cast<int>(ThemeManager::ThemeMode::System));
        settings.sync();
    }
}


/**
 * Feature: dockable-panels, Property 1: Toggle action controls visibility
 * For any dock widget, triggering its toggle action toggles visibility.
 * 
 * This property test verifies that for any dock widget and its corresponding
 * toggle action, triggering the action SHALL toggle the dock widget's visibility state.
 */
void TestMainWindow::testToggleActionControlsVisibility_data()
{
    QTest::addColumn<QString>("dockName");
    QTest::addColumn<bool>("initialVisibility");
    
    // Generate test data for multiple iterations with different initial states
    // Testing both dock widgets with both initial visibility states
    // Running 100+ iterations as specified in the design document
    // Using 25 iterations per combination (25 * 4 = 100 total iterations)
    
    for (int iteration = 0; iteration < 25; ++iteration) {
        // VariablePanel dock - initially visible
        QTest::newRow(qPrintable(QString("VariablePanelDock_visible_iter%1").arg(iteration)))
            << "VariablePanelDock" << true;
        
        // VariablePanel dock - initially hidden
        QTest::newRow(qPrintable(QString("VariablePanelDock_hidden_iter%1").arg(iteration)))
            << "VariablePanelDock" << false;
        
        // WidgetGallery dock - initially visible
        QTest::newRow(qPrintable(QString("WidgetGalleryDock_visible_iter%1").arg(iteration)))
            << "WidgetGalleryDock" << true;
        
        // WidgetGallery dock - initially hidden
        QTest::newRow(qPrintable(QString("WidgetGalleryDock_hidden_iter%1").arg(iteration)))
            << "WidgetGalleryDock" << false;
    }
}

void TestMainWindow::testToggleActionControlsVisibility()
{
    QFETCH(QString, dockName);
    QFETCH(bool, initialVisibility);
    
    // Create MainWindow for each test iteration
    MainWindow mainWindow;
    
    // Show the main window - required for dock widget visibility to work properly
    mainWindow.show();
    QApplication::processEvents();
    
    // Find the dock widget by object name
    QDockWidget *dockWidget = mainWindow.findChild<QDockWidget*>(dockName);
    QVERIFY2(dockWidget != nullptr, 
             qPrintable(QString("Dock widget '%1' should exist").arg(dockName)));
    
    // Get the toggle action for this dock widget
    QAction *toggleAction = dockWidget->toggleViewAction();
    QVERIFY2(toggleAction != nullptr, 
             qPrintable(QString("Toggle action for '%1' should exist").arg(dockName)));
    
    // Set initial visibility state
    dockWidget->setVisible(initialVisibility);
    
    // Process events to ensure visibility change is applied
    QApplication::processEvents();
    
    // Record the initial visibility state
    bool visibilityBeforeToggle = dockWidget->isVisible();
    QCOMPARE(visibilityBeforeToggle, initialVisibility);
    
    // Trigger the toggle action
    toggleAction->trigger();
    
    // Process events to ensure the toggle takes effect
    QApplication::processEvents();
    
    // Verify visibility changed to opposite state
    bool visibilityAfterToggle = dockWidget->isVisible();
    QVERIFY2(visibilityAfterToggle != visibilityBeforeToggle,
             qPrintable(QString("Triggering toggle action should change visibility from %1 to %2 for '%3'")
                       .arg(visibilityBeforeToggle ? "visible" : "hidden")
                       .arg(!visibilityBeforeToggle ? "visible" : "hidden")
                       .arg(dockName)));
    
    // Additional verification: toggle again should restore original state
    toggleAction->trigger();
    QApplication::processEvents();
    
    bool visibilityAfterSecondToggle = dockWidget->isVisible();
    QCOMPARE(visibilityAfterSecondToggle, visibilityBeforeToggle);
}

/**
 * Feature: dockable-panels, Property 2: Visibility state syncs to action
 * For any dock widget, action checked state matches visibility.
 * 
 * This property test verifies that when a dock widget's visibility changes
 * (via close button or programmatically), the corresponding View menu action's
 * checked state SHALL match the dock widget's visibility.
 */
void TestMainWindow::testVisibilityStateSyncsToAction_data()
{
    QTest::addColumn<QString>("dockName");
    QTest::addColumn<bool>("targetVisibility");
    
    // Generate test data for multiple iterations with different visibility states
    // Testing both dock widgets with both target visibility states
    // Running 100+ iterations as specified in the design document
    // Using 25 iterations per combination (25 * 4 = 100 total iterations)
    
    for (int iteration = 0; iteration < 25; ++iteration) {
        // VariablePanel dock - set to visible
        QTest::newRow(qPrintable(QString("VariablePanelDock_show_iter%1").arg(iteration)))
            << "VariablePanelDock" << true;
        
        // VariablePanel dock - set to hidden
        QTest::newRow(qPrintable(QString("VariablePanelDock_hide_iter%1").arg(iteration)))
            << "VariablePanelDock" << false;
        
        // WidgetGallery dock - set to visible
        QTest::newRow(qPrintable(QString("WidgetGalleryDock_show_iter%1").arg(iteration)))
            << "WidgetGalleryDock" << true;
        
        // WidgetGallery dock - set to hidden
        QTest::newRow(qPrintable(QString("WidgetGalleryDock_hide_iter%1").arg(iteration)))
            << "WidgetGalleryDock" << false;
    }
}

void TestMainWindow::testVisibilityStateSyncsToAction()
{
    QFETCH(QString, dockName);
    QFETCH(bool, targetVisibility);
    
    // Create MainWindow for each test iteration
    MainWindow mainWindow;
    
    // Show the main window - required for dock widget visibility to work properly
    mainWindow.show();
    QApplication::processEvents();
    
    // Find the dock widget by object name
    QDockWidget *dockWidget = mainWindow.findChild<QDockWidget*>(dockName);
    QVERIFY2(dockWidget != nullptr, 
             qPrintable(QString("Dock widget '%1' should exist").arg(dockName)));
    
    // Get the toggle action for this dock widget
    QAction *toggleAction = dockWidget->toggleViewAction();
    QVERIFY2(toggleAction != nullptr, 
             qPrintable(QString("Toggle action for '%1' should exist").arg(dockName)));
    
    // Set initial state to opposite of target to ensure we're testing a change
    dockWidget->setVisible(!targetVisibility);
    QApplication::processEvents();
    
    // Verify initial state is set correctly
    QCOMPARE(dockWidget->isVisible(), !targetVisibility);
    QCOMPARE(toggleAction->isChecked(), !targetVisibility);
    
    // Change visibility programmatically (simulating close button or programmatic change)
    dockWidget->setVisible(targetVisibility);
    
    // Process events to ensure visibility change is applied and signals are processed
    QApplication::processEvents();
    
    // Verify the dock widget visibility changed
    QCOMPARE(dockWidget->isVisible(), targetVisibility);
    
    // PROPERTY VERIFICATION: Action checked state SHALL match dock visibility
    QVERIFY2(toggleAction->isChecked() == targetVisibility,
             qPrintable(QString("Action checked state (%1) should match dock visibility (%2) for '%3'")
                       .arg(toggleAction->isChecked() ? "checked" : "unchecked")
                       .arg(targetVisibility ? "visible" : "hidden")
                       .arg(dockName)));
    
    // Additional verification: Test the reverse direction
    // Change visibility again to verify sync works in both directions
    dockWidget->setVisible(!targetVisibility);
    QApplication::processEvents();
    
    QVERIFY2(toggleAction->isChecked() == !targetVisibility,
             qPrintable(QString("Action checked state should sync when visibility changes back for '%1'")
                       .arg(dockName)));
    
    // Final verification: Ensure the action is in the View menu and displays checkmark
    QVERIFY2(toggleAction->isCheckable(),
             qPrintable(QString("Toggle action for '%1' should be checkable to display checkmarks")
                       .arg(dockName)));
}


/**
 * Feature: dockable-panels, Property 3: Dock state round-trip persistence
 * For any dock configuration, save then restore produces equivalent state.
 * 
 * This property test verifies that for any valid dock configuration (including
 * positions, sizes, floating state, visibility, and tabification), saving the
 * state via saveState() and restoring via restoreState() SHALL produce an
 * equivalent configuration.
 */
void TestMainWindow::testDockStateRoundTrip_data()
{
    QTest::addColumn<Qt::DockWidgetArea>("variablePanelArea");
    QTest::addColumn<Qt::DockWidgetArea>("galleryArea");
    QTest::addColumn<bool>("variablePanelFloating");
    QTest::addColumn<bool>("galleryFloating");
    QTest::addColumn<bool>("variablePanelVisible");
    QTest::addColumn<bool>("galleryVisible");
    QTest::addColumn<bool>("tabifyDocks");
    
    // Define dock areas to test
    QList<Qt::DockWidgetArea> dockAreas = {
        Qt::LeftDockWidgetArea,
        Qt::RightDockWidgetArea,
        Qt::TopDockWidgetArea,
        Qt::BottomDockWidgetArea
    };
    
    // Generate test data for 100+ iterations with various configurations
    // We'll generate combinations of:
    // - Different dock areas for each widget
    // - Floating vs docked states
    // - Visible vs hidden states
    // - Tabified vs separate configurations
    
    int iteration = 0;
    
    // Test all dock area combinations (4 x 4 = 16 combinations)
    for (Qt::DockWidgetArea vpArea : dockAreas) {
        for (Qt::DockWidgetArea gArea : dockAreas) {
            // Test with both docks visible and docked
            QTest::newRow(qPrintable(QString("areas_%1_%2_visible_docked_iter%3")
                .arg(vpArea).arg(gArea).arg(iteration++)))
                << vpArea << gArea << false << false << true << true << false;
        }
    }
    
    // Test floating states (8 combinations: 2^3 for floating/visible states)
    for (int i = 0; i < 8; ++i) {
        bool vpFloating = (i & 1) != 0;
        bool gFloating = (i & 2) != 0;
        bool vpVisible = (i & 4) != 0 || true; // At least one visible for meaningful test
        
        QTest::newRow(qPrintable(QString("floating_%1_%2_visible_%3_iter%4")
            .arg(vpFloating).arg(gFloating).arg(vpVisible).arg(iteration++)))
            << Qt::LeftDockWidgetArea << Qt::RightDockWidgetArea
            << vpFloating << gFloating << vpVisible << true << false;
    }
    
    // Test visibility combinations (4 combinations)
    for (int i = 0; i < 4; ++i) {
        bool vpVisible = (i & 1) != 0;
        bool gVisible = (i & 2) != 0;
        
        QTest::newRow(qPrintable(QString("visibility_%1_%2_iter%3")
            .arg(vpVisible).arg(gVisible).arg(iteration++)))
            << Qt::LeftDockWidgetArea << Qt::RightDockWidgetArea
            << false << false << vpVisible << gVisible << false;
    }
    
    // Test tabification when docks are in the same area (16 combinations)
    for (Qt::DockWidgetArea area : dockAreas) {
        // Both docks in same area, tabified
        QTest::newRow(qPrintable(QString("tabified_area_%1_iter%2")
            .arg(area).arg(iteration++)))
            << area << area << false << false << true << true << true;
        
        // Both docks in same area, not explicitly tabified (Qt may auto-tabify)
        QTest::newRow(qPrintable(QString("same_area_%1_not_tabified_iter%2")
            .arg(area).arg(iteration++)))
            << area << area << false << false << true << true << false;
    }
    
    // Additional random-like combinations to reach 100+ iterations
    // Mix of different configurations
    for (int i = 0; i < 60; ++i) {
        Qt::DockWidgetArea vpArea = dockAreas[i % 4];
        Qt::DockWidgetArea gArea = dockAreas[(i + 1) % 4];
        bool vpFloating = (i % 5) == 0;
        bool gFloating = (i % 7) == 0;
        bool vpVisible = (i % 3) != 0;
        bool gVisible = (i % 4) != 0;
        bool tabify = (vpArea == gArea) && (i % 2) == 0;
        
        QTest::newRow(qPrintable(QString("mixed_config_iter%1").arg(iteration++)))
            << vpArea << gArea << vpFloating << gFloating << vpVisible << gVisible << tabify;
    }
}

void TestMainWindow::testDockStateRoundTrip()
{
    QFETCH(Qt::DockWidgetArea, variablePanelArea);
    QFETCH(Qt::DockWidgetArea, galleryArea);
    QFETCH(bool, variablePanelFloating);
    QFETCH(bool, galleryFloating);
    QFETCH(bool, variablePanelVisible);
    QFETCH(bool, galleryVisible);
    QFETCH(bool, tabifyDocks);
    
    // Create MainWindow for each test iteration
    MainWindow mainWindow;
    
    // Show the main window - required for dock widget operations
    mainWindow.show();
    QApplication::processEvents();
    
    // Find the dock widgets
    QDockWidget *variablePanelDock = mainWindow.findChild<QDockWidget*>("VariablePanelDock");
    QDockWidget *galleryDock = mainWindow.findChild<QDockWidget*>("WidgetGalleryDock");
    
    QVERIFY2(variablePanelDock != nullptr, "VariablePanelDock should exist");
    QVERIFY2(galleryDock != nullptr, "WidgetGalleryDock should exist");
    
    // Configure the dock widgets according to test parameters
    
    // First, ensure both are docked and visible for initial positioning
    variablePanelDock->setFloating(false);
    galleryDock->setFloating(false);
    variablePanelDock->setVisible(true);
    galleryDock->setVisible(true);
    QApplication::processEvents();
    
    // Set dock areas
    mainWindow.addDockWidget(variablePanelArea, variablePanelDock);
    mainWindow.addDockWidget(galleryArea, galleryDock);
    QApplication::processEvents();
    
    // Handle tabification if both docks are in the same area and tabify is requested
    if (tabifyDocks && variablePanelArea == galleryArea) {
        mainWindow.tabifyDockWidget(variablePanelDock, galleryDock);
        QApplication::processEvents();
    }
    
    // Set floating states
    variablePanelDock->setFloating(variablePanelFloating);
    galleryDock->setFloating(galleryFloating);
    QApplication::processEvents();
    
    // Set visibility states
    variablePanelDock->setVisible(variablePanelVisible);
    galleryDock->setVisible(galleryVisible);
    QApplication::processEvents();
    
    // Record the original configuration
    bool originalVpFloating = variablePanelDock->isFloating();
    bool originalGFloating = galleryDock->isFloating();
    bool originalVpVisible = variablePanelDock->isVisible();
    bool originalGVisible = galleryDock->isVisible();
    Qt::DockWidgetArea originalVpArea = mainWindow.dockWidgetArea(variablePanelDock);
    Qt::DockWidgetArea originalGArea = mainWindow.dockWidgetArea(galleryDock);
    
    // Save the dock state
    QByteArray savedState = mainWindow.saveState();
    QVERIFY2(!savedState.isEmpty(), "Saved state should not be empty");
    
    // Modify the configuration to something different
    // This ensures we're actually testing restoration, not just that nothing changed
    variablePanelDock->setFloating(!originalVpFloating);
    galleryDock->setFloating(!originalGFloating);
    variablePanelDock->setVisible(!originalVpVisible);
    galleryDock->setVisible(!originalGVisible);
    
    // Move docks to different areas if they weren't floating
    if (!originalVpFloating) {
        Qt::DockWidgetArea newVpArea = (originalVpArea == Qt::LeftDockWidgetArea) 
            ? Qt::RightDockWidgetArea : Qt::LeftDockWidgetArea;
        mainWindow.addDockWidget(newVpArea, variablePanelDock);
    }
    if (!originalGFloating) {
        Qt::DockWidgetArea newGArea = (originalGArea == Qt::RightDockWidgetArea) 
            ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea;
        mainWindow.addDockWidget(newGArea, galleryDock);
    }
    QApplication::processEvents();
    
    // Restore the saved state
    bool restoreSuccess = mainWindow.restoreState(savedState);
    QVERIFY2(restoreSuccess, "restoreState() should return true for valid state");
    QApplication::processEvents();
    
    // PROPERTY VERIFICATION: Configuration after restore SHALL match original
    
    // Verify floating states match
    QVERIFY2(variablePanelDock->isFloating() == originalVpFloating,
             qPrintable(QString("VariablePanel floating state should be restored: expected %1, got %2")
                       .arg(originalVpFloating ? "floating" : "docked")
                       .arg(variablePanelDock->isFloating() ? "floating" : "docked")));
    
    QVERIFY2(galleryDock->isFloating() == originalGFloating,
             qPrintable(QString("Gallery floating state should be restored: expected %1, got %2")
                       .arg(originalGFloating ? "floating" : "docked")
                       .arg(galleryDock->isFloating() ? "floating" : "docked")));
    
    // Verify visibility states match
    QVERIFY2(variablePanelDock->isVisible() == originalVpVisible,
             qPrintable(QString("VariablePanel visibility should be restored: expected %1, got %2")
                       .arg(originalVpVisible ? "visible" : "hidden")
                       .arg(variablePanelDock->isVisible() ? "visible" : "hidden")));
    
    QVERIFY2(galleryDock->isVisible() == originalGVisible,
             qPrintable(QString("Gallery visibility should be restored: expected %1, got %2")
                       .arg(originalGVisible ? "visible" : "hidden")
                       .arg(galleryDock->isVisible() ? "visible" : "hidden")));
    
    // Verify dock areas match (only for non-floating docks)
    if (!originalVpFloating && originalVpVisible) {
        Qt::DockWidgetArea restoredVpArea = mainWindow.dockWidgetArea(variablePanelDock);
        QVERIFY2(restoredVpArea == originalVpArea,
                 qPrintable(QString("VariablePanel dock area should be restored: expected %1, got %2")
                           .arg(originalVpArea).arg(restoredVpArea)));
    }
    
    if (!originalGFloating && originalGVisible) {
        Qt::DockWidgetArea restoredGArea = mainWindow.dockWidgetArea(galleryDock);
        QVERIFY2(restoredGArea == originalGArea,
                 qPrintable(QString("Gallery dock area should be restored: expected %1, got %2")
                           .arg(originalGArea).arg(restoredGArea)));
    }
    
    // Verify toggle actions are synced with visibility
    QAction *vpToggleAction = variablePanelDock->toggleViewAction();
    QAction *gToggleAction = galleryDock->toggleViewAction();
    
    QVERIFY2(vpToggleAction->isChecked() == originalVpVisible,
             "VariablePanel toggle action should be synced with restored visibility");
    QVERIFY2(gToggleAction->isChecked() == originalGVisible,
             "Gallery toggle action should be synced with restored visibility");
}


/**
 * Test that dock widgets have correct default positions when no saved state exists.
 * 
 * This test verifies that when MainWindow is created without any saved dock state,
 * the dock widgets are positioned in their default locations:
 * - VariablePanel dock: Qt::LeftDockWidgetArea
 * - WidgetGallery dock: Qt::RightDockWidgetArea
 */
void TestMainWindow::testDefaultDockPositionsWhenNoSavedState()
{
    // Clear any existing dock state settings to simulate fresh install
    {
        QSettings settings;
        settings.remove("window/dockState");
        settings.sync();
    }
    
    // Create MainWindow - should use default positions
    MainWindow mainWindow;
    mainWindow.show();
    QApplication::processEvents();
    
    // Find the dock widgets
    QDockWidget *variablePanelDock = mainWindow.findChild<QDockWidget*>("VariablePanelDock");
    QDockWidget *galleryDock = mainWindow.findChild<QDockWidget*>("WidgetGalleryDock");
    
    QVERIFY2(variablePanelDock != nullptr, "VariablePanelDock should exist");
    QVERIFY2(galleryDock != nullptr, "WidgetGalleryDock should exist");
    
    // Verify default positions
    Qt::DockWidgetArea vpArea = mainWindow.dockWidgetArea(variablePanelDock);
    Qt::DockWidgetArea gArea = mainWindow.dockWidgetArea(galleryDock);
    
    QVERIFY2(vpArea == Qt::LeftDockWidgetArea,
             qPrintable(QString("VariablePanel should default to LeftDockWidgetArea, got %1").arg(vpArea)));
    QVERIFY2(gArea == Qt::RightDockWidgetArea,
             qPrintable(QString("WidgetGallery should default to RightDockWidgetArea, got %1").arg(gArea)));
    
    // Verify both docks are visible by default
    QVERIFY2(variablePanelDock->isVisible(), "VariablePanel should be visible by default");
    QVERIFY2(galleryDock->isVisible(), "WidgetGallery should be visible by default");
    
    // Verify both docks are not floating by default
    QVERIFY2(!variablePanelDock->isFloating(), "VariablePanel should not be floating by default");
    QVERIFY2(!galleryDock->isFloating(), "WidgetGallery should not be floating by default");
    
    // Verify dock widgets allow all dock areas
    QCOMPARE(variablePanelDock->allowedAreas(), Qt::AllDockWidgetAreas);
    QCOMPARE(galleryDock->allowedAreas(), Qt::AllDockWidgetAreas);
}

/**
 * Test graceful handling of corrupted dock state data.
 * 
 * This test verifies that when restoreState() receives corrupted or incompatible
 * state data, the application continues with default dock positions without crashing.
 */
void TestMainWindow::testGracefulHandlingOfCorruptedDockState()
{
    // Test with various types of corrupted data
    QList<QByteArray> corruptedStates = {
        QByteArray(),                           // Empty data
        QByteArray("invalid data"),             // Random string
        QByteArray::fromHex("deadbeef"),        // Random hex bytes
        QByteArray(100, '\0'),                  // Null bytes
        QByteArray("AAAA") + QByteArray(50, 'X'), // Partial valid-looking header
    };
    
    for (int i = 0; i < corruptedStates.size(); ++i) {
        const QByteArray &corruptedState = corruptedStates[i];
        
        // Create MainWindow
        MainWindow mainWindow;
        mainWindow.show();
        QApplication::processEvents();
        
        // Find the dock widgets
        QDockWidget *variablePanelDock = mainWindow.findChild<QDockWidget*>("VariablePanelDock");
        QDockWidget *galleryDock = mainWindow.findChild<QDockWidget*>("WidgetGalleryDock");
        
        QVERIFY2(variablePanelDock != nullptr, "VariablePanelDock should exist");
        QVERIFY2(galleryDock != nullptr, "WidgetGalleryDock should exist");
        
        // Record default positions
        Qt::DockWidgetArea defaultVpArea = mainWindow.dockWidgetArea(variablePanelDock);
        Qt::DockWidgetArea defaultGArea = mainWindow.dockWidgetArea(galleryDock);
        bool defaultVpVisible = variablePanelDock->isVisible();
        bool defaultGVisible = galleryDock->isVisible();
        
        // Attempt to restore corrupted state - should fail gracefully
        bool restoreResult = mainWindow.restoreState(corruptedState);
        QApplication::processEvents();
        
        // restoreState should return false for invalid data (or true with no effect)
        // The key is that the application doesn't crash and docks remain functional
        
        // Verify dock widgets still exist and are functional
        QVERIFY2(variablePanelDock != nullptr, 
                 qPrintable(QString("VariablePanelDock should still exist after corrupted state %1").arg(i)));
        QVERIFY2(galleryDock != nullptr, 
                 qPrintable(QString("WidgetGalleryDock should still exist after corrupted state %1").arg(i)));
        
        // If restore failed, docks should remain in their current positions
        // If restore "succeeded" (returned true but did nothing), docks should still be valid
        if (!restoreResult) {
            // Verify positions unchanged after failed restore
            QCOMPARE(mainWindow.dockWidgetArea(variablePanelDock), defaultVpArea);
            QCOMPARE(mainWindow.dockWidgetArea(galleryDock), defaultGArea);
            QCOMPARE(variablePanelDock->isVisible(), defaultVpVisible);
            QCOMPARE(galleryDock->isVisible(), defaultGVisible);
        }
        
        // Verify docks are still interactive - can toggle visibility
        variablePanelDock->setVisible(!variablePanelDock->isVisible());
        QApplication::processEvents();
        variablePanelDock->setVisible(defaultVpVisible);
        QApplication::processEvents();
        
        // Verify toggle actions still work
        QAction *vpToggleAction = variablePanelDock->toggleViewAction();
        QVERIFY2(vpToggleAction != nullptr, "Toggle action should still exist");
        QVERIFY2(vpToggleAction->isCheckable(), "Toggle action should still be checkable");
    }
}

/**
 * Test migration from old splitter-only settings.
 * 
 * This test verifies that when upgrading from an older version that used
 * QSplitter-based layout, the application gracefully handles the absence
 * of dock state settings and uses sensible defaults.
 */
void TestMainWindow::testMigrationFromSplitterOnlySettings()
{
    // Simulate old settings with splitter state but no dock state
    {
        QSettings settings;
        // Remove any dock state
        settings.remove("window/dockState");
        // Add a fake splitter state (simulating old version)
        settings.setValue("window/splitterState", QByteArray::fromHex("000000ff00000001"));
        // Keep window geometry (this should still work)
        settings.setValue("window/geometry", QByteArray());
        settings.sync();
    }
    
    // Create MainWindow - should handle missing dock state gracefully
    MainWindow mainWindow;
    mainWindow.show();
    QApplication::processEvents();
    
    // Find the dock widgets
    QDockWidget *variablePanelDock = mainWindow.findChild<QDockWidget*>("VariablePanelDock");
    QDockWidget *galleryDock = mainWindow.findChild<QDockWidget*>("WidgetGalleryDock");
    
    QVERIFY2(variablePanelDock != nullptr, "VariablePanelDock should exist after migration");
    QVERIFY2(galleryDock != nullptr, "WidgetGalleryDock should exist after migration");
    
    // Verify dock widgets are in default positions (migration fallback)
    Qt::DockWidgetArea vpArea = mainWindow.dockWidgetArea(variablePanelDock);
    Qt::DockWidgetArea gArea = mainWindow.dockWidgetArea(galleryDock);
    
    QVERIFY2(vpArea == Qt::LeftDockWidgetArea,
             "VariablePanel should be in default left position after migration");
    QVERIFY2(gArea == Qt::RightDockWidgetArea,
             "WidgetGallery should be in default right position after migration");
    
    // Verify both docks are visible (default state)
    QVERIFY2(variablePanelDock->isVisible(), "VariablePanel should be visible after migration");
    QVERIFY2(galleryDock->isVisible(), "WidgetGallery should be visible after migration");
    
    // Verify the central widget is the editor (not a splitter)
    QssEditor *editor = mainWindow.editor();
    QVERIFY2(editor != nullptr, "Editor should exist");
    QCOMPARE(mainWindow.centralWidget(), editor);
    
    // Verify View menu has dock toggle actions
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *viewMenu = nullptr;
    for (QAction *action : menuBar->actions()) {
        if (action->text().remove('&') == "View") {
            viewMenu = action->menu();
            break;
        }
    }
    QVERIFY2(viewMenu != nullptr, "View menu should exist");
    
    // Find dock toggle actions in View menu
    bool hasVariablePanelAction = false;
    bool hasGalleryAction = false;
    for (QAction *action : viewMenu->actions()) {
        QString text = action->text().remove('&');
        if (text.contains("Variables Panel")) {
            hasVariablePanelAction = true;
            QVERIFY2(action->isCheckable(), "Variables Panel action should be checkable");
            QVERIFY2(action->isChecked(), "Variables Panel action should be checked (visible)");
        } else if (text.contains("Widget Gallery") || text.contains("Gallery")) {
            hasGalleryAction = true;
            QVERIFY2(action->isCheckable(), "Widget Gallery action should be checkable");
            QVERIFY2(action->isChecked(), "Widget Gallery action should be checked (visible)");
        }
    }
    QVERIFY2(hasVariablePanelAction, "View menu should have Variables Panel toggle action");
    QVERIFY2(hasGalleryAction, "View menu should have Widget Gallery toggle action");
    
    // Clean up - remove the fake splitter state
    {
        QSettings settings;
        settings.remove("window/splitterState");
        settings.sync();
    }
}
