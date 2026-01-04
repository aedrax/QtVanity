#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief Manages stylesheet loading, saving, and application.
 * 
 * The StyleManager is responsible for:
 * - Loading QSS files from disk and applying them to the application
 * - Saving stylesheet content to .qss files
 * - Providing predefined style templates (Dark, Light, Solarized)
 * - Tracking the current stylesheet state
 */
class StyleManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a StyleManager.
     * @param parent The parent QObject.
     */
    explicit StyleManager(QObject *parent = nullptr);

    /**
     * @brief Applies a stylesheet to the entire application.
     * @param qss The QSS content to apply.
     */
    void applyStyleSheet(const QString &qss);

    /**
     * @brief Loads QSS content from a file.
     * @param filePath The path to the .qss file.
     * @return The file content, or empty string on error.
     */
    QString loadFromFile(const QString &filePath);

    /**
     * @brief Saves QSS content to a file.
     * @param filePath The path to save to.
     * @param qss The QSS content to save.
     * @return true if successful, false otherwise.
     */
    bool saveToFile(const QString &filePath, const QString &qss);

    /**
     * @brief Returns a list of available template names.
     * @return List of template names (without .qss extension).
     */
    QStringList availableTemplates() const;

    /**
     * @brief Loads a predefined template by name.
     * @param templateName The template name (e.g., "dark", "light").
     * @return The template content, or empty string if not found.
     */
    QString loadTemplate(const QString &templateName);

    /**
     * @brief Returns the current stylesheet content.
     * @return The currently applied stylesheet.
     */
    QString currentStyleSheet() const;

    /**
     * @brief Sets the templates directory path.
     * @param path The path to the templates directory.
     */
    void setTemplatesPath(const QString &path);

    /**
     * @brief Returns the templates directory path.
     * @return The current templates path.
     */
    QString templatesPath() const;

signals:
    /**
     * @brief Emitted when a stylesheet is successfully applied.
     */
    void styleApplied();

    /**
     * @brief Emitted when a file load error occurs.
     * @param error The error message.
     */
    void loadError(const QString &error);

    /**
     * @brief Emitted when a file save error occurs.
     * @param error The error message.
     */
    void saveError(const QString &error);

private:
    QString m_templatesPath;
    QString m_currentStyleSheet;
};

#endif // STYLEMANAGER_H
