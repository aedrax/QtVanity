#ifndef VARIABLEMANAGER_H
#define VARIABLEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @brief Manages QSS variables for substitution in stylesheets.
 * 
 * The VariableManager is responsible for:
 * - Storing and managing named variables with values
 * - Substituting variable references (${name}) in QSS templates
 * - Saving and loading project files (.qvp format)
 * - Exporting resolved QSS to .qss files
 */
class VariableManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a VariableManager.
     * @param parent The parent QObject.
     */
    explicit VariableManager(QObject *parent = nullptr);

    // =========================================================================
    // Variable Operations
    // =========================================================================

    /**
     * @brief Sets a variable value, creating it if it doesn't exist.
     * @param name The variable name.
     * @param value The variable value.
     */
    void setVariable(const QString &name, const QString &value);

    /**
     * @brief Removes a variable.
     * @param name The variable name to remove.
     */
    void removeVariable(const QString &name);

    /**
     * @brief Gets a variable's value.
     * @param name The variable name.
     * @return The variable value, or empty string if not found.
     */
    QString variable(const QString &name) const;

    /**
     * @brief Gets a list of all variable names.
     * @return List of variable names.
     */
    QStringList variableNames() const;

    /**
     * @brief Gets all variables as a map.
     * @return Map of variable names to values.
     */
    QMap<QString, QString> allVariables() const;

    /**
     * @brief Checks if a variable exists.
     * @param name The variable name.
     * @return true if the variable exists.
     */
    bool hasVariable(const QString &name) const;

    /**
     * @brief Removes all variables.
     */
    void clearVariables();

    // =========================================================================
    // Substitution
    // =========================================================================

    /**
     * @brief Substitutes variable references in a QSS template.
     * @param qssTemplate The template containing ${name} references.
     * @return The resolved QSS with variables substituted.
     */
    QString substitute(const QString &qssTemplate) const;

    /**
     * @brief Finds all variable references in a template.
     * @param qssTemplate The template to search.
     * @return List of variable names referenced (without ${} syntax).
     */
    QStringList findVariableReferences(const QString &qssTemplate) const;

    /**
     * @brief Finds variable references that have no matching variable.
     * @param qssTemplate The template to search.
     * @return List of undefined variable names.
     */
    QStringList findUndefinedReferences(const QString &qssTemplate) const;

    // =========================================================================
    // Project File I/O
    // =========================================================================

    /**
     * @brief Saves variables and template to a project file.
     * @param filePath The path to save to (.qvp file).
     * @param qssTemplate The QSS template content.
     * @return true if successful.
     */
    bool saveProject(const QString &filePath, const QString &qssTemplate);

    /**
     * @brief Loads variables and template from a project file.
     * @param filePath The path to load from (.qvp file).
     * @param qssTemplate Output parameter for the loaded template.
     * @return true if successful.
     */
    bool loadProject(const QString &filePath, QString &qssTemplate);

    /**
     * @brief Exports resolved QSS to a file.
     * @param filePath The path to save to (.qss file).
     * @param qssTemplate The QSS template to resolve and save.
     * @return true if successful.
     */
    bool exportResolvedQss(const QString &filePath, const QString &qssTemplate);

    // =========================================================================
    // Validation
    // =========================================================================

    /**
     * @brief Validates a variable name.
     * 
     * Valid names must:
     * - Be non-empty
     * - Start with a letter or underscore
     * - Contain only letters, numbers, underscores, or hyphens
     * 
     * @param name The name to validate.
     * @return true if the name is valid.
     */
    static bool isValidVariableName(const QString &name);

    /**
     * @brief Checks if a value appears to be a color.
     * 
     * Recognizes hex color formats:
     * - #RGB (3 hex digits)
     * - #RRGGBB (6 hex digits)
     * - #AARRGGBB (8 hex digits)
     * 
     * @param value The value to check.
     * @return true if the value is a valid hex color.
     */
    static bool isColorValue(const QString &value);

signals:
    /**
     * @brief Emitted when a variable is created or updated.
     * @param name The variable name.
     * @param value The new value.
     */
    void variableChanged(const QString &name, const QString &value);

    /**
     * @brief Emitted when a variable is removed.
     * @param name The removed variable name.
     */
    void variableRemoved(const QString &name);

    /**
     * @brief Emitted when all variables are cleared.
     */
    void variablesCleared();

    /**
     * @brief Emitted when a project is loaded.
     */
    void projectLoaded();

    /**
     * @brief Emitted when a project is saved.
     */
    void projectSaved();

    /**
     * @brief Emitted when a load error occurs.
     * @param error The error message.
     */
    void loadError(const QString &error);

    /**
     * @brief Emitted when a save error occurs.
     * @param error The error message.
     */
    void saveError(const QString &error);

private:
    QMap<QString, QString> m_variables;
};

#endif // VARIABLEMANAGER_H
