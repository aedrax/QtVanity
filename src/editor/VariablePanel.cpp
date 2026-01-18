#include "VariablePanel.h"
#include "VariableManager.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QHeaderView>
#include <QColorDialog>
#include <QLabel>
#include <QMessageBox>

VariablePanel::VariablePanel(QWidget *parent)
    : QWidget(parent)
    , m_variableManager(nullptr)
    , m_variableTable(nullptr)
    , m_updatingTable(false)
{
    setupUi();
    setupConnections();
}

VariablePanel::~VariablePanel()
{
}

void VariablePanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Allow the panel to be resized very small
    setMinimumWidth(50);

    // Title label
    QLabel *titleLabel = new QLabel(tr("Variables"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Variable table with 5 columns: Delete, Name, Value, Color, Insert
    m_variableTable = new QTableWidget(this);
    m_variableTable->setColumnCount(5);
    m_variableTable->setHorizontalHeaderLabels({QString(), tr("Name"), tr("Value"), tr("Color"), QString()});
    m_variableTable->horizontalHeader()->setStretchLastSection(false);
    
    // Delete button column - resize to content
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_DELETE, QHeaderView::ResizeToContents);
    
    // Name and Value columns - interactive
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_NAME, QHeaderView::Interactive);
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_VALUE, QHeaderView::Interactive);
    m_variableTable->horizontalHeader()->resizeSection(COL_NAME, 70);
    m_variableTable->horizontalHeader()->resizeSection(COL_VALUE, 70);
    
    // Color column - fixed width
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_COLOR, QHeaderView::Fixed);
    QFontMetrics fm(m_variableTable->horizontalHeader()->font());
    int colorHeaderWidth = fm.horizontalAdvance(tr("Color")) + 16;
    int minColorWidth = qMax(40, colorHeaderWidth);
    m_variableTable->horizontalHeader()->resizeSection(COL_COLOR, minColorWidth);
    
    // Insert button column - resize to content
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_INSERT, QHeaderView::ResizeToContents);
    
    m_variableTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_variableTable->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_variableTable, 1);
    
    // Add the empty input row
    ensureEmptyRowExists();
}

void VariablePanel::setupConnections()
{
    connect(m_variableTable, &QTableWidget::itemChanged, this, &VariablePanel::onItemChanged);
    connect(m_variableTable, &QTableWidget::cellClicked, this, &VariablePanel::onCellClicked);
}

void VariablePanel::setVariableManager(VariableManager *manager)
{
    // Disconnect from old manager
    if (m_variableManager) {
        disconnect(m_variableManager, nullptr, this, nullptr);
    }
    
    m_variableManager = manager;
    
    // Connect to new manager
    if (m_variableManager) {
        connect(m_variableManager, &VariableManager::variableChanged,
                this, &VariablePanel::onVariableChanged);
        connect(m_variableManager, &VariableManager::variableRemoved,
                this, &VariablePanel::onVariableRemoved);
        connect(m_variableManager, &VariableManager::variablesCleared,
                this, &VariablePanel::onVariablesCleared);
        connect(m_variableManager, &VariableManager::projectLoaded,
                this, &VariablePanel::refreshVariableList);
    }
    
    refreshVariableList();
}

VariableManager* VariablePanel::variableManager() const
{
    return m_variableManager;
}

QString VariablePanel::formatVariableReference(const QString &name)
{
    return QStringLiteral("${%1}").arg(name);
}

void VariablePanel::onItemChanged(QTableWidgetItem *item)
{
    if (m_updatingTable || !m_variableManager) {
        return;
    }
    
    int row = item->row();
    int col = item->column();
    
    // Check if this is the empty row (last row)
    bool isEmptyRow = (row == m_variableTable->rowCount() - 1);
    
    if (isEmptyRow) {
        // Handle new variable creation from empty row
        if (col == COL_NAME || col == COL_VALUE) {
            tryCreateVariableFromEmptyRow(row);
        }
    } else {
        // Handle existing variable value change
        if (col == COL_VALUE) {
            QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
            if (nameItem) {
                QString name = nameItem->text();
                QString newValue = item->text();
                m_variableManager->setVariable(name, newValue);
                updateColorSwatch(row, newValue);
            }
        }
    }
}

