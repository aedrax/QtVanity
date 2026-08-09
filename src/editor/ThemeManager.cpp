#include "ThemeManager.h"
#include "SettingsManager.h"

#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QStyleHints>
#include <QTimer>

// Qt gained a queryable colour scheme in 6.5 and a settable one in 6.8. Below
// those versions we fall back to per-platform probing and an explicit palette.
#define QTVANITY_HAS_COLOR_SCHEME_QUERY (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
#define QTVANITY_HAS_COLOR_SCHEME_SET   (QT_VERSION >= QT_VERSION_CHECK(6, 8, 0))

#if !QTVANITY_HAS_COLOR_SCHEME_QUERY
#include <QSettings>
#ifdef Q_OS_MACOS
#include <QProcess>
#endif
#ifdef Q_OS_LINUX
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#endif
#endif

namespace {

#if !QTVANITY_HAS_COLOR_SCHEME_QUERY
const int SYSTEM_THEME_POLL_INTERVAL_MS = 5000; // 5 seconds
#endif

#if !QTVANITY_HAS_COLOR_SCHEME_SET
/**
 * @brief Builds a dark palette for Qt versions that cannot be told the scheme.
 *
 * Modelled on Qt's own Fusion dark palette so that the chrome remains legible
 * with the platform style the user has selected.
 */
QPalette buildDarkPalette()
{
    QPalette palette;
    const QColor window(53, 53, 53);
    const QColor base(35, 35, 35);
    const QColor text(220, 220, 220);
    const QColor disabled(127, 127, 127);
    const QColor highlight(38, 110, 183);

    palette.setColor(QPalette::Window, window);
    palette.setColor(QPalette::WindowText, text);
    palette.setColor(QPalette::Base, base);
    palette.setColor(QPalette::AlternateBase, window);
    palette.setColor(QPalette::ToolTipBase, window);
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::Button, window);
    palette.setColor(QPalette::ButtonText, text);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, highlight);
    palette.setColor(QPalette::Highlight, highlight);
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::PlaceholderText, disabled);

    palette.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabled);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled);

    return palette;
}
#endif // !QTVANITY_HAS_COLOR_SCHEME_SET

} // namespace

ThemeManager::ThemeManager(SettingsManager *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
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
    if (!qApp) {
        return;
    }

#if QTVANITY_HAS_COLOR_SCHEME_SET
    // Ask Qt for the appearance and let the active style honour it. This never
    // touches the application stylesheet, so the user's QSS is left alone.
    switch (m_currentMode) {
    case ThemeMode::Dark:
        qApp->styleHints()->setColorScheme(Qt::ColorScheme::Dark);
        break;
    case ThemeMode::Light:
        qApp->styleHints()->setColorScheme(Qt::ColorScheme::Light);
        break;
    case ThemeMode::System:
        qApp->styleHints()->unsetColorScheme();
        break;
    }
#else
    // No settable colour scheme: express the appearance as a palette instead.
    if (effectiveTheme() == ThemeMode::Dark) {
        qApp->setPalette(buildDarkPalette());
    } else if (QStyle *style = qApp->style()) {
        qApp->setPalette(style->standardPalette());
    }
#endif
}

void ThemeManager::loadPreference()
{
    if (!m_settings) {
        return;
    }

    int modeValue = m_settings->loadThemeMode();

    // Validate the loaded value is within valid range (0-2)
    if (modeValue < 0 || modeValue > 2) {
        m_currentMode = ThemeMode::System;
    } else {
        m_currentMode = static_cast<ThemeMode>(modeValue);
    }
}

void ThemeManager::savePreference()
{
    if (m_settings) {
        m_settings->saveThemeMode(static_cast<int>(m_currentMode));
    }
}

bool ThemeManager::detectSystemDarkMode() const
{
#if QTVANITY_HAS_COLOR_SCHEME_QUERY
    // In System mode the colour scheme is unset, so this reports what the OS
    // is actually doing. In an explicit mode effectiveTheme() never consults
    // this function, so reading back our own request is not a concern.
    return qApp && qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#elif defined(Q_OS_WIN)
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

    QString gtkTheme = qEnvironmentVariable("GTK_THEME");
    if (!gtkTheme.isEmpty()) {
        return gtkTheme.contains(QStringLiteral("dark"), Qt::CaseInsensitive);
    }

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

    return false;
#else
    // Unknown platform: default to light mode
    return false;
#endif
}

void ThemeManager::setupSystemThemeWatcher()
{
#if QTVANITY_HAS_COLOR_SCHEME_QUERY
    // Qt notifies us; no polling, and no process spawned every few seconds.
    if (qApp) {
        connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
                this, &ThemeManager::onSystemThemeChanged);
    }
#else
    m_systemThemeTimer = new QTimer(this);
    m_systemThemeTimer->setInterval(SYSTEM_THEME_POLL_INTERVAL_MS);
    connect(m_systemThemeTimer, &QTimer::timeout, this, &ThemeManager::onSystemThemeChanged);

    // Only start the timer if we're in System mode
    if (m_currentMode == ThemeMode::System) {
        m_systemThemeTimer->start();
    }
#endif
}
