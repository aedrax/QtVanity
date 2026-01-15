#ifndef TEST_STYLEMANAGER_H
#define TEST_STYLEMANAGER_H

#include <QObject>
#include <QtTest>

class TestStyleManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Unit tests
    void testLoadValidFile();
    void testLoadNonexistentFile();
    void testSaveAndLoad();
    void testAvailableTemplates();
    void testClearStyleSheet();
    void testHasCustomStyleSheet();
    
    // Property-based tests
    void testRoundTripProperty();
    void testRoundTripProperty_data();
    
    // Property 10: Template Loading Content
    void testTemplateLoadingProperty();
    void testTemplateLoadingProperty_data();
    
    // Style Toggle Integration Property Tests
    // Property 1: Default Mode Empty Stylesheet
    void testDefaultModeEmptyStylesheet();
    void testDefaultModeEmptyStylesheet_data();
    
    // Property 2: Custom Mode Restores Stylesheet
    void testCustomModeRestoresStylesheet();
    void testCustomModeRestoresStylesheet_data();
    
    // QStyle management tests
    void testAvailableStylesNonEmpty();
    void testSetStyleChangesCurrentStyle();
    void testSetStyleInvalidNameEmitsError();
    void testQssPreservedAfterStyleChange();
    
    // QStyle Selector Property-Based Tests
    // Property 1: Style List Completeness
    void testStyleListCompletenessProperty();
    void testStyleListCompletenessProperty_data();
    
    // Property 2: QSS Preservation on Style Change
    void testQssPreservationProperty();
    void testQssPreservationProperty_data();
    
    // Property 3: Style Application Consistency
    void testStyleApplicationConsistencyProperty();
    void testStyleApplicationConsistencyProperty_data();
};

#endif // TEST_STYLEMANAGER_H
