#ifndef VARIABLEPANEL_H
#define VARIABLEPANEL_H

#include <QWidget>
#include <QString>
#include <QColor>

class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class QLineEdit;
class VariableManager;

/**
 * @brief Widget providing UI for variable management with color picker integration.
 * 
 * The VariablePanel provides:
 * - Table-based variable list with name and value columns
 * - Add/delete buttons for variable management
 * - Color swatch display for color values
 * - Color picker integration via QColorDialog
 * - Variable insertion support for QSS editor
 */
class VariablePanel : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a VariablePanel widget.
     * @param parent The parent widget.
     */
    explicit VariablePanel(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~VariablePanel();

    /**
     * @brief Sets the VariableManager to use for data operations.
     * @param manager The VariableManager instance.
     */
    void setVariableManager(VariableManager *manager);

    /**
     * @brief Returns the current VariableManager.
     * @return Pointer to the VariableManager, or nullptr if not set.
     */
    VariableManager* variableManager() const;

    /**
     * @brief Formats a variable name as a reference string.
     * @param name The variable name.
     * @return The formatted reference string (e.g., "${name}").
     */
    static QString formatVariableReference(const QString &name);

signals:
    /**
     * @brief Emitted when the user requests to insert a variable reference.
     * @param reference The formatted variable reference (e.g., "${name}").
     */
    void variableInsertRequested(const QString &reference);

private slots:
    void onAddVariable();
    void onDeleteVariable();
    void onInsertVariable();
    void onVariableValueChanged(QTableWidgetItem *item);
    void onColorButtonClicked();
    void onTableSelectionChanged();
    
    // Slots for VariableManager signals
    void onVariableChanged(const QString &name, const QString &value);
    void onVariableRemoved(const QString &name);
    void onVariablesCleared();

private:
    void setupUi();
    void setupConnections();
    void refreshVariableList();
    void updateColorSwatch(int row, const QString &value);
    void updateButtonStates();
    int findRowByName(const QString &name) const;
    
    /**
     * @brief Parses a color value string into a QColor.
     * @param value The color value string (hex format).
     * @return The parsed QColor, or invalid QColor if not a valid color.
     */
    QColor parseColor(const QString &value) const;

    VariableManager *m_variableManager;
    QTableWidget *m_variableTable;
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QPushButton *m_insertButton;
    QLineEdit *m_nameEdit;
    QLineEdit *m_valueEdit;
    QPushButton *m_colorPickerButton;
    
    // Column indices
    static constexpr int COL_NAME = 0;
    static constexpr int COL_VALUE = 1;
    static constexpr int COL_COLOR = 2;
    
    // Flag to prevent recursive updates
    bool m_updatingTable;
};

#endif // VARIABLEPANEL_H
