#include "test_settingsmanager.h"
#include "SettingsManager.h"

#include <QSettings>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QByteArray>
#include <QStyleFactory>

void TestSettingsManager::initTestCase()
{
    // Set up application name for QSettings
    QCoreApplication::setOrganizationName(QStringLiteral("QtVanityTest"));
    QCoreApplication::setApplicationName(QStringLiteral("SettingsManagerTest"));
}

void TestSettingsManager::cleanupTestCase()
{
    // Clean up QSettings
    QSettings settings;
    settings.clear();
}

void TestSettingsManager::init()
{
    // Clear settings before each test using the same organization/app name as SettingsManager
    QSettings settings(QStringLiteral("QtVanity"), QStringLiteral("QtVanity"));
    settings.remove(QStringLiteral("window/geometry"));
    settings.remove(QStringLiteral("window/splitterState"));
    settings.remove(QStringLiteral("window/dockState"));
    settings.remove(QStringLiteral("appearance/baseStyle"));
    settings.remove(QStringLiteral("recentProjects"));
    settings.remove(QStringLiteral("plugins/directory"));
}

void TestSettingsManager::cleanup()
{
    // Clean up after each test using the same organization/app name as SettingsManager
    QSettings settings(QStringLiteral("QtVanity"), QStringLiteral("QtVanity"));
    settings.remove(QStringLiteral("window/geometry"));
    settings.remove(QStringLiteral("window/splitterState"));
    settings.remove(QStringLiteral("window/dockState"));
    settings.remove(QStringLiteral("appearance/baseStyle"));
    settings.remove(QStringLiteral("recentProjects"));
    settings.remove(QStringLiteral("plugins/directory"));
}

/**
 * Unit test: Default values when no settings exist
 * 
 * Tests that SettingsManager returns appropriate default values when
 * no settings have been saved yet.
 */
void TestSettingsManager::testDefaultValuesWhenNoSettingsExist()
{
    // Check that settings are cleared
    {
        QSettings settings(QStringLiteral("QtVanity"), QStringLiteral("QtVanity"));
        settings.clear();
    }
    
    SettingsManager settingsManager;
    
    // Test window geometry defaults
    QVERIFY2(!settingsManager.hasWindowGeometry(),
             "hasWindowGeometry() should return false when no geometry saved");
    QVERIFY2(settingsManager.loadWindowGeometry().isEmpty(),
             "loadWindowGeometry() should return empty QByteArray when no geometry saved");
    
    // Test splitter state defaults
    QVERIFY2(!settingsManager.hasSplitterState(),
             "hasSplitterState() should return false when no state saved");
    QVERIFY2(settingsManager.loadSplitterState().isEmpty(),
             "loadSplitterState() should return empty QByteArray when no state saved");
    
    // Test dock state defaults
    QVERIFY2(!settingsManager.hasDockState(),
             "hasDockState() should return false when no state saved");
    QVERIFY2(settingsManager.loadDockState().isEmpty(),
             "loadDockState() should return empty QByteArray when no state saved");
    
    // Test base style defaults
    QVERIFY2(!settingsManager.hasBaseStyle(),
             "hasBaseStyle() should return false when no style saved");
    QVERIFY2(settingsManager.loadBaseStyle().isEmpty(),
             "loadBaseStyle() should return empty string when no style saved");
    
    // Test recent projects defaults
    QVERIFY2(settingsManager.recentProjects().isEmpty(),
             "recentProjects() should return empty list when no projects saved");
}

/**
 * Unit test: Invalid style name handling
 * 
 * Tests that SettingsManager correctly handles invalid or unavailable style names.
 * The caller (MainWindow) is responsible for falling back to platform default.
 */
void TestSettingsManager::testInvalidStyleNameHandling()
{
    SettingsManager settingsManager;
    
    // Save an invalid/non-existent style name
    QString invalidStyle = QStringLiteral("NonExistentStyleThatDoesNotExist12345");
    settingsManager.saveBaseStyle(invalidStyle);
    
    // SettingsManager should store and return the value as-is
    // (validation is the caller's responsibility)
    QVERIFY2(settingsManager.hasBaseStyle(),
             "hasBaseStyle() should return true after saving any style name");
    
    QString loadedStyle = settingsManager.loadBaseStyle();
    QVERIFY2(loadedStyle == invalidStyle,
             qPrintable(QString("loadBaseStyle() should return saved value '%1', got '%2'")
                       .arg(invalidStyle).arg(loadedStyle)));
    
    // Verify the style is not in available styles (confirming it's invalid)
    QStringList availableStyles = QStyleFactory::keys();
    QVERIFY2(!availableStyles.contains(invalidStyle, Qt::CaseInsensitive),
             "Test style should not be in available styles list");
    
    // Test empty style name
    settingsManager.saveBaseStyle(QString());
    QVERIFY2(settingsManager.loadBaseStyle().isEmpty(),
             "loadBaseStyle() should return empty string when empty style saved");
}

