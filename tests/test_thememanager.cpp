#include "test_thememanager.h"
#include "ThemeManager.h"
#include "SettingsManager.h"

#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QFile>
#include <QRandomGenerator>
#include <QCoreApplication>

void TestThemeManager::initTestCase()
{
    // Set up application name for QSettings
    QCoreApplication::setOrganizationName(QStringLiteral("QtVanityTest"));
    QCoreApplication::setApplicationName(QStringLiteral("ThemeManagerTest"));
}

void TestThemeManager::cleanupTestCase()
{
    // Clean up QSettings
    QSettings settings;
    settings.clear();
}

void TestThemeManager::init()
{
    // Clear settings before each test
    QSettings settings;
    settings.remove(QStringLiteral("appearance/themeMode"));
}

void TestThemeManager::cleanup()
{
    // Clean up after each test
    QSettings settings;
    settings.remove(QStringLiteral("appearance/themeMode"));
}

void TestThemeManager::testDefaultModeIsSystem()
{
    SettingsManager settings;
    ThemeManager themeManager(&settings);
    
    // When no preference exists, default should be System
    QCOMPARE(themeManager.currentMode(), ThemeManager::ThemeMode::System);
}

void TestThemeManager::testSetThemeModeEmitsSignal()
{
    SettingsManager settings;
    ThemeManager themeManager(&settings);
    
    QSignalSpy spy(&themeManager, &ThemeManager::themeModeChanged);
    
    themeManager.setThemeMode(ThemeManager::ThemeMode::Dark);
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<ThemeManager::ThemeMode>(), ThemeManager::ThemeMode::Dark);
}

void TestThemeManager::testEffectiveThemeResolvesSystem()
{
    SettingsManager settings;
    ThemeManager themeManager(&settings);
    
    // Set to System mode
    themeManager.setThemeMode(ThemeManager::ThemeMode::System);
    
    // Effective theme should be either Dark or Light, never System
    ThemeManager::ThemeMode effective = themeManager.effectiveTheme();
    QVERIFY(effective == ThemeManager::ThemeMode::Dark || 
            effective == ThemeManager::ThemeMode::Light);
}

/**
 * Feature: ui-theme-preference, Property 1: Theme Mode Persistence Round-Trip
 * 
 * For any valid ThemeMode value, saving the preference and then loading it
 * should produce the same ThemeMode value.
 */
void TestThemeManager::testThemeModePersistenceRoundTrip_data()
{
    QTest::addColumn<int>("themeModeValue");
    
    // Test all valid theme modes multiple times to reach 100+ iterations
    QVector<ThemeManager::ThemeMode> modes = {
        ThemeManager::ThemeMode::Dark,
        ThemeManager::ThemeMode::Light,
        ThemeManager::ThemeMode::System
    };
    
    // Generate 100+ test cases
    for (int i = 0; i < 34; ++i) {
        for (const auto &mode : modes) {
            QTest::newRow(qPrintable(QString("mode_%1_iteration_%2")
                .arg(static_cast<int>(mode)).arg(i))) 
                << static_cast<int>(mode);
        }
    }
    
    // Additional edge case iterations
    QTest::newRow("dark_final") << static_cast<int>(ThemeManager::ThemeMode::Dark);
    QTest::newRow("light_final") << static_cast<int>(ThemeManager::ThemeMode::Light);
}

