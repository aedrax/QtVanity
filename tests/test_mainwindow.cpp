#include "test_mainwindow.h"
#include "MainWindow.h"
#include "QssEditor.h"
#include "StyleManager.h"
#include "ThemeManager.h"
#include "WidgetGallery.h"

#include <QSplitter>
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
 * Test that the initial layout contains a splitter.
 */
void TestMainWindow::testInitialLayoutContainsSplitter()
{
    MainWindow mainWindow;
    
    // Verify splitter exists
    QSplitter *splitter = mainWindow.splitter();
    QVERIFY2(splitter != nullptr, "MainWindow should have a splitter");
    
    // Verify splitter is the central widget
    QCOMPARE(mainWindow.centralWidget(), splitter);
    
    // Verify splitter has horizontal orientation for side-by-side layout
    QCOMPARE(splitter->orientation(), Qt::Horizontal);
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
 * Test that the splitter contains both WidgetGallery and QssEditor.
 */
void TestMainWindow::testSplitterContainsGalleryAndEditor()
{
    MainWindow mainWindow;
    
    QSplitter *splitter = mainWindow.splitter();
    QVERIFY2(splitter != nullptr, "MainWindow should have a splitter");
    
    // Verify splitter has exactly 2 widgets
    QCOMPARE(splitter->count(), 2);
    
    // Verify gallery exists and is in the splitter
    WidgetGallery *gallery = mainWindow.gallery();
    QVERIFY2(gallery != nullptr, "MainWindow should have a gallery");
    QVERIFY2(splitter->indexOf(gallery) >= 0, "Gallery should be in the splitter");
    
    // Verify editor exists and is in the splitter
    QssEditor *editor = mainWindow.editor();
    QVERIFY2(editor != nullptr, "MainWindow should have an editor");
    QVERIFY2(splitter->indexOf(editor) >= 0, "Editor should be in the splitter");
    
    // Verify editor is on the left (index 0) and gallery on the right (index 1)
    QCOMPARE(splitter->indexOf(editor), 0);
    QCOMPARE(splitter->indexOf(gallery), 1);
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
    
    bool hasLoadStyle = false;
    bool hasSaveStyle = false;
    bool hasExit = false;
    bool hasTemplatesMenu = false;
    
    for (QAction *action : actions) {
        QString text = action->text().remove('&').remove("...");
        if (text.contains("Load Style")) {
            hasLoadStyle = true;
        } else if (text.contains("Save Style")) {
            hasSaveStyle = true;
        } else if (text.contains("Exit") || text.contains("Quit")) {
            hasExit = true;
        } else if (text.contains("Template")) {
            hasTemplatesMenu = true;
            QVERIFY2(action->menu() != nullptr, "Templates should be a submenu");
        }
    }
    
    QVERIFY2(hasLoadStyle, "File menu should have Load Style action");
    QVERIFY2(hasSaveStyle, "File menu should have Save Style action");
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
