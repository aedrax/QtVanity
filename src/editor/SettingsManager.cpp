#include "SettingsManager.h"
#include <QDir>

namespace {
    // Settings keys
    const QString KeyWindowGeometry = QStringLiteral("window/geometry");
    const QString KeyDockState = QStringLiteral("window/dockState");
    const QString KeyBaseStyle = QStringLiteral("appearance/baseStyle");
    const QString KeyThemeMode = QStringLiteral("appearance/themeMode");
    const QString KeyPreviewWholeApp = QStringLiteral("preview/appliesToApplication");
    const QString KeyAutoApply = QStringLiteral("preview/autoApply");
    const QString KeyRecentProjects = QStringLiteral("recentProjects");
    const QString KeyPluginDirectory = QStringLiteral("plugins/directory");
}

// The default QSettings constructor picks up the organisation and application
// names set in main.cpp. Passing explicit names here used to send these
// settings to a different file than ThemeManager's, so the theme preference
// was persisted somewhere nothing else could see it.
SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
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

// Theme mode
void SettingsManager::saveThemeMode(int mode)
{
    m_settings.setValue(KeyThemeMode, mode);
}

int SettingsManager::loadThemeMode() const
{
    // -1 means "not set"; ThemeManager treats out-of-range values as System.
    return m_settings.value(KeyThemeMode, -1).toInt();
}

bool SettingsManager::hasThemeMode() const
{
    return m_settings.contains(KeyThemeMode);
}

// Preview scope
void SettingsManager::savePreviewAppliesToApplication(bool enabled)
{
    m_settings.setValue(KeyPreviewWholeApp, enabled);
}

bool SettingsManager::previewAppliesToApplication() const
{
    // Default false: an unreadable stylesheet under test should not be able to
    // take the rest of the program with it.
    return m_settings.value(KeyPreviewWholeApp, false).toBool();
}

// Live preview
void SettingsManager::saveAutoApply(bool enabled)
{
    m_settings.setValue(KeyAutoApply, enabled);
}

bool SettingsManager::autoApply() const
{
    // Default true: watching the gallery change as you type is the point.
    return m_settings.value(KeyAutoApply, true).toBool();
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
