#include "test_mainwindow.h"
#include "MainWindow.h"
#include "QssEditor.h"
#include "StyleManager.h"
#include "WidgetGallery.h"

#include <QSplitter>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QApplication>

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
    
    // Verify gallery is on the left (index 0) and editor on the right (index 1)
    QCOMPARE(splitter->indexOf(gallery), 0);
    QCOMPARE(splitter->indexOf(editor), 1);
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
