#ifndef WIDGETPLUGININTERFACE_H
#define WIDGETPLUGININTERFACE_H

#include <QtPlugin>
#include <QString>
#include <QIcon>
#include <QWidget>

/**
 * @brief Abstract interface for custom widget plugins.
 * 
 * WidgetPluginInterface defines the contract that all custom widget plugins
 * must implement to be compatible with QtVanity's plugin system. Plugins
 * implementing this interface can provide custom QWidget classes that will
 * be displayed in the Custom Widgets gallery page for QSS styling preview.
 * 
 * Required methods:
 * - createWidget(): Factory method to create widget instances
 * - widgetName(): Returns the display name for the widget
 * - widgetDescription(): Returns a description of the widget
 * 
 * Optional methods (with defaults):
 * - widgetIcon(): Returns an optional icon for the widget
 * - widgetCategory(): Returns an optional category for grouping widgets
 */
class WidgetPluginInterface
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~WidgetPluginInterface() = default;
    
    /**
     * @brief Creates a new instance of the custom widget.
     * 
     * This factory method is called by QtVanity to create widget instances
     * for display in the Custom Widgets gallery page. The plugin is responsible
     * for creating and returning a properly initialized widget.
     * 
     * @param parent The parent widget for the created widget.
     * @return A pointer to the newly created widget, or nullptr on failure.
     */
    virtual QWidget* createWidget(QWidget *parent = nullptr) = 0;
    
    /**
     * @brief Returns the display name of the widget.
     * 
     * This name is shown in the Custom Widgets gallery page to identify
     * the widget to users.
     * 
     * @return The widget's display name.
     */
    virtual QString widgetName() const = 0;
    
    /**
     * @brief Returns a description of the widget.
     * 
     * This description is shown in the Custom Widgets gallery page to
     * provide users with information about the widget's purpose and usage.
     * 
     * @return The widget's description.
     */
    virtual QString widgetDescription() const = 0;
    
    /**
     * @brief Returns an optional icon for the widget.
     * 
     * If provided, this icon may be displayed alongside the widget in
     * the Custom Widgets gallery page.
     * 
     * @return The widget's icon, or an empty QIcon if not provided.
     */
    virtual QIcon widgetIcon() const { return QIcon(); }
    
    /**
     * @brief Returns an optional category for grouping the widget.
     * 
     * Widgets with the same category are grouped together in the
     * Custom Widgets gallery page. If not provided (empty string),
     * the widget will be assigned to the default "Custom" category.
     * 
     * @return The widget's category, or an empty QString if not specified.
     */
    virtual QString widgetCategory() const { return QString(); }
};

#define WidgetPluginInterface_iid "com.qtvanity.WidgetPluginInterface/1.0"
Q_DECLARE_INTERFACE(WidgetPluginInterface, WidgetPluginInterface_iid)

#endif // WIDGETPLUGININTERFACE_H