void VariablePanel::tryCreateVariableFromEmptyRow(int row)
{
    QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
    QTableWidgetItem *valueItem = m_variableTable->item(row, COL_VALUE);
    
    QString name = nameItem ? nameItem->text().trimmed() : QString();
    QString value = valueItem ? valueItem->text() : QString();
    
    // Need at least a name to create a variable
    if (name.isEmpty()) {
        return;
    }
    
    // Validate the name
    if (!VariableManager::isValidVariableName(name)) {
        QMessageBox::warning(this, tr("Invalid Name"),
                            tr("Variable name must start with a letter or underscore "
                               "and contain only letters, numbers, underscores, or hyphens."));
        // Clear the invalid name
        m_updatingTable = true;
        if (nameItem) nameItem->setText(QString());
        m_updatingTable = false;
        return;
    }
    
    if (m_variableManager->hasVariable(name)) {
        QMessageBox::warning(this, tr("Duplicate Name"),
                            tr("A variable with this name already exists."));
        // Clear the duplicate name
        m_updatingTable = true;
        if (nameItem) nameItem->setText(QString());
        m_updatingTable = false;
        return;
    }
    
    // Create the variable - this will trigger onVariableChanged which converts
    // the empty row to a proper variable row and adds a new empty row
    m_variableManager->setVariable(name, value);
}

void VariablePanel::onCellClicked(int row, int column)
{
    // Don't handle color picker for the empty row
    if (row == m_variableTable->rowCount() - 1) {
        return;
    }
    
    // Only handle clicks on the color column
    if (column == COL_COLOR) {
        openColorPickerForRow(row);
    }
}

void VariablePanel::onRowDeleteClicked()
{
    if (!m_variableManager) {
        return;
    }
    
    // Get the button that was clicked
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) {
        return;
    }
    
    // Find the row containing this button (exclude the empty row)
    for (int row = 0; row < m_variableTable->rowCount() - 1; ++row) {
        QWidget *widget = m_variableTable->cellWidget(row, COL_DELETE);
        if (widget == button) {
            QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
            if (nameItem) {
                m_variableManager->removeVariable(nameItem->text());
            }
            return;
        }
    }
}

void VariablePanel::onRowInsertClicked()
{
    // Get the button that was clicked
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) {
        return;
    }
    
    // Find the row containing this button (exclude the empty row)
    for (int row = 0; row < m_variableTable->rowCount() - 1; ++row) {
        QWidget *widget = m_variableTable->cellWidget(row, COL_INSERT);
        if (widget == button) {
            QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
            if (nameItem) {
                QString reference = formatVariableReference(nameItem->text());
                emit variableInsertRequested(reference);
            }
            return;
        }
    }
}

void VariablePanel::openColorPickerForRow(int row)
{
    if (!m_variableManager) {
        return;
    }
    
    // Get the variable name from the row
    QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
    if (!nameItem) {
        return;
    }
    QString name = nameItem->text();
    
    // Get current value from the value cell
    QTableWidgetItem *valueItem = m_variableTable->item(row, COL_VALUE);
    QString currentValue = valueItem ? valueItem->text() : QString();
    
    // Parse color using existing parseColor() method, use white as default
    QColor initialColor = Qt::white;
    if (!currentValue.isEmpty()) {
        QColor parsed = parseColor(currentValue);
        if (parsed.isValid()) {
            initialColor = parsed;
        }
    }
    
    // Open QColorDialog with initial color
    QColor color = QColorDialog::getColor(initialColor, this, tr("Select Color"),
                                          QColorDialog::ShowAlphaChannel);
    
    // If user confirms, format color and update variable via VariableManager
    if (color.isValid()) {
        QString colorStr = formatColorToHex(color);
        m_variableManager->setVariable(name, colorStr);
    }
}

