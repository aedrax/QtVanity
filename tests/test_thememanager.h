#ifndef TEST_THEMEMANAGER_H
#define TEST_THEMEMANAGER_H

#include <QObject>
#include <QtTest>

class TestThemeManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Unit tests
    void testDefaultModeIsSystem();
    void testSetThemeModeEmitsSignal();
    void testEffectiveThemeResolvesSystem();
    
    // Property-based tests
    // Property 1: Theme Mode Persistence Round-Trip
    void testThemeModePersistenceRoundTrip();
    void testThemeModePersistenceRoundTrip_data();
    
    // Property 2: System Mode Resolution Consistency
    void testSystemModeResolutionConsistency();
    void testSystemModeResolutionConsistency_data();
    
    // Property 3: Theme Application Correctness
    void testThemeApplicationCorrectness();
    void testThemeApplicationCorrectness_data();
    
    // Property 4: System Theme Auto-Update
    void testSystemThemeAutoUpdate();
    void testSystemThemeAutoUpdate_data();
};

#endif // TEST_THEMEMANAGER_H
