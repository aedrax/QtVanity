#ifndef TEST_QSSEDITOR_H
#define TEST_QSSEDITOR_H

#include <QObject>
#include <QtTest>

class TestQssEditor : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Unit tests
    void testSetStyleSheet();
    void testGetStyleSheet();
    void testHasUnsavedChangesInitialState();
    void testHasUnsavedChangesAfterEdit();
    void testAutoApplyToggle();
    
    // Property-based tests
    void testCursorPositionPreservation();
    void testCursorPositionPreservation_data();
    
    void testUnsavedChangesTracking();
    void testUnsavedChangesTracking_data();
    
    void testAutoApplyTiming();
    void testAutoApplyTiming_data();
};

#endif // TEST_QSSEDITOR_H
