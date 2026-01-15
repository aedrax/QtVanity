#include "test_mainwindowpage.h"
#include "MainWindowPage.h"

#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>
#include <QSplitter>
#include <QMenu>

void TestMainWindowPage::initTestCase()
{
    // Setup code if needed
}

void TestMainWindowPage::cleanupTestCase()
{
    // Cleanup code if needed
}

void TestMainWindowPage::testPageCreation()
{
    MainWindowPage page;
    
    // Page should be created without errors
    // Verify the page has child widgets (indicates setup was successful)
    QVERIFY(page.findChildren<QWidget*>().size() > 0);
}

void TestMainWindowPage::testToolBarWidgetsPresent()
{
    MainWindowPage page;
    
    // Find QToolBar children
    QList<QToolBar*> toolbars = page.findChildren<QToolBar*>();
    
    // Should have multiple toolbars (main toolbar + style variations)
    QVERIFY2(toolbars.size() >= 1, 
             qPrintable(QString("Expected at least 1 QToolBar widget, found %1").arg(toolbars.size())));
    
    // Verify at least one toolbar has actions
    bool hasActions = false;
    for (QToolBar *toolbar : toolbars) {
        if (!toolbar->actions().isEmpty()) {
            hasActions = true;
            break;
        }
    }
    QVERIFY2(hasActions, "At least one toolbar should have actions");
}

void TestMainWindowPage::testStatusBarWidgetsPresent()
{
    MainWindowPage page;
    
    // Find QStatusBar children
    QList<QStatusBar*> statusBars = page.findChildren<QStatusBar*>();
    
    // Should have at least one status bar
    QVERIFY2(statusBars.size() >= 1, 
             qPrintable(QString("Expected at least 1 QStatusBar widget, found %1").arg(statusBars.size())));
}

void TestMainWindowPage::testMenuBarWidgetsPresent()
{
    MainWindowPage page;
    
    // Find QMenuBar children
    QList<QMenuBar*> menuBars = page.findChildren<QMenuBar*>();
    
    // Should have at least one menu bar
    QVERIFY2(menuBars.size() >= 1, 
             qPrintable(QString("Expected at least 1 QMenuBar widget, found %1").arg(menuBars.size())));
    
    // Verify menu bar has menus
    QMenuBar *menuBar = menuBars.first();
    QList<QMenu*> menus = menuBar->findChildren<QMenu*>();
    QVERIFY2(menus.size() >= 1, "Menu bar should have at least one menu");
}

void TestMainWindowPage::testDockWidgetPresent()
{
    MainWindowPage page;
    
    // Find QDockWidget children
    QList<QDockWidget*> dockWidgets = page.findChildren<QDockWidget*>();
    
    // Should have at least one dock widget
    QVERIFY2(dockWidgets.size() >= 1, 
             qPrintable(QString("Expected at least 1 QDockWidget, found %1").arg(dockWidgets.size())));
    
    // Verify dock widgets have content
    for (QDockWidget *dock : dockWidgets) {
        QVERIFY2(dock->widget() != nullptr, 
                 qPrintable(QString("Dock widget '%1' should have content widget").arg(dock->windowTitle())));
    }
}

void TestMainWindowPage::testSplitterWidgetsPresent()
{
    MainWindowPage page;
    
    // Find QSplitter children
    QList<QSplitter*> splitters = page.findChildren<QSplitter*>();
    
    // Should have multiple splitters (horizontal, vertical, nested)
    QVERIFY2(splitters.size() >= 2, 
             qPrintable(QString("Expected at least 2 QSplitter widgets, found %1").arg(splitters.size())));
    
    // Verify we have both horizontal and vertical splitters
    bool hasHorizontal = false;
    bool hasVertical = false;
    
    for (QSplitter *splitter : splitters) {
        if (splitter->orientation() == Qt::Horizontal) {
            hasHorizontal = true;
        } else if (splitter->orientation() == Qt::Vertical) {
            hasVertical = true;
        }
    }
    
    QVERIFY2(hasHorizontal, "Should have at least one horizontal splitter");
    QVERIFY2(hasVertical, "Should have at least one vertical splitter");
}

void TestMainWindowPage::testSetWidgetsEnabled()
{
    MainWindowPage page;
    
    // Disable all widgets
    page.setWidgetsEnabled(false);
    
    // Verify widgets are disabled
    QList<QToolBar*> toolbars = page.findChildren<QToolBar*>();
    for (QToolBar *toolbar : toolbars) {
        QVERIFY2(!toolbar->isEnabled(), "QToolBar should be disabled");
    }
    
    QList<QStatusBar*> statusBars = page.findChildren<QStatusBar*>();
    for (QStatusBar *statusBar : statusBars) {
        QVERIFY2(!statusBar->isEnabled(), "QStatusBar should be disabled");
    }
    
    QList<QSplitter*> splitters = page.findChildren<QSplitter*>();
    for (QSplitter *splitter : splitters) {
        QVERIFY2(!splitter->isEnabled(), "QSplitter should be disabled");
    }
    
    // Enable all widgets
    page.setWidgetsEnabled(true);
    
    // Verify widgets are enabled
    for (QToolBar *toolbar : toolbars) {
        QVERIFY2(toolbar->isEnabled(), "QToolBar should be enabled");
    }
    
    for (QStatusBar *statusBar : statusBars) {
        QVERIFY2(statusBar->isEnabled(), "QStatusBar should be enabled");
    }
    
    for (QSplitter *splitter : splitters) {
        QVERIFY2(splitter->isEnabled(), "QSplitter should be enabled");
    }
}