/**
 * Unit test: Recent projects cap enforcement
 * 
 * Tests that the recent projects list is properly capped at MaxRecentProjects (10).
 */
void TestSettingsManager::testRecentProjectsCapEnforcement()
{
    SettingsManager settingsManager;
    
    // Add more than MaxRecentProjects (10) projects
    const int numProjectsToAdd = 15;
    for (int i = 0; i < numProjectsToAdd; ++i) {
        settingsManager.addRecentProject(QString("/path/to/project_%1.qvp").arg(i));
    }
    
    QStringList projects = settingsManager.recentProjects();
    
    // Verify cap is enforced
    QVERIFY2(projects.size() == SettingsManager::MaxRecentProjects,
             qPrintable(QString("Recent projects should be capped at %1, got %2")
                       .arg(SettingsManager::MaxRecentProjects).arg(projects.size())));
    
    // Verify most recent project is first (project_14 was added last)
    QVERIFY2(projects.first() == QStringLiteral("/path/to/project_14.qvp"),
             qPrintable(QString("Most recent project should be first, got '%1'")
                       .arg(projects.first())));
    
    // Verify oldest projects were removed (project_0 through project_4)
    for (int i = 0; i < 5; ++i) {
        QString oldProject = QString("/path/to/project_%1.qvp").arg(i);
        QVERIFY2(!projects.contains(oldProject),
                 qPrintable(QString("Old project '%1' should have been removed").arg(oldProject)));
    }
    
    // Verify newest projects are retained (project_5 through project_14)
    for (int i = 5; i < numProjectsToAdd; ++i) {
        QString newProject = QString("/path/to/project_%1.qvp").arg(i);
        QVERIFY2(projects.contains(newProject),
                 qPrintable(QString("New project '%1' should be in list").arg(newProject)));
    }
}

/**
 * Unit test: Clear recent projects
 * 
 * Tests that clearRecentProjects() properly empties the list and emits signal.
 */
void TestSettingsManager::testClearRecentProjects()
{
    SettingsManager settingsManager;
    
    // Add some projects
    settingsManager.addRecentProject(QStringLiteral("/path/to/project1.qvp"));
    settingsManager.addRecentProject(QStringLiteral("/path/to/project2.qvp"));
    settingsManager.addRecentProject(QStringLiteral("/path/to/project3.qvp"));
    
    // Verify projects were added
    QVERIFY2(settingsManager.recentProjects().size() == 3,
             "Should have 3 recent projects before clearing");
    
    // Set up signal spy to verify signal emission
    QSignalSpy spy(&settingsManager, &SettingsManager::recentProjectsChanged);
    
    // Clear recent projects
    settingsManager.clearRecentProjects();
    
    // Verify list is empty
    QVERIFY2(settingsManager.recentProjects().isEmpty(),
             "Recent projects list should be empty after clearing");
    
    // Verify signal was emitted
    QVERIFY2(spy.count() == 1,
             qPrintable(QString("recentProjectsChanged signal should be emitted once, got %1")
                       .arg(spy.count())));
    
    // Verify clearing an already empty list still works
    settingsManager.clearRecentProjects();
    QVERIFY2(settingsManager.recentProjects().isEmpty(),
             "Recent projects list should remain empty after clearing again");
}

/**
 * Feature: settings-persistence, Property 2: Base Style Round-Trip
 * 
 * For any valid style name from the available styles list, saving the base style
 * and then loading it should return the same style name.
 */