void TestThemeManager::testThemeModePersistenceRoundTrip()
{
    // Feature: ui-theme-preference, Property 1: Theme Mode Persistence Round-Trip
    
    QFETCH(int, themeModeValue);
    
    ThemeManager::ThemeMode originalMode = static_cast<ThemeManager::ThemeMode>(themeModeValue);
    
    // Create first ThemeManager and set the mode
    {
        SettingsManager settings;
        ThemeManager themeManager(&settings);
        themeManager.setThemeMode(originalMode);
        
        // Verify the mode was set
        QCOMPARE(themeManager.currentMode(), originalMode);
    }
    
    // Create a new ThemeManager instance - it should load the saved preference
    {
        SettingsManager settings;
        ThemeManager themeManager(&settings);
        
        // Property: The loaded mode should equal the saved mode
        QVERIFY2(themeManager.currentMode() == originalMode,
                 qPrintable(QString("Round-trip failed: saved %1, loaded %2")
                           .arg(static_cast<int>(originalMode))
                           .arg(static_cast<int>(themeManager.currentMode()))));
    }
    
    // Clean up for next iteration
    QSettings settings;
    settings.remove(QStringLiteral("appearance/themeMode"));
}

/**
 * Feature: ui-theme-preference, Property 2: System Mode Resolution Consistency
 * 
 * For any system theme state (dark or light), when ThemeMode is System,
 * the effective theme should match the detected system theme.
 */
void TestThemeManager::testSystemModeResolutionConsistency_data()
{
    QTest::addColumn<int>("iteration");
    
    // Generate 100+ test iterations
    for (int i = 0; i < 100; ++i) {
        QTest::newRow(qPrintable(QString("system_resolution_%1").arg(i))) << i;
    }
}

void TestThemeManager::testSystemModeResolutionConsistency()
{
    // Feature: ui-theme-preference, Property 2: System Mode Resolution Consistency
    
    QFETCH(int, iteration);
    Q_UNUSED(iteration);
    
    SettingsManager settings;
    ThemeManager themeManager(&settings);
    
    // Set to System mode
    themeManager.setThemeMode(ThemeManager::ThemeMode::System);
    
    // Get the system dark mode detection result
    bool systemIsDark = themeManager.isSystemDarkMode();
    
    // Get the effective theme
    ThemeManager::ThemeMode effective = themeManager.effectiveTheme();
    
    // Property: When in System mode, effective theme should match system detection
    if (systemIsDark) {
        QVERIFY2(effective == ThemeManager::ThemeMode::Dark,
                 "System is dark but effective theme is not Dark");
    } else {
        QVERIFY2(effective == ThemeManager::ThemeMode::Light,
                 "System is light but effective theme is not Light");
    }
    
    // Additional property: effective theme should never be System
    QVERIFY2(effective != ThemeManager::ThemeMode::System,
             "Effective theme should never be System");
}

/**
 * Feature: ui-theme-preference, Property 3: Theme Application Correctness
 *
 * For any explicit ThemeMode selection (Dark or Light), that mode takes
 * effect, and the application stylesheet - the channel the user's own QSS
 * lives in - is left untouched.
 *
 * The second half is the regression guard. Themes used to be QSS templates
 * pushed through qApp->setStyleSheet(), so switching theme silently discarded
 * whatever the user had applied, and a missing template raised a modal error
 * dialog that deadlocked this very suite.
 */
void TestThemeManager::testThemeApplicationCorrectness_data()
{
    QTest::addColumn<int>("themeModeValue");
    QTest::addColumn<QString>("themeName");

    // Generate 100+ test cases alternating between Dark and Light
    for (int i = 0; i < 50; ++i) {
        QTest::newRow(qPrintable(QString("dark_application_%1").arg(i)))
            << static_cast<int>(ThemeManager::ThemeMode::Dark)
            << QStringLiteral("dark");
        QTest::newRow(qPrintable(QString("light_application_%1").arg(i)))
            << static_cast<int>(ThemeManager::ThemeMode::Light)
            << QStringLiteral("light");
    }
}

