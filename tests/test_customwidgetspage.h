#ifndef TEST_CUSTOMWIDGETSPAGE_H
#define TEST_CUSTOMWIDGETSPAGE_H

#include <QObject>
#include <QtTest>

class PluginManager;

/**
 * @brief Test class for CustomWidgetsPage functionality.
 * 
 * Contains both unit tests and property-based tests for the CustomWidgetsPage class.
 * Since we cannot easily create real plugins for testing, these tests verify
 * the page's behavior with mock data and by examining internal state.
 */
class TestCustomWidgetsPage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Unit tests
    
    /**
     * @brief Tests that CustomWidgetsPage can be created with a PluginManager.
     */
    void testPageCreation();
    
    /**
     * @brief Tests that the empty state is displayed when no plugins are loaded.
     */
    void testEmptyStateDisplay();
    
    /**
     * @brief Tests that rebuildWidgets() can be called without crashing.
     */
    void testRebuildWidgets();
    
    // Property-based tests
    
    /**
     * Property 8: Widget Display Completeness
     * 
     * For any set of loaded plugins, the Custom_Widgets_Page SHALL display
     * a widget instance for each plugin, and each display SHALL include
     * the widget's name and description text.
     * 
     * Since we cannot easily create real plugins, this test verifies:
     * 1. The page correctly processes plugin metadata
     * 2. For each valid plugin metadata, a display container is created
     * 3. Each display contains the widget's name and description as QLabel text
     * 
     * **Validates: Requirements 5.2, 5.4, 6.1**
     */
    void testWidgetDisplayCompleteness();
    void testWidgetDisplayCompleteness_data();
    
    /**
     * Property 9: Enabled State Propagation
     * 
     * For any Custom_Widgets_Page with loaded widgets, calling 
     * setWidgetsEnabled(enabled) SHALL set the enabled property of all 
     * custom widget instances to the specified value.
     * 
     * Since we cannot easily create real plugins, this test verifies:
     * 1. setWidgetsEnabled(false) can be called without errors
     * 2. setWidgetsEnabled(true) can be called without errors
     * 3. The method correctly propagates enabled state to all widgets
     *    in the m_customWidgets list
     * 4. Multiple consecutive calls with different states work correctly
     * 5. The enabled state is correctly applied regardless of the number
     *    of widgets or the order of calls
     * 
     * **Validates: Requirements 6.2**
     */
    void testEnabledStatePropagation();
    void testEnabledStatePropagation_data();

private:
    /**
     * @brief Generates random plugin metadata for testing.
     * 
     * Creates PluginMetadata with random names, descriptions, and categories.
     * 
     * @param isValid Whether the generated metadata should be marked as valid.
     * @return A randomly generated PluginMetadata structure.
     */
    struct PluginMetadata generateRandomPluginMetadata(bool isValid = true);
    
    /**
     * @brief Generates a random string of specified length.
     * 
     * @param minLength Minimum length of the string.
     * @param maxLength Maximum length of the string.
     * @return A random string.
     */
    QString generateRandomString(int minLength, int maxLength);
    
    /**
     * @brief Generates a random category name.
     * 
     * @return A random category name or empty string (for default category).
     */
    QString generateRandomCategory();
};

#endif // TEST_CUSTOMWIDGETSPAGE_H
