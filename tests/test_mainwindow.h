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
    void testInitialLayoutContainsDockWidgets();
    void testMenuBarContainsExpectedMenus();
    void testStyleApplicationUpdatesWidgets();
    void testDockWidgetsContainGalleryAndEditor();
    void testFileMenuActions();
    void testEditMenuActions();
    void testHelpMenuActions();
    
    // Style toggle integration tests
    void testToggleStyleActionExistsInEditMenu();
    void testToggleStyleKeyboardShortcut();
    void testToggleActionTriggersModeChange();
    
    // QStyle selector integration tests
    void testStyleSelectorWorkflow();
    void testQssPreservationAcrossStyleChanges();
    void testStyleChangeErrorHandling();
    
    // Theme integration tests
    void testViewMenuContainsThemeSubmenu();
    void testThemeActionsAreMutuallyExclusive();
    void testThemeActionConnectionsWork();
    void testInitialThemeStateMatchesSavedPreference();
    
    // Property-based tests for dock widget visibility
    // Feature: dockable-panels, Property 1: Toggle action controls visibility
    void testToggleActionControlsVisibility_data();
    void testToggleActionControlsVisibility();
    
    // Feature: dockable-panels, Property 2: Visibility state syncs to action
    void testVisibilityStateSyncsToAction_data();
    void testVisibilityStateSyncsToAction();
    
    // Feature: dockable-panels, Property 3: Dock state round-trip persistence
    void testDockStateRoundTrip_data();
    void testDockStateRoundTrip();
    
    // Unit tests for dock state persistence edge cases
    void testDefaultDockPositionsWhenNoSavedState();
    void testGracefulHandlingOfCorruptedDockState();
    void testMigrationFromSplitterOnlySettings();
    
    // Unit tests for template loading
    void testLoadValidQvpTemplate();
    void testLoadTemplateErrorForNonExistentFile();
    void testProjectStateResetAfterLoadingTemplate();
};

#endif // TEST_MAINWINDOW_H
