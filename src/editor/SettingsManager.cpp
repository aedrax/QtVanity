#include "SettingsManager.h"
#include <QDir>

namespace {
    // Settings keys
    const QString KeyWindowGeometry = QStringLiteral("window/geometry");
    const QString KeySplitterState = QStringLiteral("window/splitterState");
    const QString KeyDockState = QStringLiteral("window/dockState");
    const QString KeyBaseStyle = QStringLiteral("appearance/baseStyle");
    const QString KeyRecentProjects = QStringLiteral("recentProjects");
    const QString KeyPluginDirectory = QStringLiteral("plugins/directory");
}

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(QStringLiteral("QtVanity"), QStringLiteral("QtVanity"))
{
}

SettingsManager::~SettingsManager() = default;

// Window geometry
void SettingsManager::saveWindowGeometry(const QByteArray &geometry)
{
    m_settings.setValue(KeyWindowGeometry, geometry);
}

QByteArray SettingsManager::loadWindowGeometry() const
{
    return m_settings.value(KeyWindowGeometry).toByteArray();
}

bool SettingsManager::hasWindowGeometry() const
{
    return m_settings.contains(KeyWindowGeometry);
}

// Splitter state
void SettingsManager::saveSplitterState(const QByteArray &state)
{
    m_settings.setValue(KeySplitterState, state);
}

QByteArray SettingsManager::loadSplitterState() const
{
    return m_settings.value(KeySplitterState).toByteArray();
}

bool SettingsManager::hasSplitterState() const
{
    return m_settings.contains(KeySplitterState);
}

// Dock state
void SettingsManager::saveDockState(const QByteArray &state)
{
    m_settings.setValue(KeyDockState, state);
}

QByteArray SettingsManager::loadDockState() const
{
    return m_settings.value(KeyDockState).toByteArray();
}

bool SettingsManager::hasDockState() const
{
    return m_settings.contains(KeyDockState);
}

// Base style
void SettingsManager::saveBaseStyle(const QString &styleName)
{
    m_settings.setValue(KeyBaseStyle, styleName);
}

QString SettingsManager::loadBaseStyle() const
{
    return m_settings.value(KeyBaseStyle).toString();
}

bool SettingsManager::hasBaseStyle() const
{
    return m_settings.contains(KeyBaseStyle);
}

// Plugin directory
QString SettingsManager::pluginDirectory() const
{
    QString path = m_settings.value(KeyPluginDirectory).toString();
    if (path.isEmpty()) {
        return defaultPluginDirectory();
    }
    return path;
}

void SettingsManager::setPluginDirectory(const QString &path)
{
    QString currentPath = pluginDirectory();
    if (currentPath != path) {
        m_settings.setValue(KeyPluginDirectory, path);
        emit pluginDirectoryChanged();
    }
}

QString SettingsManager::defaultPluginDirectory() const
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(appDataPath).filePath(QStringLiteral("plugins"));
}

// Recent projects
void SettingsManager::addRecentProject(const QString &filePath)
{
    QStringList projects = recentProjects();
    
    // Remove if already exists (will be re-added at front)
    projects.removeAll(filePath);
    
    // Add to front
    projects.prepend(filePath);
    
    // Enforce cap
    while (projects.size() > MaxRecentProjects) {
        projects.removeLast();
    }
    
    m_settings.setValue(KeyRecentProjects, projects);
    emit recentProjectsChanged();
}

void SettingsManager::removeRecentProject(const QString &filePath)
{
    QStringList projects = recentProjects();
    
    if (projects.removeAll(filePath) > 0) {
        m_settings.setValue(KeyRecentProjects, projects);
        emit recentProjectsChanged();
    }
}

QStringList SettingsManager::recentProjects() const
{
    return m_settings.value(KeyRecentProjects).toStringList();
}

void SettingsManager::clearRecentProjects()
{
    m_settings.remove(KeyRecentProjects);
    emit recentProjectsChanged();
}