void VariablePanel::onVariableChanged(const QString &name, const QString &value)
{
    m_updatingTable = true;
    
    int row = findRowByName(name);
    if (row >= 0) {
        // Update existing row
        QTableWidgetItem *valueItem = m_variableTable->item(row, COL_VALUE);
        if (valueItem && valueItem->text() != value) {
            valueItem->setText(value);
        }
        updateColorSwatch(row, value);
    } else {
        // Insert new row before the empty row (which is always last)
        int emptyRowIndex = m_variableTable->rowCount() - 1;
        m_variableTable->insertRow(emptyRowIndex);
        row = emptyRowIndex;
        
        // Create delete and insert buttons for this row
        createRowButtons(row, name);
        
        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        m_variableTable->setItem(row, COL_NAME, nameItem);
        
        QTableWidgetItem *valueItem = new QTableWidgetItem(value);
        m_variableTable->setItem(row, COL_VALUE, valueItem);
        
        QTableWidgetItem *colorItem = new QTableWidgetItem();
        colorItem->setFlags(colorItem->flags() & ~Qt::ItemIsEditable);
        m_variableTable->setItem(row, COL_COLOR, colorItem);
        
        updateColorSwatch(row, value);
        
        // Clear the empty row (now at the end) in case it had data
        int newEmptyRow = m_variableTable->rowCount() - 1;
        QTableWidgetItem *emptyNameItem = m_variableTable->item(newEmptyRow, COL_NAME);
        QTableWidgetItem *emptyValueItem = m_variableTable->item(newEmptyRow, COL_VALUE);
        if (emptyNameItem) emptyNameItem->setText(QString());
        if (emptyValueItem) emptyValueItem->setText(QString());
    }
    
    m_updatingTable = false;
}

void VariablePanel::createRowButtons(int row, const QString &name)
{
    Q_UNUSED(name);
    
    // Create delete button - let it size naturally
    QPushButton *deleteBtn = new QPushButton(QStringLiteral("×"), this);
    deleteBtn->setToolTip(tr("Delete this variable"));
    deleteBtn->setStyleSheet(QStringLiteral("QPushButton { color: #c00; font-weight: bold; padding: 2px; margin: 0; }"));
    connect(deleteBtn, &QPushButton::clicked, this, &VariablePanel::onRowDeleteClicked);
    m_variableTable->setCellWidget(row, COL_DELETE, deleteBtn);
    
    // Create insert button - let it size naturally
    QPushButton *insertBtn = new QPushButton(QStringLiteral("→"), this);
    insertBtn->setToolTip(tr("Insert variable reference at cursor"));
    insertBtn->setStyleSheet(QStringLiteral("QPushButton { padding: 2px; margin: 0; }"));
    connect(insertBtn, &QPushButton::clicked, this, &VariablePanel::onRowInsertClicked);
    m_variableTable->setCellWidget(row, COL_INSERT, insertBtn);
}

void VariablePanel::ensureEmptyRowExists()
{
    m_updatingTable = true;
    
    int rowCount = m_variableTable->rowCount();
    
    // Check if last row is already empty
    if (rowCount > 0) {
        QTableWidgetItem *nameItem = m_variableTable->item(rowCount - 1, COL_NAME);
        QTableWidgetItem *valueItem = m_variableTable->item(rowCount - 1, COL_VALUE);
        bool nameEmpty = !nameItem || nameItem->text().isEmpty();
        bool valueEmpty = !valueItem || valueItem->text().isEmpty();
        
        // Check if it has no buttons (empty row indicator)
        QWidget *deleteWidget = m_variableTable->cellWidget(rowCount - 1, COL_DELETE);
        if (nameEmpty && valueEmpty && !deleteWidget) {
            m_updatingTable = false;
            return; // Empty row already exists
        }
    }
    
    // Add new empty row
    int newRow = rowCount;
    m_variableTable->insertRow(newRow);
    
    // Create editable name and value items
    QTableWidgetItem *nameItem = new QTableWidgetItem();
    m_variableTable->setItem(newRow, COL_NAME, nameItem);
    
    QTableWidgetItem *valueItem = new QTableWidgetItem();
    m_variableTable->setItem(newRow, COL_VALUE, valueItem);
    
    // Create non-editable color item (empty for new row)
    QTableWidgetItem *colorItem = new QTableWidgetItem();
    colorItem->setFlags(colorItem->flags() & ~Qt::ItemIsEditable);
    m_variableTable->setItem(newRow, COL_COLOR, colorItem);
    
    // No buttons for the empty row - they get added when variable is created
    
    m_updatingTable = false;
}

void VariablePanel::onVariableRemoved(const QString &name)
{
    m_updatingTable = true;
    
    int row = findRowByName(name);
    if (row >= 0) {
        m_variableTable->removeRow(row);
    }
    
    m_updatingTable = false;
    ensureEmptyRowExists();
}

void VariablePanel::onVariablesCleared()
{
    m_updatingTable = true;
    m_variableTable->setRowCount(0);
    m_updatingTable = false;
    ensureEmptyRowExists();
}

