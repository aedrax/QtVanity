/**
 * @file ExampleWidgetPlugin.h
 * @brief Example custom widget plugin for QtVanity
 *
 * This file demonstrates how to create a custom widget plugin that implements
 * the WidgetPluginInterface. The plugin provides a gradient-styled button
 * widget that can be previewed in QtVanity's Custom Widgets gallery page.
 *
 * This serves as a reference implementation for developers creating their
 * own custom widget plugins for QtVanity.
 */

#ifndef EXAMPLEWIDGETPLUGIN_H
#define EXAMPLEWIDGETPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "WidgetPluginInterface.h"

/**
 * @brief Example plugin implementing WidgetPluginInterface
 *
 * ExampleWidgetPlugin demonstrates the complete implementation of a QtVanity
 * custom widget plugin. It provides:
 * - A custom GradientButton widget with visual styling
 * - Proper plugin metadata (name, description, category)
 * - Qt plugin export macros for dynamic loading
 *
 * Plugin developers can use this as a template for creating their own
 * custom widget plugins.
 */
class ExampleWidgetPlugin : public QObject, public WidgetPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID WidgetPluginInterface_iid FILE "examplewidgetplugin.json")
    Q_INTERFACES(WidgetPluginInterface)

public:
    /**
     * @brief Constructs the ExampleWidgetPlugin
     * @param parent Optional parent QObject
     */
    explicit ExampleWidgetPlugin(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ExampleWidgetPlugin() override = default;

    /**
     * @brief Creates a new instance of the custom widget
     *
     * This factory method creates a GradientButton widget instance.
     * The widget features a gradient background and demonstrates
     * how custom widgets can be styled with QSS.
     *
     * @param parent The parent widget for the created widget
     * @return A pointer to the newly created GradientButton widget
     */
    QWidget* createWidget(QWidget *parent = nullptr) override;

    /**
     * @brief Returns the display name of the widget
     * @return "Example Widget" - the widget's display name
     */
    QString widgetName() const override;

    /**
     * @brief Returns a description of the widget
     * @return A description explaining the widget's purpose
     */
    QString widgetDescription() const override;

    /**
     * @brief Returns an optional icon for the widget
     * @return An empty QIcon (no custom icon provided)
     */
    QIcon widgetIcon() const override;

    /**
     * @brief Returns the category for grouping the widget
     * @return "Examples" - the widget's category
     */
    QString widgetCategory() const override;
};

#endif // EXAMPLEWIDGETPLUGIN_H
