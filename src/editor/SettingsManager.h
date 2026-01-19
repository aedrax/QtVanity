#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QStringList>

/**
 * @brief Manages persistent application settings using QSettings.
 * 
 * The SettingsManager is responsible for:
 * - Saving and loading window geometry
 * - Saving and loading splitter state (panel widths)
 * - Saving and loading base style preference
 * - Managing the recent projects list
 * - Persisting settings using platform-appropriate storage
 */
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a SettingsManager.
     * @param parent The parent QObject.
     * 
     * Initializes QSettings with organization "QtVanity" and application "QtVanity".
     */
    explicit SettingsManager(QObject *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~SettingsManager() override;

    // Window geometry
    /**
     * @brief Saves the window geometry.
     * @param geometry The geometry data from QWidget::saveGeometry().
     */
    void saveWindowGeometry(const QByteArray &geometry);

    /**
     * @brief Loads the saved window geometry.
     * @return The saved geometry data, or empty QByteArray if none exists.
     */
    QByteArray loadWindowGeometry() const;

    /**
     * @brief Checks if window geometry has been saved.
     * @return true if saved geometry exists, false otherwise.
     */
    bool hasWindowGeometry() const;

    // Splitter state
    /**
     * @brief Saves the splitter state.
     * @param state The state data from QSplitter::saveState().
     */
    void saveSplitterState(const QByteArray &state);

    /**
     * @brief Loads the saved splitter state.
     * @return The saved state data, or empty QByteArray if none exists.
     */
    QByteArray loadSplitterState() const;

    /**
     * @brief Checks if splitter state has been saved.
     * @return true if saved state exists, false otherwise.
     */
    bool hasSplitterState() const;

    // Dock state
    /**
     * @brief Saves the dock widget state.
     * @param state The state data from QMainWindow::saveState().
     */
    void saveDockState(const QByteArray &state);

    /**
     * @brief Loads the saved dock widget state.
     * @return The saved state data, or empty QByteArray if none exists.
     */
    QByteArray loadDockState() const;

    /**
     * @brief Checks if dock state has been saved.
     * @return true if saved state exists, false otherwise.
     */
    bool hasDockState() const;

    // Base style
    /**
     * @brief Saves the base style name.
     * @param styleName The name of the QStyle to save.
     */
    void saveBaseStyle(const QString &styleName);

    /**
     * @brief Loads the saved base style name.
     * @return The saved style name, or empty string if none exists.
     */
    QString loadBaseStyle() const;

    /**
     * @brief Checks if a base style has been saved.
     * @return true if saved style exists, false otherwise.
     */
    bool hasBaseStyle() const;

    // Recent projects
    /**
     * @brief Adds a project to the recent projects list.
     * @param filePath The full path to the project file.
     * 
     * If the project already exists in the list, it is moved to the front.
     * The list is capped at MaxRecentProjects entries.
     * Emits recentProjectsChanged() after modification.
     */
    void addRecentProject(const QString &filePath);

    /**
     * @brief Removes a project from the recent projects list.
     * @param filePath The full path to the project file to remove.
     * 
     * Emits recentProjectsChanged() if the project was found and removed.
     */
    void removeRecentProject(const QString &filePath);

    /**
     * @brief Returns the list of recent projects.
     * @return List of project file paths, most recent first.
     */
    QStringList recentProjects() const;

    /**
     * @brief Clears all recent projects.
     * 
     * Emits recentProjectsChanged() after clearing.
     */
    void clearRecentProjects();

    /**
     * @brief Maximum number of recent projects to store.
     */
    static constexpr int MaxRecentProjects = 10;

signals:
    /**
     * @brief Emitted when the recent projects list changes.
     */
    void recentProjectsChanged();

private:
    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H
