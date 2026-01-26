#ifndef CUSTOMWIDGETSPAGE_H
#define CUSTOMWIDGETSPAGE_H

#include "GalleryPage.h"

#include <QList>

class PluginManager;
class PluginMetadata;
class QWidget;

/**
 * @brief Gallery page that displays custom widgets from loaded plugins.
 * 
 * CustomWidgetsPage is a specialized GalleryPage that displays widgets
 * loaded from external Qt plugins via the PluginManager. It provides:
 * - Display of all widgets from loaded plugins
 * - Grouping of widgets by their category metadata
 * - Display of each widget's name and description
 * - A helpful message when no plugins are loaded
 * - Support for enabling/disabling all custom widgets
 * - Automatic rebuilding when plugins are refreshed
 * 
 * The page integrates with the PluginManager to:
 * - Retrieve loaded plugin metadata
 * - Create widget instances using plugin interfaces
 * - Respond to plugin refresh events
 * 
 * Usage:
 * @code
 * PluginManager *pluginManager = new PluginManager(this);
 * pluginManager->loadPlugins();
 * 
 * CustomWidgetsPage *page = new CustomWidgetsPage(pluginManager, tabWidget);
 * tabWidget->addTab(page, tr("Custom Widgets"));
 * 
 * // Connect for automatic updates
 * connect(pluginManager, &PluginManager::pluginsLoaded,
 *         page, &CustomWidgetsPage::rebuildWidgets);
 * @endcode
 * 
 * @see GalleryPage
 * @see PluginManager
 * @see WidgetPluginInterface
 */
class CustomWidgetsPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a CustomWidgetsPage.
     * 
     * @param pluginManager Pointer to the PluginManager that provides
     *                      access to loaded plugins. Must not be nullptr.
     * @param parent The parent widget.
     */
    explicit CustomWidgetsPage(PluginManager *pluginManager, 
                               QWidget *parent = nullptr);
    
    /**
     * @brief Destructor.
     */
    ~CustomWidgetsPage();
    
    /**
     * @brief Enables or disables all custom widgets on this page.
     * 
     * Iterates through all widget instances created from plugins and
     * sets their enabled state. This allows testing how custom widgets
     * appear in both enabled and disabled states.
     * 
     * @param enabled True to enable widgets, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

public slots:
    /**
     * @brief Rebuilds the widget display from current plugin state.
     * 
     * Clears all existing widget displays and recreates them based on
     * the current state of the PluginManager. This slot should be
     * connected to PluginManager::pluginsLoaded() for automatic updates
     * when plugins are refreshed.
     */
    void rebuildWidgets();

private:
    /**
     * @brief Sets up the initial widget display.
     * 
     * Called during construction to create the initial layout and
     * populate it with widgets from loaded plugins.
     */
    void setupWidgets();
    
    /**
     * @brief Sets up the empty state display.
     * 
     * Shows a helpful message explaining how to add plugins when
     * no plugins are currently loaded.
     */
    void setupEmptyState();
    
    /**
     * @brief Creates a widget group for a category.
     * 
     * Creates a group box containing all widgets from plugins in
     * the specified category.
     * 
     * @param category The category name for the group.
     * @param plugins List of plugin metadata for widgets in this category.
     */
    void createWidgetGroup(const QString &category, 
                           const QList<PluginMetadata> &plugins);
    
    /**
     * @brief Creates the display for a single plugin widget.
     * 
     * Creates a container with the widget's name, description, and
     * an instance of the widget itself. If widget creation fails,
     * displays an error placeholder instead.
     * 
     * @param metadata The metadata for the plugin to display.
     * @return Pointer to the created display container widget.
     */
    QWidget* createWidgetDisplay(const PluginMetadata &metadata);

    PluginManager *m_pluginManager;     ///< Plugin manager providing access to plugins
    QList<QWidget*> m_customWidgets;    ///< List of created custom widget instances
};

#endif // CUSTOMWIDGETSPAGE_H
