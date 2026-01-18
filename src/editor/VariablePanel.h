#ifndef VARIABLEPANEL_H
#define VARIABLEPANEL_H

#include <QWidget>
#include <QString>
#include <QColor>

class QTableWidget;
class QTableWidgetItem;
class QPushButton;
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

signals:
    void variableInsertRequested(const QString &reference);

private slots:
    void onItemChanged(QTableWidgetItem *item);
    void onCellClicked(int row, int column);
    void onRowDeleteClicked();
    void onRowInsertClicked();
    
    // Slots for VariableManager signals
    void onVariableChanged(const QString &name, const QString &value);
    void onVariableRemoved(const QString &name);
    void onVariablesCleared();

private:
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
