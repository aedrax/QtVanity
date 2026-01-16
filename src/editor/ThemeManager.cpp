#include "ThemeManager.h"
#include "StyleManager.h"

#include <QSettings>
#include <QTimer>

#ifdef Q_OS_WIN
#include <QSettings>
#endif

#ifdef Q_OS_MACOS
#include <QProcess>
#endif

#ifdef Q_OS_LINUX
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#endif

namespace {
    const QString SETTINGS_KEY = QStringLiteral("appearance/themeMode");
    const int SYSTEM_THEME_POLL_INTERVAL_MS = 5000; // 5 seconds
}

ThemeManager::ThemeManager(StyleManager *styleManager, QObject *parent)
    : QObject(parent)
    , m_styleManager(styleManager)
    , m_currentMode(ThemeMode::System)
    , m_lastEffectiveTheme(ThemeMode::Light)
    , m_systemThemeTimer(nullptr)
{
    loadPreference();
    m_lastEffectiveTheme = effectiveTheme();
    setupSystemThemeWatcher();
    applyCurrentTheme();
}

ThemeManager::~ThemeManager()
{
    if (m_systemThemeTimer) {
        m_systemThemeTimer->stop();
    }
}

ThemeManager::ThemeMode ThemeManager::currentMode() const
{
    return m_currentMode;
}

void ThemeManager::setThemeMode(ThemeMode mode)
{
    if (m_currentMode == mode) {
        return;
    }

    ThemeMode oldEffective = effectiveTheme();
    m_currentMode = mode;
    savePreference();

    emit themeModeChanged(mode);

    // Update system theme watcher based on mode
    if (m_systemThemeTimer) {
        if (mode == ThemeMode::System) {
            m_systemThemeTimer->start();
        } else {
            m_systemThemeTimer->stop();
        }
    }

    applyCurrentTheme();

    ThemeMode newEffective = effectiveTheme();
    if (oldEffective != newEffective) {
        m_lastEffectiveTheme = newEffective;
        emit effectiveThemeChanged(newEffective);
    }
}

bool ThemeManager::isSystemDarkMode() const
{
    return detectSystemDarkMode();
}

ThemeManager::ThemeMode ThemeManager::effectiveTheme() const
{
    if (m_currentMode == ThemeMode::System) {
        return detectSystemDarkMode() ? ThemeMode::Dark : ThemeMode::Light;
    }
    return m_currentMode;
}

void ThemeManager::onSystemThemeChanged()
{
    if (m_currentMode != ThemeMode::System) {
        return;
    }

    ThemeMode newEffective = effectiveTheme();
    if (newEffective != m_lastEffectiveTheme) {
        m_lastEffectiveTheme = newEffective;
        applyCurrentTheme();
        emit effectiveThemeChanged(newEffective);
    }
}

void ThemeManager::applyCurrentTheme()
{
    if (!m_styleManager) {
        return;
    }

    ThemeMode effective = effectiveTheme();
    QString templateName = (effective == ThemeMode::Dark) ? QStringLiteral("dark") 
                                                          : QStringLiteral("light");

    QString qss = m_styleManager->loadTemplate(templateName);
    if (!qss.isEmpty()) {
        m_styleManager->applyStyleSheet(qss);
    }
}

void ThemeManager::loadPreference()
{
    QSettings settings;
    int modeValue = settings.value(SETTINGS_KEY, static_cast<int>(ThemeMode::System)).toInt();

    // Validate the loaded value is within valid range (0-2)
    if (modeValue < 0 || modeValue > 2) {
        m_currentMode = ThemeMode::System;
    } else {
        m_currentMode = static_cast<ThemeMode>(modeValue);
    }
}

void ThemeManager::savePreference()
{
    QSettings settings;
    settings.setValue(SETTINGS_KEY, static_cast<int>(m_currentMode));
}


bool ThemeManager::detectSystemDarkMode() const
{
#ifdef Q_OS_WIN
    // Windows: Read registry AppsUseLightTheme
    // Value of 0 means dark mode, 1 means light mode
    QSettings settings(
        QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
        QSettings::NativeFormat);
    return settings.value(QStringLiteral("AppsUseLightTheme"), 1).toInt() == 0;
#elif defined(Q_OS_MACOS)
    // macOS: Use defaults read for AppleInterfaceStyle
    QProcess process;
    process.start(QStringLiteral("defaults"), 
                  QStringList() << QStringLiteral("read") 
                               << QStringLiteral("-g") 
                               << QStringLiteral("AppleInterfaceStyle"));
    process.waitForFinished(1000);
    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    return output.compare(QStringLiteral("Dark"), Qt::CaseInsensitive) == 0;
#elif defined(Q_OS_LINUX)
    // Linux: Check GTK theme name for "dark" keyword
    // Try multiple locations for GTK settings
    
    // First try GTK 3.0 settings.ini
    QString configPath = QDir::homePath() + QStringLiteral("/.config/gtk-3.0/settings.ini");
    QFile file(configPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith(QStringLiteral("gtk-theme-name"), Qt::CaseInsensitive)) {
                file.close();
                return line.contains(QStringLiteral("dark"), Qt::CaseInsensitive);
            }
        }
        file.close();
    }
    
    // Try environment variable
    QString gtkTheme = qEnvironmentVariable("GTK_THEME");
    if (!gtkTheme.isEmpty()) {
        return gtkTheme.contains(QStringLiteral("dark"), Qt::CaseInsensitive);
    }
    
    // Fallback: check XDG color-scheme via gsettings (if available)
    QProcess process;
    process.start(QStringLiteral("gsettings"), 
                  QStringList() << QStringLiteral("get") 
                               << QStringLiteral("org.gnome.desktop.interface") 
                               << QStringLiteral("color-scheme"));
    process.waitForFinished(1000);
    if (process.exitCode() == 0) {
        QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
        return output.contains(QStringLiteral("dark"), Qt::CaseInsensitive);
    }
    
    // Default to light mode if detection fails
    return false;
#else
    // Unknown platform: default to light mode
    return false;
#endif
}

void ThemeManager::setupSystemThemeWatcher()
{
    m_systemThemeTimer = new QTimer(this);
    m_systemThemeTimer->setInterval(SYSTEM_THEME_POLL_INTERVAL_MS);
    connect(m_systemThemeTimer, &QTimer::timeout, this, &ThemeManager::onSystemThemeChanged);

    // Only start the timer if we're in System mode
    if (m_currentMode == ThemeMode::System) {
        m_systemThemeTimer->start();
    }
}