void TestSettingsManager::testBaseStyleRoundTrip_data()
{
    QTest::addColumn<QString>("styleName");
    
    // Get all available styles from QStyleFactory
    QStringList availableStyles = QStyleFactory::keys();
    
    // Generate test cases for each available style (run multiple times for PBT coverage)
    // This verifies we test all available styles on the current platform
    for (int iteration = 0; iteration < 100; ++iteration) {
        for (const QString &style : availableStyles) {
            QTest::newRow(qPrintable(QString("style_%1_iter_%2").arg(style).arg(iteration))) << style;
        }
    }
    
    // Edge cases
    QTest::newRow("empty_style") << QString();
}

void TestSettingsManager::testBaseStyleRoundTrip()
{
    // Feature: settings-persistence, Property 2: Base Style Round-Trip
    
    QFETCH(QString, styleName);
    
    // Create first SettingsManager and save the base style
    {
        SettingsManager settingsManager;
        settingsManager.saveBaseStyle(styleName);
        
        // Verify hasBaseStyle returns true after saving (unless empty)
        if (!styleName.isEmpty()) {
            QVERIFY2(settingsManager.hasBaseStyle(),
                     "hasBaseStyle() should return true after saving non-empty style");
        }
    }
    
    // Create a new SettingsManager instance - it should load the saved style
    {
        SettingsManager settingsManager;
        QString loadedStyle = settingsManager.loadBaseStyle();
        
        // Property: The loaded style should equal the saved style
        QVERIFY2(loadedStyle == styleName,
                 qPrintable(QString("Round-trip failed: saved '%1', loaded '%2'")
                           .arg(styleName)
                           .arg(loadedStyle)));
    }
}

/**
 * Feature: settings-persistence, Property 3: Splitter State Round-Trip
 * 
 * For any valid splitter state (list of panel widths), saving the state
 * and then loading it should return an equivalent state.
 */
void TestSettingsManager::testSplitterStateRoundTrip_data()
{
    QTest::addColumn<QByteArray>("splitterState");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100+ test cases with random splitter state data
    for (int i = 0; i < 100; ++i) {
        // Generate random splitter state byte array (simulating QSplitter::saveState() output)
        // Real splitter state has a specific format, but for round-trip testing,
        // any byte array should work since we're testing storage/retrieval
        int size = rng->bounded(20, 100);  // Typical splitter state size range
        QByteArray state;
        state.resize(size);
        for (int j = 0; j < size; ++j) {
            state[j] = static_cast<char>(rng->bounded(256));
        }
        
        QTest::newRow(qPrintable(QString("splitter_state_iteration_%1").arg(i))) << state;
    }
    
    // Edge cases
    QTest::newRow("empty_state") << QByteArray();
    QTest::newRow("single_byte") << QByteArray(1, 'S');
    QTest::newRow("large_state") << QByteArray(500, 'Y');
}

void TestSettingsManager::testSplitterStateRoundTrip()
{
    // Feature: settings-persistence, Property 3: Splitter State Round-Trip
    
    QFETCH(QByteArray, splitterState);
    
    // Create first SettingsManager and save the splitter state
    {
        SettingsManager settingsManager;
        settingsManager.saveSplitterState(splitterState);
        
        // Verify hasSplitterState returns true after saving (unless empty)
        if (!splitterState.isEmpty()) {
            QVERIFY2(settingsManager.hasSplitterState(),
                     "hasSplitterState() should return true after saving non-empty state");
        }
    }
    
    // Create a new SettingsManager instance - it should load the saved state
    {
        SettingsManager settingsManager;
        QByteArray loadedState = settingsManager.loadSplitterState();
        
        // Property: The loaded state should equal the saved state
        QVERIFY2(loadedState == splitterState,
                 qPrintable(QString("Round-trip failed: saved %1 bytes, loaded %2 bytes")
                           .arg(splitterState.size())
                           .arg(loadedState.size())));
    }
}

/**
 * Unit test: Dock state save stores data correctly
 * 
 * Tests that saveDockState() correctly stores dock state data.
 */
void TestSettingsManager::testDockStateSaveStoresData()
{
    SettingsManager settingsManager;
    
    // Create test dock state data (simulating QMainWindow::saveState() output)
    QByteArray testState("test_dock_state_data_12345");
    
    // Save the dock state
    settingsManager.saveDockState(testState);
    
    // Verify the data was stored by loading it back
    QByteArray loadedState = settingsManager.loadDockState();
    QVERIFY2(loadedState == testState,
             qPrintable(QString("saveDockState() did not store data correctly: expected '%1', got '%2'")
                       .arg(QString::fromLatin1(testState))
                       .arg(QString::fromLatin1(loadedState))));
}

