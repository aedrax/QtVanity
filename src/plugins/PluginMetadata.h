#ifndef PLUGINMETADATA_H
#define PLUGINMETADATA_H

#include <QString>
#include <QIcon>

/**
 * @brief Data structure storing extracted information about a loaded plugin.
 * 
 * PluginMetadata contains all relevant information about a plugin that has
 * been discovered by the PluginManager. This includes both successfully loaded
 * plugins and plugins that failed to load (with error information).
 * 
 * For successfully loaded plugins:
 * - isValid is true
 * - name, description, category, and icon are populated from the plugin interface
 * - errorMessage is empty
 * 
 * For failed plugins:
 * - isValid is false
 * - filePath contains the path to the failed plugin
 * - errorMessage contains details about the failure
 * - Other fields may be empty or default values
 */
struct PluginMetadata
{
    /**
     * @brief Full absolute path to the plugin file.
     * 
     * This is the complete filesystem path to the plugin file (.so, .dll, or .dylib).
     */
    QString filePath;
    
    /**
     * @brief Display name of the widget.
     * 
     * Extracted from the plugin's widgetName() method. This name is shown
     * in the Custom Widgets gallery page to identify the widget.
     */
    QString name;
    
    /**
     * @brief Description of the widget.
     * 
     * Extracted from the plugin's widgetDescription() method. Provides
     * information about the widget's purpose and usage.
     */
    QString description;
    
    /**
     * @brief Category for grouping widgets.
     * 
     * Extracted from the plugin's widgetCategory() method. If the plugin
     * returns an empty string, this is set to "Custom" by the PluginManager.
     * Widgets with the same category are grouped together in the gallery.
     */
    QString category;
    
    /**
     * @brief Optional icon for the widget.
     * 
     * Extracted from the plugin's widgetIcon() method. May be an empty
     * QIcon if the plugin does not provide an icon.
     */
    QIcon icon;
    
    /**
     * @brief Indicates whether the plugin loaded successfully.
     * 
     * True if the plugin was loaded and verified to implement the
     * WidgetPluginInterface correctly. False if loading failed for
     * any reason (file not found, corrupted, interface mismatch, etc.).
     */
    bool isValid = false;
    
    /**
     * @brief Error message if loading failed.
     * 
     * Contains a descriptive error message when isValid is false.
     * Empty string when the plugin loaded successfully.
     */
    QString errorMessage;
};

#endif // PLUGINMETADATA_H
