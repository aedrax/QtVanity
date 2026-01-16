#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>

class StyleManager;
class QTimer;

/**
 * @brief Manages UI theme preferences (Dark, Light, System).
 * 
 * The ThemeManager is responsible for:
 * - Storing the user's theme mode preference (Dark/Light/System)
 * - Detecting the system's current appearance setting
 * - Persisting preferences via QSettings
 * - Automatically updating the theme when system appearance changes
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Available theme modes.
     */
    enum class ThemeMode {
        Dark = 0,   ///< Always use dark theme
        Light = 1,  ///< Always use light theme
        System = 2  ///< Follow system appearance setting
    };
    Q_ENUM(ThemeMode)

    /**
     * @brief Constructs a ThemeManager.
     * @param styleManager The StyleManager to use for applying themes.
     * @param parent The parent QObject.
     */
    explicit ThemeManager(StyleManager *styleManager, QObject *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~ThemeManager() override;

    /**
     * @brief Returns the current theme mode.
     * @return The currently selected ThemeMode.
     */
    ThemeMode currentMode() const;

    /**
     * @brief Sets the theme mode.
     * @param mode The ThemeMode to set.
     * 
     * This will save the preference and apply the appropriate theme.
     */
    void setThemeMode(ThemeMode mode);

    /**
     * @brief Checks if the system is currently in dark mode.
     * @return true if system dark mode is detected, false otherwise.
     */
    bool isSystemDarkMode() const;

    /**
     * @brief Returns the effective theme (resolves System to actual Dark/Light).
     * @return ThemeMode::Dark or ThemeMode::Light based on current settings.
     */
    ThemeMode effectiveTheme() const;

signals:
    /**
     * @brief Emitted when the theme mode changes.
     * @param mode The new ThemeMode.
     */
    void themeModeChanged(ThemeMode mode);

    /**
     * @brief Emitted when the effective theme changes.
     * @param effectiveTheme The new effective theme (Dark or Light).
     * 
     * This is emitted when:
     * - The user changes the theme mode
     * - System mode is active and the system theme changes
     */
    void effectiveThemeChanged(ThemeMode effectiveTheme);

private slots:
    /**
     * @brief Called when the system theme may have changed.
     */
    void onSystemThemeChanged();

private:
    /**
     * @brief Applies the current theme based on mode and system settings.
     */
    void applyCurrentTheme();

    /**
     * @brief Loads the theme preference from QSettings.
     */
    void loadPreference();

    /**
     * @brief Saves the theme preference to QSettings.
     */
    void savePreference();

    /**
     * @brief Detects if the system is in dark mode.
     * @return true if system dark mode is detected.
     */
    bool detectSystemDarkMode() const;

    /**
     * @brief Sets up the system theme change watcher.
     */
    void setupSystemThemeWatcher();

    StyleManager *m_styleManager;
    ThemeMode m_currentMode;
    ThemeMode m_lastEffectiveTheme;
    QTimer *m_systemThemeTimer;
};

#endif // THEMEMANAGER_H