/**
 * Unit test: Dock state load retrieves stored data
 * 
 * Tests that loadDockState() correctly retrieves previously saved dock state data.
 */
void TestSettingsManager::testDockStateLoadRetrievesData()
{
    // Create test dock state data
    QByteArray testState("dock_state_for_retrieval_test");
    
    // Save using one SettingsManager instance
    {
        SettingsManager settingsManager;
        settingsManager.saveDockState(testState);
    }
    
    // Load using a new SettingsManager instance to verify persistence
    {
        SettingsManager settingsManager;
        QByteArray loadedState = settingsManager.loadDockState();
        
        QVERIFY2(loadedState == testState,
                 qPrintable(QString("loadDockState() did not retrieve stored data: expected '%1', got '%2'")
                           .arg(QString::fromLatin1(testState))
                           .arg(QString::fromLatin1(loadedState))));
    }
}

/**
 * Unit test: hasDockState returns correct boolean
 * 
 * Tests that hasDockState() returns false when no state exists and true after saving.
 */
void TestSettingsManager::testHasDockStateReturnsCorrectBoolean()
{
    SettingsManager settingsManager;
    
    // Initially should return false (no state saved)
    QVERIFY2(!settingsManager.hasDockState(),
             "hasDockState() should return false when no dock state has been saved");
    
    // Save some dock state
    QByteArray testState("some_dock_state");
    settingsManager.saveDockState(testState);
    
    // Now should return true
    QVERIFY2(settingsManager.hasDockState(),
             "hasDockState() should return true after saving dock state");
    
    // Verify with a new instance to ensure persistence
    {
        SettingsManager newSettingsManager;
        QVERIFY2(newSettingsManager.hasDockState(),
                 "hasDockState() should return true in new instance after saving dock state");
    }
}

/**
 * Unit test: Empty dock state handling
 * 
 * Tests that saving and loading empty QByteArray works correctly.
 */
void TestSettingsManager::testEmptyDockStateHandling()
{
    SettingsManager settingsManager;
    
    // Save empty state
    QByteArray emptyState;
    settingsManager.saveDockState(emptyState);
    
    // Load should return empty QByteArray
    QByteArray loadedState = settingsManager.loadDockState();
    QVERIFY2(loadedState.isEmpty(),
             "loadDockState() should return empty QByteArray when empty state was saved");
    
    // hasDockState should return true because the key exists (even if value is empty)
    QVERIFY2(settingsManager.hasDockState(),
             "hasDockState() should return true even when empty state was saved (key exists)");
}

/**
 * Feature: dockable-panels, Property: Dock State Round-Trip Persistence
 * 
 * For any valid dock state (QByteArray), saving the state and then loading it
 * should return an equivalent state.
 */
void TestSettingsManager::testDockStateRoundTrip_data()
{
    QTest::addColumn<QByteArray>("dockState");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100+ test cases with random dock state data
    for (int i = 0; i < 100; ++i) {
        // Generate random dock state byte array (simulating QMainWindow::saveState() output)
        // Real dock state has a specific format, but for round-trip testing,
        // any byte array should work since we're testing storage/retrieval
        int size = rng->bounded(50, 300);  // Typical dock state size range
        QByteArray state;
        state.resize(size);
        for (int j = 0; j < size; ++j) {
            state[j] = static_cast<char>(rng->bounded(256));
        }
        
        QTest::newRow(qPrintable(QString("dock_state_iteration_%1").arg(i))) << state;
    }
    
    // Edge cases
    QTest::newRow("empty_state") << QByteArray();
    QTest::newRow("single_byte") << QByteArray(1, 'D');
    QTest::newRow("large_state") << QByteArray(1000, 'Z');
    QTest::newRow("binary_data") << QByteArray("\x00\x01\x02\xFF\xFE\xFD", 6);
}

void TestSettingsManager::testDockStateRoundTrip()
{
    // Feature: dockable-panels, Property: Dock State Round-Trip Persistence
    
    QFETCH(QByteArray, dockState);
    
    // Create first SettingsManager and save the dock state
    {
        SettingsManager settingsManager;
        settingsManager.saveDockState(dockState);
        
        // Verify hasDockState returns true after saving (key exists even if empty)
        QVERIFY2(settingsManager.hasDockState(),
                 "hasDockState() should return true after saving state");
    }
    
    // Create a new SettingsManager instance - it should load the saved state
    {
        SettingsManager settingsManager;
        QByteArray loadedState = settingsManager.loadDockState();
        
        // Property: The loaded state should equal the saved state
        QVERIFY2(loadedState == dockState,
                 qPrintable(QString("Round-trip failed: saved %1 bytes, loaded %2 bytes")
                           .arg(dockState.size())
                           .arg(loadedState.size())));
    }
}

