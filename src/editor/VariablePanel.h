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
 * - Inline row buttons for delete and insert operations
 * - Color swatch display for color values
 * - Color picker integration via QColorDialog
 * - Empty row at bottom for adding new variables
 */
class VariablePanel : public QWidget
{
    Q_OBJECT

public:
    explicit VariablePanel(QWidget *parent = nullptr);
    ~VariablePanel();

    void setVariableManager(VariableManager *manager);
    VariableManager* variableManager() const;

    static QString formatVariableReference(const QString &name);
    QString formatColorToHex(const QColor &color) const;

    /**
     * @brief Refreshes all color swatches in the variable table.
     * 
     * Call this after applying a global stylesheet to ensure
     * the color swatches remain visible.
     */
    void refreshColorSwatches();

signals:
    void variableInsertRequested(const QString &reference);

    /**
     * @brief Emitted when a variable is renamed.
     * @param oldName The previous name.
     * @param newName The new name.
     *
     * References in the template have to be rewritten to match, which only the
     * owner of the document can do.
     */
    void variableRenamed(const QString &oldName, const QString &newName);

private slots:
    /**
     * @brief Hides rows whose name or value does not contain @p text.
     */
    void setFilter(const QString &text);

    void onItemChanged(QTableWidgetItem *item);
    void onCellClicked(int row, int column);
    void onRowDeleteClicked();
    void onRowInsertClicked();
    
    // Slots for VariableManager signals
    void onVariableChanged(const QString &name, const QString &value);
    void onVariableRemoved(const QString &name);
    void onVariablesCleared();

private:
    /**
     * @brief Applies a rename requested by editing the name cell.
     * @return true if the rename was accepted.
     */
    bool tryRenameVariable(int row, const QString &oldName, const QString &newName);

    void setupUi();
    void setupConnections();
    void refreshVariableList();
    void updateColorSwatch(int row, const QString &value);
    int findRowByName(const QString &name) const;
    void createRowButtons(int row, const QString &name);
    void openColorPickerForRow(int row);
    QColor parseColor(const QString &value) const;
    void ensureEmptyRowExists();
    void tryCreateVariableFromEmptyRow(int row);

    VariableManager *m_variableManager;
    QLineEdit *m_filterEdit;
    QTableWidget *m_variableTable;
    
    // Column indices
    static constexpr int COL_DELETE = 0;
    static constexpr int COL_NAME = 1;
    static constexpr int COL_VALUE = 2;
    static constexpr int COL_COLOR = 3;
    static constexpr int COL_INSERT = 4;
    
    bool m_updatingTable;
};

#endif // VARIABLEPANEL_H
