#include "CustomWidgetsPage.h"

#include "PluginManager.h"
#include "PluginMetadata.h"
#include "WidgetPluginInterface.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>

CustomWidgetsPage::CustomWidgetsPage(PluginManager *pluginManager, QWidget *parent)
    : GalleryPage(parent)
    , m_pluginManager(pluginManager)
{
    setupWidgets();
}

CustomWidgetsPage::~CustomWidgetsPage()
{
    // Qt handles child widget deletion
}

void CustomWidgetsPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_customWidgets) {
        widget->setEnabled(enabled);
    }
}

void CustomWidgetsPage::rebuildWidgets()
{
    // Clear existing custom widgets list
    m_customWidgets.clear();
    
    // Clear all existing content from the layout
    // Remove all widgets except the stretch at the end
    while (mainLayout()->count() > 1) {
        QLayoutItem *item = mainLayout()->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // Rebuild widgets from current plugin state
    setupWidgets();
}

void CustomWidgetsPage::setupWidgets()
{
    QList<PluginMetadata> plugins = m_pluginManager->loadedPlugins();
    
    // Filter to only valid plugins
    QList<PluginMetadata> validPlugins;
    for (const PluginMetadata &meta : plugins) {
        if (meta.isValid) {
            validPlugins.append(meta);
        }
    }
    
    // If no valid plugins, show empty state
    if (validPlugins.isEmpty()) {
        setupEmptyState();
        return;
    }
    
    // Group plugins by category
    QMap<QString, QList<PluginMetadata>> pluginsByCategory;
    for (const PluginMetadata &meta : validPlugins) {
        QString category = meta.category.isEmpty() ? tr("Custom") : meta.category;
        pluginsByCategory[category].append(meta);
    }
    
    // Create widget groups for each category
    for (auto it = pluginsByCategory.constBegin(); it != pluginsByCategory.constEnd(); ++it) {
        createWidgetGroup(it.key(), it.value());
    }
}

void CustomWidgetsPage::setupEmptyState()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("No Plugins Loaded")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QLabel *messageLabel = new QLabel(group);
    messageLabel->setText(tr(
        "No custom widget plugins are currently loaded.\n\n"
        "To add custom widgets:\n"
        "1. Create a Qt plugin implementing WidgetPluginInterface\n"
        "2. Place the plugin file (.so, .dll, or .dylib) in the plugins directory\n"
        "3. Use View → Refresh Plugins or restart the application\n\n"
        "The plugins directory can be configured in the application settings."
    ));
    messageLabel->setWordWrap(true);
    groupLayout->addWidget(messageLabel);
}

void CustomWidgetsPage::createWidgetGroup(const QString &category, 
                                          const QList<PluginMetadata> &plugins)
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(category));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    for (const PluginMetadata &metadata : plugins) {
        QWidget *widgetDisplay = createWidgetDisplay(metadata);
        groupLayout->addWidget(widgetDisplay);
    }
}

QWidget* CustomWidgetsPage::createWidgetDisplay(const PluginMetadata &metadata)
{
    QWidget *container = new QWidget(contentWidget());
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 8);
    layout->setSpacing(4);
    
    // Add name label
    QLabel *nameLabel = new QLabel(metadata.name, container);
    nameLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(nameLabel);
    
    // Add description
    QLabel *descLabel = new QLabel(metadata.description, container);
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);
    
    // Try to create widget instance
    WidgetPluginInterface *interface = m_pluginManager->pluginInterface(metadata.name);
    
    if (interface) {
        QWidget *widget = interface->createWidget(container);
        if (widget) {
            layout->addWidget(widget);
            m_customWidgets.append(widget);
        } else {
            // Handle createWidget() returning nullptr with error placeholder
            QLabel *errorLabel = new QLabel(
                tr("Error: Failed to create widget instance"), container);
            errorLabel->setStyleSheet("color: red;");
            layout->addWidget(errorLabel);
        }
    } else {
        // Plugin interface not found (shouldn't happen for valid metadata)
        QLabel *errorLabel = new QLabel(
            tr("Error: Plugin interface not found"), container);
        errorLabel->setStyleSheet("color: red;");
        layout->addWidget(errorLabel);
    }
    
    return container;
}