/**
 * Feature: settings-persistence, Property 4: Window Geometry Round-Trip
 * 
 * For any valid window geometry, saving the geometry and then loading it
 * should return the same geometry bytes.
 */
void TestSettingsManager::testWindowGeometryRoundTrip_data()
{
    QTest::addColumn<QByteArray>("geometry");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100+ test cases with random geometry data
    for (int i = 0; i < 100; ++i) {
        // Generate random geometry byte array (simulating QWidget::saveGeometry() output)
        // Real geometry data has a specific format, but for round-trip testing,
        // any byte array should work since we're testing storage/retrieval
        int size = rng->bounded(50, 200);  // Typical geometry size range
        QByteArray geometry;
        geometry.resize(size);
        for (int j = 0; j < size; ++j) {
            geometry[j] = static_cast<char>(rng->bounded(256));
        }
        
        QTest::newRow(qPrintable(QString("geometry_iteration_%1").arg(i))) << geometry;
    }
    
    // Edge cases
    QTest::newRow("empty_geometry") << QByteArray();
    QTest::newRow("single_byte") << QByteArray(1, 'A');
    QTest::newRow("large_geometry") << QByteArray(1000, 'X');
}

void TestSettingsManager::testWindowGeometryRoundTrip()
{
    // Feature: settings-persistence, Property 4: Window Geometry Round-Trip
    
    QFETCH(QByteArray, geometry);
    
    // Create first SettingsManager and save the geometry
    {
        SettingsManager settingsManager;
        settingsManager.saveWindowGeometry(geometry);
        
        // Verify hasWindowGeometry returns true after saving (unless empty)
        if (!geometry.isEmpty()) {
            QVERIFY2(settingsManager.hasWindowGeometry(),
                     "hasWindowGeometry() should return true after saving non-empty geometry");
        }
    }
    
    // Create a new SettingsManager instance - it should load the saved geometry
    {
        SettingsManager settingsManager;
        QByteArray loadedGeometry = settingsManager.loadWindowGeometry();
        
        // Property: The loaded geometry should equal the saved geometry
        QVERIFY2(loadedGeometry == geometry,
                 qPrintable(QString("Round-trip failed: saved %1 bytes, loaded %2 bytes")
                           .arg(geometry.size())
                           .arg(loadedGeometry.size())));
    }
}


/**
 * Feature: settings-persistence, Property 5: Recent Projects List Ordering and Cap
 * 
 * For any sequence of project additions, the recent projects list should:
 * - Contain at most MaxRecentProjects (10) entries
 * - Have the most recently added project at the front
 * - Not contain duplicates (adding an existing project moves it to front)
 */
void TestSettingsManager::testRecentProjectsOrderingAndCap_data()
{
    QTest::addColumn<QStringList>("projectsToAdd");
    QTest::addColumn<QString>("expectedFirst");
    QTest::addColumn<int>("expectedMaxSize");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100+ test cases with random project sequences
    for (int i = 0; i < 100; ++i) {
        QStringList projects;
        int numProjects = rng->bounded(1, 20);  // Add 1-19 projects
        
        for (int j = 0; j < numProjects; ++j) {
            // Generate random project path
            QString path = QString("/path/to/project_%1_%2.qvp")
                          .arg(i)
                          .arg(rng->bounded(1000));
            projects.append(path);
        }
        
        // The last added project should be first in the list
        QString expectedFirst = projects.last();
        
        // Calculate expected size: count unique projects (in order of last occurrence), capped at 10
        QStringList uniqueProjects;
        for (int k = projects.size() - 1; k >= 0; --k) {
            if (!uniqueProjects.contains(projects[k])) {
                uniqueProjects.prepend(projects[k]);
            }
        }
        int expectedMaxSize = qMin(uniqueProjects.size(), SettingsManager::MaxRecentProjects);
        
        QTest::newRow(qPrintable(QString("random_sequence_%1").arg(i))) 
            << projects << expectedFirst << expectedMaxSize;
    }
    
    // Test duplicate handling - adding same project moves it to front
    QStringList duplicateTest = {"/a.qvp", "/b.qvp", "/c.qvp", "/a.qvp"};
    QTest::newRow("duplicate_moves_to_front") << duplicateTest << QString("/a.qvp") << 3;
    
    // Test cap enforcement - adding more than 10 projects
    QStringList manyProjects;
    for (int i = 0; i < 15; ++i) {
        manyProjects.append(QString("/project_%1.qvp").arg(i));
    }
    QTest::newRow("cap_at_10") << manyProjects << QString("/project_14.qvp") << 10;
    
    // Edge case: single project
    QTest::newRow("single_project") << QStringList{"/single.qvp"} << QString("/single.qvp") << 1;
    
    // Edge case: empty path (should still be added)
    QTest::newRow("empty_path") << QStringList{""} << QString("") << 1;
}

