#ifndef TEST_PLUGINMANAGER_H
#define TEST_PLUGINMANAGER_H

#include <QObject>
#include <QtTest>

/**
 * @brief Test class for PluginManager functionality.
 * 
 * Contains both unit tests and property-based tests for the PluginManager class.
 */
class TestPluginManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Property-based tests
    
    /**
     * Property 1: Platform Extension Recognition
     * 
     * For any filename string, the Plugin_Manager SHALL recognize it as a plugin file
     * if and only if it has the correct platform-specific extension (.so on Linux,
     * .dll on Windows, .dylib on macOS).
     */
    void testPlatformExtensionRecognition();
    void testPlatformExtensionRecognition_data();
    
    /**
     * Property 5: Default Category Assignment
     * 
     * For any loaded plugin that returns an empty string from widgetCategory(),
     * the Plugin_Manager SHALL assign the category "Custom" in the PluginMetadata.
     */
    void testDefaultCategoryAssignment();
    void testDefaultCategoryAssignment_data();
    
    /**
     * Property 6: Error Logging for Failed Plugins
     * 
     * For any plugin file that fails to load (invalid file, missing interface, etc.),
     * the Plugin_Manager SHALL add an error entry containing the plugin's filename
     * to the loading errors list.
     */
    void testErrorLoggingForFailedPlugins();
    void testErrorLoggingForFailedPlugins_data();
    
    /**
     * Property 7: Continued Loading After Failure
     * 
     * For any set of plugin files where some are valid and some are invalid,
     * the Plugin_Manager SHALL attempt to load all plugins, successfully loading
     * all valid ones regardless of failures in others.
     * 
     * Since we cannot easily create real valid plugins for testing, this test verifies:
     * 1. When multiple invalid plugin files exist, all are attempted (multiple errors logged)
     * 2. The scanDirectory loop continues after each failure
     * 3. The number of errors matches the number of invalid files
     */
    void testContinuedLoadingAfterFailure();
    void testContinuedLoadingAfterFailure_data();
    
    /**
     * Property 11: Refresh Operation Completeness
     * 
     * For any refresh operation on the Plugin_Manager, all previously loaded plugins
     * SHALL be unloaded, the Plugin_Directory SHALL be rescanned, all discovered
     * plugins SHALL be reloaded, and the pluginsLoaded signal SHALL be emitted.
     * 
     * This test verifies:
     * 1. After refresh, pluginsUnloaded signal is emitted
     * 2. After refresh, pluginsLoaded signal is emitted
     * 3. The plugin directory is rescanned (errors are regenerated for invalid plugins)
     * 4. State is properly cleared and rebuilt
     */
    void testRefreshOperationCompleteness();
    void testRefreshOperationCompleteness_data();

private:
    /**
     * @brief Generates a random filename base (without extension).
     * 
     * Creates random filenames with various characteristics:
     * - Different lengths
     * - Alphanumeric characters
     * - Underscores and hyphens
     * - May include dots in the name (not as extension separator)
     * 
     * @return A random filename base string.
     */
    QString generateRandomFilenameBase();
};

#endif // TEST_PLUGINMANAGER_H