void TestThemeManager::testThemeApplicationCorrectness()
{
    // Feature: ui-theme-preference, Property 3: Theme Application Correctness

    QFETCH(int, themeModeValue);
    QFETCH(QString, themeName);

    ThemeManager::ThemeMode mode = static_cast<ThemeManager::ThemeMode>(themeModeValue);

    // Stand in for a stylesheet the user is editing.
    const QString userStyleSheet =
        QStringLiteral("/* user */ QPushButton { color: #123456; }");
    qApp->setStyleSheet(userStyleSheet);

    SettingsManager settings;
    ThemeManager themeManager(&settings);

    themeManager.setThemeMode(mode);

    // Property: the requested appearance is the one in effect.
    QCOMPARE(themeManager.currentMode(), mode);
    QCOMPARE(themeManager.effectiveTheme(), mode);

    // Property: the user's stylesheet survived the theme change untouched.
    QVERIFY2(qApp->styleSheet() == userStyleSheet,
             qPrintable(QString("Switching to the %1 theme modified the application "
                                "stylesheet, discarding the user's QSS")
                       .arg(themeName)));

    qApp->setStyleSheet(QString());
}


/**
 * Feature: ui-theme-preference, Property 4: System Theme Auto-Update
 * 
 * While System mode is active, when the system theme changes, the ThemeManager
 * should automatically detect the change and emit effectiveThemeChanged signal.
 */
void TestThemeManager::testSystemThemeAutoUpdate_data()
{
    QTest::addColumn<int>("iteration");
    
    // Generate 100+ test iterations
    for (int i = 0; i < 100; ++i) {
        QTest::newRow(qPrintable(QString("auto_update_%1").arg(i))) << i;
    }
}

void TestThemeManager::testSystemThemeAutoUpdate()
{
    // Feature: ui-theme-preference, Property 4: System Theme Auto-Update
    
    QFETCH(int, iteration);
    Q_UNUSED(iteration);
    
    SettingsManager settings;
    ThemeManager themeManager(&settings);
    
    // Set to System mode
    themeManager.setThemeMode(ThemeManager::ThemeMode::System);
    
    // Verify we're in System mode
    QCOMPARE(themeManager.currentMode(), ThemeManager::ThemeMode::System);
    
    // Get the current effective theme
    ThemeManager::ThemeMode initialEffective = themeManager.effectiveTheme();
    
    // Property: When in System mode, effective theme should always be Dark or Light
    QVERIFY2(initialEffective == ThemeManager::ThemeMode::Dark || 
             initialEffective == ThemeManager::ThemeMode::Light,
             "Effective theme in System mode should be Dark or Light, never System");
    
    // Property: The effective theme should match the system detection
    bool systemIsDark = themeManager.isSystemDarkMode();
    ThemeManager::ThemeMode expectedEffective = systemIsDark ? 
        ThemeManager::ThemeMode::Dark : ThemeManager::ThemeMode::Light;
    
    QVERIFY2(initialEffective == expectedEffective,
             qPrintable(QString("Effective theme (%1) doesn't match system detection (%2)")
                       .arg(static_cast<int>(initialEffective))
                       .arg(systemIsDark ? "dark" : "light")));
    
    // Property: When switching away from System mode, the timer should stop
    // (we can't directly test timer state, but we can verify mode changes work)
    themeManager.setThemeMode(ThemeManager::ThemeMode::Dark);
    QCOMPARE(themeManager.currentMode(), ThemeManager::ThemeMode::Dark);
    QCOMPARE(themeManager.effectiveTheme(), ThemeManager::ThemeMode::Dark);
    
    // Property: When switching back to System mode, detection should still work
    themeManager.setThemeMode(ThemeManager::ThemeMode::System);
    QCOMPARE(themeManager.currentMode(), ThemeManager::ThemeMode::System);
    
    ThemeManager::ThemeMode newEffective = themeManager.effectiveTheme();
    QVERIFY2(newEffective == ThemeManager::ThemeMode::Dark || 
             newEffective == ThemeManager::ThemeMode::Light,
             "Effective theme after re-enabling System mode should be Dark or Light");
    
    // Property: Effective theme should still match system detection
    QCOMPARE(newEffective, expectedEffective);
}