void TestSettingsManager::testRecentProjectsOrderingAndCap()
{
    // Feature: settings-persistence, Property 5: Recent Projects List Ordering and Cap
    
    QFETCH(QStringList, projectsToAdd);
    QFETCH(QString, expectedFirst);
    QFETCH(int, expectedMaxSize);
    
    // Clear any existing recent projects first using the correct settings
    {
        QSettings settings(QStringLiteral("QtVanity"), QStringLiteral("QtVanity"));
        settings.remove(QStringLiteral("recentProjects"));
    }
    
    SettingsManager settingsManager;
    
    // Add all projects in sequence
    for (const QString &project : projectsToAdd) {
        settingsManager.addRecentProject(project);
    }
    
    QStringList recentProjects = settingsManager.recentProjects();
    
    // Property 1: List size should not exceed MaxRecentProjects (10)
    QVERIFY2(recentProjects.size() <= SettingsManager::MaxRecentProjects,
             qPrintable(QString("List size %1 exceeds max %2")
                       .arg(recentProjects.size())
                       .arg(SettingsManager::MaxRecentProjects)));
    
    // Property 2: Most recently added project should be at front
    if (!recentProjects.isEmpty()) {
        QVERIFY2(recentProjects.first() == expectedFirst,
                 qPrintable(QString("Expected first '%1', got '%2'")
                           .arg(expectedFirst)
                           .arg(recentProjects.first())));
    }
    
    // Property 3: No duplicates in the list
    QSet<QString> uniqueSet(recentProjects.begin(), recentProjects.end());
    QVERIFY2(uniqueSet.size() == recentProjects.size(),
             qPrintable(QString("List contains duplicates: %1 unique vs %2 total")
                       .arg(uniqueSet.size())
                       .arg(recentProjects.size())));
    
    // Property 4: Size should match expected (accounting for duplicates and cap)
    QVERIFY2(recentProjects.size() == expectedMaxSize,
             qPrintable(QString("Expected size %1, got %2")
                       .arg(expectedMaxSize)
                       .arg(recentProjects.size())));
}


/**
 * Feature: custom-widget-plugin, Property 12: Settings Storage Round-Trip
 * 
 * For any valid directory path set as the Plugin_Directory in SettingsManager,
 * retrieving the Plugin_Directory should return the same path.
 */
void TestSettingsManager::testPluginDirectoryRoundTrip_data()
{
    QTest::addColumn<QString>("directoryPath");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100+ test cases with random directory paths
    for (int i = 0; i < 100; ++i) {
        // Generate random directory path components
        int numComponents = rng->bounded(1, 6);  // 1-5 path components
        QString path;
        
        // Start with root or drive letter
#ifdef Q_OS_WIN
        path = QString("C:");
#else
        path = QString();
#endif
        
        for (int j = 0; j < numComponents; ++j) {
            // Generate random directory name (alphanumeric + underscore)
            int nameLength = rng->bounded(3, 15);
            QString dirName;
            for (int k = 0; k < nameLength; ++k) {
                int charType = rng->bounded(3);
                if (charType == 0) {
                    dirName += QChar('a' + rng->bounded(26));  // lowercase letter
                } else if (charType == 1) {
                    dirName += QChar('A' + rng->bounded(26));  // uppercase letter
                } else {
                    dirName += QChar('0' + rng->bounded(10));  // digit
                }
            }
            path += "/" + dirName;
        }
        
        QTest::newRow(qPrintable(QString("random_path_%1").arg(i))) << path;
    }
    
    // Edge cases - various valid directory path formats
    QTest::newRow("simple_path") << QString("/home/user/plugins");
    QTest::newRow("nested_path") << QString("/usr/local/share/qtvanity/plugins");
    QTest::newRow("path_with_numbers") << QString("/opt/app123/plugins456");
    QTest::newRow("path_with_underscores") << QString("/my_app/my_plugins");
    QTest::newRow("path_with_dots") << QString("/home/user/.config/qtvanity/plugins");
    QTest::newRow("single_component") << QString("/plugins");
    
#ifdef Q_OS_WIN
    // Windows-specific paths
    QTest::newRow("windows_path") << QString("C:/Users/Test/AppData/Local/QtVanity/plugins");
    QTest::newRow("windows_program_files") << QString("C:/Program Files/QtVanity/plugins");
#endif
    
    // Path with spaces (valid on most filesystems)
    QTest::newRow("path_with_spaces") << QString("/home/user/My Plugins");
    
    // Unicode path (valid on modern filesystems)
    QTest::newRow("unicode_path") << QString("/home/user/плагины");
}

