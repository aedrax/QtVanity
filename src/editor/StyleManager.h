#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QPointer>

class QWidget;

/**
 * @brief Manages stylesheet loading, saving, and application.
 *
 * The StyleManager is responsible for:
 * - Loading QSS files from disk and applying them to the preview target
 * - Saving stylesheet content to .qss files
 * - Providing predefined style templates
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
     * @brief Applies a stylesheet to the current preview target.
     * @param qss The QSS content to apply.
     *
     * Goes to the widget set by setPreviewTarget(), or to the whole
     * application if no target is set or applyToApplication() is true.
     */
    void applyStyleSheet(const QString &qss);

    /**
     * @brief Sets the widget the preview is applied to.
     * @param target The widget to style, or nullptr for the application.
     *
     * Confining the preview to one widget means a stylesheet under test cannot
     * make the rest of the program unreadable - which, with a rule as ordinary
     * as `QWidget { color: black; }` on a dark theme, it otherwise can.
     */
    void setPreviewTarget(QWidget *target);

    /**
     * @brief Returns the widget the preview is applied to, or nullptr.
     */
    QWidget* previewTarget() const;

    /**
     * @brief Sets whether the preview covers the whole application.
     * @param enabled true to style everything, false to style only the target.
     *
     * Application-wide is the only way to preview rules for QMainWindow,
     * QDockWidget, QMenuBar and QStatusBar, so it stays available as a choice.
     */
    void setApplyToApplication(bool enabled);

    /**
     * @brief Returns whether the preview covers the whole application.
     */
    bool applyToApplication() const;

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
     * @brief Clears the application stylesheet (applies default Qt styling).
     */
    void clearStyleSheet();

    /**
     * @brief Returns whether a custom stylesheet is currently applied.
     * @return true if a non-empty stylesheet is applied.
     */
    bool hasCustomStyleSheet() const;

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

    /**
     * @brief Returns a list of available QStyle names.
     * @return List of style names from QStyleFactory::keys().
     */
    QStringList availableStyles() const;

    /**
     * @brief Returns the name of the currently active QStyle.
     * @return The current style name.
     */
    QString currentStyle() const;

    /**
     * @brief Returns the name of the platform default QStyle.
     * @return The default style name.
     */
    QString defaultStyle() const;

    /**
     * @brief Sets the application's QStyle.
     * @param styleName The name of the style to apply.
     * 
     * If the style name is invalid, emits styleChangeError.
     * After applying the new style, reapplies the current QSS.
     */
    void setStyle(const QString &styleName);

signals:
    /**
     * @brief Emitted when a stylesheet is successfully applied.
     */
    void styleApplied();

    /**
     * @brief Emitted when the stylesheet is cleared.
     */
    void styleCleared();

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

    /**
     * @brief Emitted when the QStyle is successfully changed.
     * @param styleName The name of the new style.
     */
    void styleChanged(const QString &styleName);

    /**
     * @brief Emitted when a QStyle change fails.
     * @param error The error message.
     */
    void styleChangeError(const QString &error);

    /**
     * @brief Emitted after every apply with any parse errors Qt reported.
     * @param errors The messages, empty when the stylesheet parsed cleanly.
     *
     * Qt reports a malformed stylesheet by logging a warning and applying
     * nothing, so this is the only signal that the user's QSS did not take.
     */
    void diagnosticsChanged(const QStringList &errors);

private:
    /**
     * @brief Sends @p qss to whichever destination is currently selected.
     */
    void routeStyleSheet(const QString &qss);

    QString m_templatesPath;
    QString m_currentStyleSheet;

    /// QPointer: the target is a widget this object does not own.
    QPointer<QWidget> m_previewTarget;
    bool m_applyToApplication;
    QString m_currentStyle;
    QString m_defaultStyle;
};

#endif // STYLEMANAGER_H
