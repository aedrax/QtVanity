#include "PluginManager.h"
#include "WidgetPluginInterface.h"

#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QDebug>

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
}

PluginManager::~PluginManager()
{
    unloadPlugins();
}

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

void PluginManager::setPluginDirectory(const QString &path)
{
    m_pluginDirectory = path;
}

QString PluginManager::pluginDirectory() const
{
    return m_pluginDirectory;
}

// -----------------------------------------------------------------------------
// Plugin Operations
// -----------------------------------------------------------------------------

void PluginManager::loadPlugins()
{
    // Clear any previous state
    m_errors.clear();
    m_metadata.clear();
    
    // Scan directory for plugin files
    scanDirectory();
    
    emit pluginsLoaded();
}

void PluginManager::refreshPlugins()
{
    unloadPlugins();
    loadPlugins();
}

void PluginManager::unloadPlugins()
{
    // Unload all plugins
    for (QPluginLoader *loader : m_loaders) {
        loader->unload();
        delete loader;
    }
    
    m_loaders.clear();
    m_plugins.clear();
    m_metadata.clear();
    m_errors.clear();
    
    emit pluginsUnloaded();
}

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

QList<PluginMetadata> PluginManager::loadedPlugins() const
{
    return m_metadata;
}

QList<QString> PluginManager::loadingErrors() const
{
    return m_errors;
}

WidgetPluginInterface* PluginManager::pluginInterface(const QString &name) const
{
    return m_plugins.value(name, nullptr);
}

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------

QStringList PluginManager::supportedExtensions()
{
    QStringList extensions;
    
#if defined(Q_OS_LINUX)
    extensions << "so";
#elif defined(Q_OS_WIN)
    extensions << "dll";
#elif defined(Q_OS_MACOS)
    extensions << "dylib";
#else
    // Fallback: support all common extensions
    extensions << "so" << "dll" << "dylib";
#endif
    
    return extensions;
}

// -----------------------------------------------------------------------------
// Private Methods
// -----------------------------------------------------------------------------

void PluginManager::scanDirectory()
{
    if (m_pluginDirectory.isEmpty()) {
        qWarning() << "PluginManager: Plugin directory not set";
        return;
    }
    
    QDir dir(m_pluginDirectory);
    
    // Create directory if it doesn't exist
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "PluginManager: Failed to create plugin directory:" << m_pluginDirectory;
            return;
        }
    }
    
    // Get all files in the directory
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    
    for (const QFileInfo &fileInfo : files) {
        QString filePath = fileInfo.absoluteFilePath();
        
        // Only process files with valid plugin extensions
        if (isPluginFile(filePath)) {
            loadPlugin(filePath);
        }
    }
}

bool PluginManager::loadPlugin(const QString &filePath)
{
    QPluginLoader *loader = new QPluginLoader(filePath, this);
    
    // Attempt to load the plugin
    if (!loader->load()) {
        QString errorMsg = QString("Failed to load %1: %2")
                          .arg(QFileInfo(filePath).fileName())
                          .arg(loader->errorString());
        m_errors.append(errorMsg);
        emit pluginLoadError(filePath, loader->errorString());
        delete loader;
        return false;
    }
    
    // Get the plugin instance
    QObject *plugin = loader->instance();
    if (!plugin) {
        QString errorMsg = QString("Failed to get instance from %1")
                          .arg(QFileInfo(filePath).fileName());
        m_errors.append(errorMsg);
        emit pluginLoadError(filePath, "Failed to get plugin instance");
        loader->unload();
        delete loader;
        return false;
    }
    
    // Verify plugin implements WidgetPluginInterface
    WidgetPluginInterface *interface = qobject_cast<WidgetPluginInterface*>(plugin);
    if (!interface) {
        QString errorMsg = QString("%1 does not implement WidgetPluginInterface")
                          .arg(QFileInfo(filePath).fileName());
        m_errors.append(errorMsg);
        emit pluginLoadError(filePath, "Plugin does not implement WidgetPluginInterface");
        loader->unload();
        delete loader;
        return false;
    }
    
    // Success - store loader and interface
    m_loaders.append(loader);
    m_plugins.insert(interface->widgetName(), interface);
    
    // Extract metadata
    PluginMetadata metadata;
    metadata.filePath = filePath;
    metadata.name = interface->widgetName();
    metadata.description = interface->widgetDescription();
    metadata.icon = interface->widgetIcon();
    // Assign default "Custom" category if plugin returns empty category
    metadata.category = interface->widgetCategory().isEmpty() 
                       ? QStringLiteral("Custom") 
                       : interface->widgetCategory();
    metadata.isValid = true;
    m_metadata.append(metadata);
    
    // Emit signal on success
    emit pluginLoaded(metadata);
    
    qDebug() << "PluginManager: Loaded plugin" << metadata.name << "from" << filePath;
    
    return true;
}

bool PluginManager::isPluginFile(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    
    // Check against platform-specific extensions
    QStringList extensions = supportedExtensions();
    
    return extensions.contains(suffix, Qt::CaseInsensitive);
}