void TestSettingsManager::testPluginDirectoryRoundTrip()
{
    // Feature: custom-widget-plugin, Property 12: Settings Storage Round-Trip
    
    QFETCH(QString, directoryPath);
    
    // Create first SettingsManager and set the plugin directory
    {
        SettingsManager settingsManager;
        settingsManager.setPluginDirectory(directoryPath);
    }
    
    // Create a new SettingsManager instance - it should return the saved directory
    {
        SettingsManager settingsManager;
        QString loadedPath = settingsManager.pluginDirectory();
        
        // Property: The loaded path should equal the saved path
        QVERIFY2(loadedPath == directoryPath,
                 qPrintable(QString("Round-trip failed: saved '%1', loaded '%2'")
                           .arg(directoryPath)
                           .arg(loadedPath)));
    }
}


/**
 * Feature: custom-widget-plugin, Property 13: Directory Change Triggers Rescan
 * 
 * For any change to the Plugin_Directory setting, the SettingsManager SHALL emit
 * the pluginDirectoryChanged() signal. When the same path is set again (idempotency),
 * no signal should be emitted.
 */
void TestSettingsManager::testDirectoryChangeTriggerSignal_data()
{
    QTest::addColumn<QString>("initialPath");
    QTest::addColumn<QString>("newPath");
    QTest::addColumn<bool>("expectSignal");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100+ test cases with random directory path changes
    for (int i = 0; i < 100; ++i) {
        // Generate two different random directory paths
        auto generateRandomPath = [&rng]() -> QString {
            int numComponents = rng->bounded(1, 6);  // 1-5 path components
            QString path;
            
#ifdef Q_OS_WIN
            path = QString("C:");
#else
            path = QString();
#endif
            
            for (int j = 0; j < numComponents; ++j) {
                int nameLength = rng->bounded(3, 15);
                QString dirName;
                for (int k = 0; k < nameLength; ++k) {
                    int charType = rng->bounded(3);
                    if (charType == 0) {
                        dirName += QChar('a' + rng->bounded(26));
                    } else if (charType == 1) {
                        dirName += QChar('A' + rng->bounded(26));
                    } else {
                        dirName += QChar('0' + rng->bounded(10));
                    }
                }
                path += "/" + dirName;
            }
            return path;
        };
        
        QString initialPath = generateRandomPath();
        QString newPath = generateRandomPath();
        
        // Ensure paths are different for the "change" test cases
        while (newPath == initialPath) {
            newPath = generateRandomPath();
        }
        
        // Test case: changing to a different path should emit signal
        QTest::newRow(qPrintable(QString("different_path_%1").arg(i))) 
            << initialPath << newPath << true;
    }
    
    // Generate 50 test cases for idempotency (same path, no signal)
    for (int i = 0; i < 50; ++i) {
        auto generateRandomPath = [&rng]() -> QString {
            int numComponents = rng->bounded(1, 6);
            QString path;
            
#ifdef Q_OS_WIN
            path = QString("C:");
#else
            path = QString();
#endif
            
            for (int j = 0; j < numComponents; ++j) {
                int nameLength = rng->bounded(3, 15);
                QString dirName;
                for (int k = 0; k < nameLength; ++k) {
                    int charType = rng->bounded(3);
                    if (charType == 0) {
                        dirName += QChar('a' + rng->bounded(26));
                    } else if (charType == 1) {
                        dirName += QChar('A' + rng->bounded(26));
                    } else {
                        dirName += QChar('0' + rng->bounded(10));
                    }
                }
                path += "/" + dirName;
            }
            return path;
        };
        
        QString samePath = generateRandomPath();
        
        // Test case: setting the same path should NOT emit signal (idempotency)
        QTest::newRow(qPrintable(QString("same_path_%1").arg(i))) 
            << samePath << samePath << false;
    }
    
    // Edge cases - different paths
    QTest::newRow("simple_change") 
        << QString("/home/user/plugins") 
        << QString("/home/user/other_plugins") 
        << true;
    
    QTest::newRow("nested_to_simple") 
        << QString("/usr/local/share/qtvanity/plugins") 
        << QString("/plugins") 
        << true;
    
    QTest::newRow("simple_to_nested") 
        << QString("/plugins") 
        << QString("/usr/local/share/qtvanity/plugins") 
        << true;
    
    // Edge cases - same paths (idempotency)
    QTest::newRow("same_simple_path") 
        << QString("/home/user/plugins") 
        << QString("/home/user/plugins") 
        << false;
    
    QTest::newRow("same_nested_path") 
        << QString("/usr/local/share/qtvanity/plugins") 
        << QString("/usr/local/share/qtvanity/plugins") 
        << false;
    
    QTest::newRow("same_path_with_spaces") 
        << QString("/home/user/My Plugins") 
        << QString("/home/user/My Plugins") 
        << false;
    
#ifdef Q_OS_WIN
    // Windows-specific edge cases
    QTest::newRow("windows_different_paths") 
        << QString("C:/Users/Test/plugins") 
        << QString("D:/Plugins") 
        << true;
    
    QTest::newRow("windows_same_path") 
        << QString("C:/Users/Test/AppData/Local/QtVanity/plugins") 
        << QString("C:/Users/Test/AppData/Local/QtVanity/plugins") 
        << false;
#endif
}

