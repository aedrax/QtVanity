#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <QObject>
#include <QtTest>

class TestMainWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Unit tests
    void testInitialLayoutContainsSplitter();
    void testMenuBarContainsExpectedMenus();
    void testStyleApplicationUpdatesWidgets();
    void testSplitterContainsGalleryAndEditor();
    void testFileMenuActions();
    void testEditMenuActions();
    void testHelpMenuActions();
    
    // Style toggle integration tests
    void testToggleStyleActionExistsInEditMenu();
    void testToggleStyleKeyboardShortcut();
    void testToggleActionTriggersModeChange();
};

#endif // TEST_MAINWINDOW_H
