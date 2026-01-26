#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

#include "PluginMetadata.h"

class QPluginLoader;
class WidgetPluginInterface;

/**
 * @brief Manages plugin discovery, loading, and lifecycle.
 * 
 * PluginManager is the core component of QtVanity's plugin system. It handles:
 * - Discovering plugin files in the configured plugin directory
 * - Loading plugins using Qt's QPluginLoader mechanism
 * - Verifying plugins implement the WidgetPluginInterface
 * - Extracting metadata from loaded plugins
 * - Managing plugin lifecycle (load, unload, refresh)
 * - Error handling and reporting for failed plugin loads
 * 
 * The PluginManager supports platform-specific plugin extensions:
 * - Linux: .so
 * - Windows: .dll
 * - macOS: .dylib
 * 
 * Usage:
 * @code
 * PluginManager *manager = new PluginManager(this);
 * manager->setPluginDirectory("/path/to/plugins");
 * manager->loadPlugins();
 * 
 * // Access loaded plugins
 * for (const PluginMetadata &meta : manager->loadedPlugins()) {
 *     if (meta.isValid) {
 *         WidgetPluginInterface *plugin = manager->pluginInterface(meta.name);
 *         // Use plugin...
 *     }
 * }
 * @endcode
 * 
 * @see WidgetPluginInterface
 * @see PluginMetadata
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a PluginManager.
     * 
     * @param parent The parent QObject.
     */
    explicit PluginManager(QObject *parent = nullptr);
    
    /**
     * @brief Destructor.
     * 
     * Unloads all plugins and cleans up resources.
     */
    ~PluginManager();
    
    // -------------------------------------------------------------------------
    // Configuration
    // -------------------------------------------------------------------------
    
    /**
     * @brief Sets the directory path where plugins are located.
     * 
     * This directory will be scanned for plugin files when loadPlugins()
     * or refreshPlugins() is called. The directory will be created if it
     * does not exist.
     * 
     * @param path The absolute or relative path to the plugin directory.
     */
    void setPluginDirectory(const QString &path);
    
    /**
     * @brief Returns the current plugin directory path.
     * 
     * @return The path to the plugin directory.
     */
    QString pluginDirectory() const;
    
    // -------------------------------------------------------------------------
    // Plugin Operations
    // -------------------------------------------------------------------------
    
    /**
     * @brief Loads all plugins from the plugin directory.
     * 
     * Scans the plugin directory for files with valid plugin extensions
     * and attempts to load each one. Successfully loaded plugins are
     * stored in the internal registry. Failed plugins are logged with
     * error messages.
     * 
     * Emits pluginLoaded() for each successfully loaded plugin,
     * pluginLoadError() for each failed plugin, and pluginsLoaded()
     * when all plugins have been processed.
     * 
     * @see refreshPlugins()
     * @see unloadPlugins()
     */
    void loadPlugins();
    
    /**
     * @brief Refreshes all plugins by unloading and reloading.
     * 
     * This method unloads all currently loaded plugins, then rescans
     * the plugin directory and loads all discovered plugins. Useful
     * for picking up new plugins or reloading modified plugins without
     * restarting the application.
     * 
     * Emits pluginsUnloaded() after unloading, then the same signals
     * as loadPlugins() during the reload process.
     * 
     * @see loadPlugins()
     * @see unloadPlugins()
     */
    void refreshPlugins();
    
    /**
     * @brief Unloads all currently loaded plugins.
     * 
     * Clears the plugin registry and releases all plugin resources.
     * Emits pluginsUnloaded() when complete.
     * 
     * @see loadPlugins()
     * @see refreshPlugins()
     */
    void unloadPlugins();
    
    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------
    
    /**
     * @brief Returns metadata for all discovered plugins.
     * 
     * This includes both successfully loaded plugins (isValid == true)
     * and plugins that failed to load (isValid == false with errorMessage).
     * 
     * @return A list of PluginMetadata for all discovered plugins.
     */
    QList<PluginMetadata> loadedPlugins() const;
    
    /**
     * @brief Returns all error messages from plugin loading.
     * 
     * Contains error messages for all plugins that failed to load,
     * including the filename and reason for failure.
     * 
     * @return A list of error message strings.
     */
    QList<QString> loadingErrors() const;
    
    /**
     * @brief Returns the plugin interface for a loaded plugin.
     * 
     * Retrieves the WidgetPluginInterface for a plugin by its widget name.
     * Returns nullptr if no plugin with the given name is loaded.
     * 
     * @param name The widget name (from widgetName()) of the plugin.
     * @return Pointer to the WidgetPluginInterface, or nullptr if not found.
     */
    WidgetPluginInterface* pluginInterface(const QString &name) const;
    
    // -------------------------------------------------------------------------
    // Utility
    // -------------------------------------------------------------------------
    
    /**
     * @brief Returns the list of supported plugin file extensions.
     * 
     * Returns platform-specific extensions:
     * - Linux: {"so"}
     * - Windows: {"dll"}
     * - macOS: {"dylib"}
     * 
     * @return A list of supported file extensions (without the dot).
     */
    static QStringList supportedExtensions();
    
    /**
     * @brief Checks if a file has a valid plugin extension.
     * 
     * Determines whether a file path has a platform-appropriate plugin extension.
     * This method is used during plugin discovery to filter out non-plugin files.
     * 
     * Platform-specific extensions:
     * - Linux: .so
     * - Windows: .dll
     * - macOS: .dylib
     * 
     * The check is case-insensitive.
     * 
     * @param filePath The path to the file to check.
     * @return True if the file has a valid plugin extension for the current platform.
     */
    bool isPluginFile(const QString &filePath) const;

signals:
    /**
     * @brief Emitted when all plugins have been loaded.
     * 
     * This signal is emitted after loadPlugins() or refreshPlugins()
     * has finished processing all plugin files in the directory.
     */
    void pluginsLoaded();
    
    /**
     * @brief Emitted when a single plugin is successfully loaded.
     * 
     * @param metadata The metadata for the loaded plugin.
     */
    void pluginLoaded(const PluginMetadata &metadata);
    
    /**
     * @brief Emitted when a plugin fails to load.
     * 
     * @param filePath The path to the plugin file that failed to load.
     * @param error A description of the error that occurred.
     */
    void pluginLoadError(const QString &filePath, const QString &error);
    
    /**
     * @brief Emitted when all plugins have been unloaded.
     * 
     * This signal is emitted after unloadPlugins() or at the start
     * of refreshPlugins() when existing plugins are cleared.
     */
    void pluginsUnloaded();

private:
    /**
     * @brief Scans the plugin directory for plugin files.
     * 
     * Populates the internal list of plugin file paths to be loaded.
     */
    void scanDirectory();
    
    /**
     * @brief Attempts to load a single plugin file.
     * 
     * @param filePath The path to the plugin file.
     * @return True if the plugin was loaded successfully, false otherwise.
     */
    bool loadPlugin(const QString &filePath);
    
    QString m_pluginDirectory;                      ///< Path to the plugin directory
    QList<QPluginLoader*> m_loaders;                ///< Active plugin loaders
    QMap<QString, WidgetPluginInterface*> m_plugins; ///< Name-to-interface mapping
    QList<PluginMetadata> m_metadata;               ///< Metadata for all discovered plugins
    QList<QString> m_errors;                        ///< Accumulated error messages
};

#endif // PLUGINMANAGER_H