void TestSettingsManager::testDirectoryChangeTriggerSignal()
{
    // Feature: custom-widget-plugin, Property 13: Directory Change Triggers Rescan
    
    QFETCH(QString, initialPath);
    QFETCH(QString, newPath);
    QFETCH(bool, expectSignal);
    
    // Clear any existing plugin directory setting
    {
        QSettings settings(QStringLiteral("QtVanity"), QStringLiteral("QtVanity"));
        settings.remove(QStringLiteral("plugins/directory"));
    }
    
    SettingsManager settingsManager;
    
    // Set the initial path first
    settingsManager.setPluginDirectory(initialPath);
    
    // Verify initial path was set correctly
    QVERIFY2(settingsManager.pluginDirectory() == initialPath,
             qPrintable(QString("Initial path not set correctly: expected '%1', got '%2'")
                       .arg(initialPath)
                       .arg(settingsManager.pluginDirectory())));
    
    // Set up signal spy to monitor pluginDirectoryChanged signal
    QSignalSpy spy(&settingsManager, &SettingsManager::pluginDirectoryChanged);
    QVERIFY2(spy.isValid(), "Signal spy should be valid");
    
    // Now set the new path (which may be the same or different)
    settingsManager.setPluginDirectory(newPath);
    
    // Verify the path is now set to newPath
    QVERIFY2(settingsManager.pluginDirectory() == newPath,
             qPrintable(QString("New path not set correctly: expected '%1', got '%2'")
                       .arg(newPath)
                       .arg(settingsManager.pluginDirectory())));
    
    if (expectSignal) {
        // Property: When directory changes, signal MUST be emitted
        QVERIFY2(spy.count() == 1,
                 qPrintable(QString("Expected pluginDirectoryChanged signal to be emitted once when "
                                   "changing from '%1' to '%2', but signal count was %3")
                           .arg(initialPath)
                           .arg(newPath)
                           .arg(spy.count())));
    } else {
        // Property: When directory is the same (idempotency), signal MUST NOT be emitted
        QVERIFY2(spy.count() == 0,
                 qPrintable(QString("Expected NO pluginDirectoryChanged signal when setting same path '%1', "
                                   "but signal was emitted %2 time(s)")
                           .arg(newPath)
                           .arg(spy.count())));
    }
}
