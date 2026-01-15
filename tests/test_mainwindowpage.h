#ifndef TEST_MAINWINDOWPAGE_H
#define TEST_MAINWINDOWPAGE_H

#include <QObject>
#include <QtTest>

/**
 * @brief Test class for MainWindowPage functionality.
 * 
 * Tests include:
 * - Widget presence verification (QToolBar, QStatusBar, QMenuBar, QDockWidget, QSplitter)
 * - Enabled state propagation
 */
class TestMainWindowPage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Unit tests
    void testPageCreation();
    void testToolBarWidgetsPresent();
    void testStatusBarWidgetsPresent();
    void testMenuBarWidgetsPresent();
    void testDockWidgetPresent();
    void testSplitterWidgetsPresent();
    void testSetWidgetsEnabled();
};

#endif // TEST_MAINWINDOWPAGE_H
