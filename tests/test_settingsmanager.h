#ifndef TEST_SETTINGSMANAGER_H
#define TEST_SETTINGSMANAGER_H

#include <QObject>
#include <QtTest>

class TestSettingsManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Unit tests for edge cases
    void testDefaultValuesWhenNoSettingsExist();
    void testInvalidStyleNameHandling();
    void testRecentProjectsCapEnforcement();
    void testClearRecentProjects();
    
    // Property-based tests
    // Property 2: Base Style Round-Trip
    void testBaseStyleRoundTrip();
    void testBaseStyleRoundTrip_data();
    
    // Property 3: Splitter State Round-Trip
    void testSplitterStateRoundTrip();
    void testSplitterStateRoundTrip_data();
    
    // Property 4: Window Geometry Round-Trip
    void testWindowGeometryRoundTrip();
    void testWindowGeometryRoundTrip_data();
    
    // Property 5: Recent Projects List Ordering and Cap
    void testRecentProjectsOrderingAndCap();
    void testRecentProjectsOrderingAndCap_data();
};

#endif // TEST_SETTINGSMANAGER_H