void VariablePanel::refreshVariableList()
{
    m_updatingTable = true;
    m_variableTable->setRowCount(0);
    
    if (m_variableManager) {
        QMap<QString, QString> vars = m_variableManager->allVariables();
        for (auto it = vars.constBegin(); it != vars.constEnd(); ++it) {
            int row = m_variableTable->rowCount();
            m_variableTable->insertRow(row);
            
            // Create delete and insert buttons for this row
            createRowButtons(row, it.key());
            
            QTableWidgetItem *nameItem = new QTableWidgetItem(it.key());
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            m_variableTable->setItem(row, COL_NAME, nameItem);
            
            QTableWidgetItem *valueItem = new QTableWidgetItem(it.value());
            m_variableTable->setItem(row, COL_VALUE, valueItem);
            
            QTableWidgetItem *colorItem = new QTableWidgetItem();
            colorItem->setFlags(colorItem->flags() & ~Qt::ItemIsEditable);
            m_variableTable->setItem(row, COL_COLOR, colorItem);
            
            updateColorSwatch(row, it.value());
        }
    }
    
    m_updatingTable = false;
    ensureEmptyRowExists();
}

void VariablePanel::updateColorSwatch(int row, const QString &value)
{
    if (VariableManager::isColorValue(value)) {
        QColor color = parseColor(value);
        if (color.isValid()) {
            // Use a QWidget with inline stylesheet to prevent global stylesheet override
            QWidget *colorWidget = new QWidget();
            colorWidget->setAutoFillBackground(true);
            // Use inline stylesheet which has higher specificity than global stylesheet
            QString styleSheet = QStringLiteral("background-color: %1; border: 1px solid #888;")
                .arg(color.name(QColor::HexArgb));
            colorWidget->setStyleSheet(styleSheet);
            colorWidget->setToolTip(value);
            m_variableTable->setCellWidget(row, COL_COLOR, colorWidget);
            return;
        }
    }
    
    // Not a color - remove any color widget
    m_variableTable->removeCellWidget(row, COL_COLOR);
}

int VariablePanel::findRowByName(const QString &name) const
{
    // Don't search the empty row (last row)
    for (int row = 0; row < m_variableTable->rowCount() - 1; ++row) {
        QTableWidgetItem *item = m_variableTable->item(row, COL_NAME);
        if (item && item->text() == name) {
            return row;
        }
    }
    return -1;
}

QColor VariablePanel::parseColor(const QString &value) const
{
    if (!VariableManager::isColorValue(value)) {
        return QColor();
    }
    
    QString hex = value.mid(1); // Remove '#'
    
    if (hex.length() == 3) {
        // #RGB -> #RRGGBB
        QString expanded;
        for (int i = 0; i < 3; ++i) {
            expanded += hex[i];
            expanded += hex[i];
        }
        return QColor(QStringLiteral("#") + expanded);
    } else if (hex.length() == 6) {
        // #RRGGBB
        return QColor(value);
    } else if (hex.length() == 8) {
        // #AARRGGBB
        int alpha = hex.mid(0, 2).toInt(nullptr, 16);
        int red = hex.mid(2, 2).toInt(nullptr, 16);
        int green = hex.mid(4, 2).toInt(nullptr, 16);
        int blue = hex.mid(6, 2).toInt(nullptr, 16);
        return QColor(red, green, blue, alpha);
    }
    
    return QColor();
}

QString VariablePanel::formatColorToHex(const QColor &color) const
{
    if (color.alpha() < 255) {
        // Use #AARRGGBB format for colors with alpha
        return QStringLiteral("#%1%2%3%4")
            .arg(color.alpha(), 2, 16, QLatin1Char('0'))
            .arg(color.red(), 2, 16, QLatin1Char('0'))
            .arg(color.green(), 2, 16, QLatin1Char('0'))
            .arg(color.blue(), 2, 16, QLatin1Char('0'));
    } else {
        // Use #RRGGBB format for opaque colors
        return color.name();
    }
}

void VariablePanel::refreshColorSwatches()
{
    // Refresh all color swatches (exclude the empty row at the end)
    for (int row = 0; row < m_variableTable->rowCount() - 1; ++row) {
        QTableWidgetItem *valueItem = m_variableTable->item(row, COL_VALUE);
        if (valueItem) {
            updateColorSwatch(row, valueItem->text());
        }
    }
}
