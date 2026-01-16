#include "VariablePanel.h"
#include "VariableManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QHeaderView>
#include <QColorDialog>
#include <QLabel>
#include <QMessageBox>

VariablePanel::VariablePanel(QWidget *parent)
    : QWidget(parent)
    , m_variableManager(nullptr)
    , m_variableTable(nullptr)
    , m_addButton(nullptr)
    , m_deleteButton(nullptr)
    , m_insertButton(nullptr)
    , m_nameEdit(nullptr)
    , m_valueEdit(nullptr)
    , m_colorPickerButton(nullptr)
    , m_updatingTable(false)
{
    setupUi();
    setupConnections();
    updateButtonStates();
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

    // Variable table
    m_variableTable = new QTableWidget(this);
    m_variableTable->setColumnCount(3);
    m_variableTable->setHorizontalHeaderLabels({tr("Name"), tr("Value"), tr("Color")});
    m_variableTable->horizontalHeader()->setStretchLastSection(false);
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_NAME, QHeaderView::Interactive);
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_VALUE, QHeaderView::Interactive);
    m_variableTable->horizontalHeader()->setSectionResizeMode(COL_COLOR, QHeaderView::Fixed);
    m_variableTable->horizontalHeader()->resizeSection(COL_NAME, 80);
    m_variableTable->horizontalHeader()->resizeSection(COL_VALUE, 80);
    m_variableTable->horizontalHeader()->resizeSection(COL_COLOR, 40);
    m_variableTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_variableTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_variableTable->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_variableTable, 1);

    // Input row for new variables
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(4);
    
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(tr("Variable name"));
    inputLayout->addWidget(m_nameEdit);
    
    m_valueEdit = new QLineEdit(this);
    m_valueEdit->setPlaceholderText(tr("Value"));
    inputLayout->addWidget(m_valueEdit);
    
    m_colorPickerButton = new QPushButton(this);
    m_colorPickerButton->setFixedSize(24, 24);
    m_colorPickerButton->setToolTip(tr("Pick color"));
    m_colorPickerButton->setText(QStringLiteral("..."));
    inputLayout->addWidget(m_colorPickerButton);
    
    mainLayout->addLayout(inputLayout);

    // Button row
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(4);
    
    m_addButton = new QPushButton(tr("Add"), this);
    m_addButton->setToolTip(tr("Add a new variable"));
    buttonLayout->addWidget(m_addButton);
    
    m_deleteButton = new QPushButton(tr("Delete"), this);
    m_deleteButton->setToolTip(tr("Delete selected variable"));
    buttonLayout->addWidget(m_deleteButton);
    
    m_insertButton = new QPushButton(tr("Insert"), this);
    m_insertButton->setToolTip(tr("Insert variable reference at cursor"));
    buttonLayout->addWidget(m_insertButton);
    
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

void VariablePanel::setupConnections()
{
    connect(m_addButton, &QPushButton::clicked, this, &VariablePanel::onAddVariable);
    connect(m_deleteButton, &QPushButton::clicked, this, &VariablePanel::onDeleteVariable);
    connect(m_insertButton, &QPushButton::clicked, this, &VariablePanel::onInsertVariable);
    connect(m_colorPickerButton, &QPushButton::clicked, this, &VariablePanel::onColorButtonClicked);
    connect(m_variableTable, &QTableWidget::itemChanged, this, &VariablePanel::onVariableValueChanged);
    connect(m_variableTable, &QTableWidget::itemSelectionChanged, this, &VariablePanel::onTableSelectionChanged);
    
    // Allow Enter key to add variable
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &VariablePanel::onAddVariable);
    connect(m_valueEdit, &QLineEdit::returnPressed, this, &VariablePanel::onAddVariable);
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

void VariablePanel::onAddVariable()
{
    if (!m_variableManager) {
        return;
    }
    
    QString name = m_nameEdit->text().trimmed();
    QString value = m_valueEdit->text();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Invalid Name"),
                            tr("Variable name cannot be empty."));
        m_nameEdit->setFocus();
        return;
    }
    
    if (!VariableManager::isValidVariableName(name)) {
        QMessageBox::warning(this, tr("Invalid Name"),
                            tr("Variable name must start with a letter or underscore "
                               "and contain only letters, numbers, underscores, or hyphens."));
        m_nameEdit->setFocus();
        return;
    }
    
    if (m_variableManager->hasVariable(name)) {
        QMessageBox::warning(this, tr("Duplicate Name"),
                            tr("A variable with this name already exists."));
        m_nameEdit->setFocus();
        return;
    }
    
    m_variableManager->setVariable(name, value);
    
    // Clear inputs
    m_nameEdit->clear();
    m_valueEdit->clear();
    m_nameEdit->setFocus();
}

void VariablePanel::onDeleteVariable()
{
    if (!m_variableManager) {
        return;
    }
    
    int row = m_variableTable->currentRow();
    if (row < 0) {
        return;
    }
    
    QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
    if (nameItem) {
        m_variableManager->removeVariable(nameItem->text());
    }
}

void VariablePanel::onInsertVariable()
{
    int row = m_variableTable->currentRow();
    if (row < 0) {
        return;
    }
    
    QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
    if (nameItem) {
        QString reference = formatVariableReference(nameItem->text());
        emit variableInsertRequested(reference);
    }
}

void VariablePanel::onVariableValueChanged(QTableWidgetItem *item)
{
    if (m_updatingTable || !m_variableManager) {
        return;
    }
    
    // Only handle value column changes
    if (item->column() != COL_VALUE) {
        return;
    }
    
    int row = item->row();
    QTableWidgetItem *nameItem = m_variableTable->item(row, COL_NAME);
    if (!nameItem) {
        return;
    }
    
    QString name = nameItem->text();
    QString newValue = item->text();
    
    m_variableManager->setVariable(name, newValue);
    updateColorSwatch(row, newValue);
}

void VariablePanel::onColorButtonClicked()
{
    QColor initialColor = Qt::white;
    QString currentValue = m_valueEdit->text();
    
    if (VariableManager::isColorValue(currentValue)) {
        initialColor = parseColor(currentValue);
    }
    
    QColor color = QColorDialog::getColor(initialColor, this, tr("Select Color"),
                                          QColorDialog::ShowAlphaChannel);
    
    if (color.isValid()) {
        QString colorStr;
        if (color.alpha() < 255) {
            // Use #AARRGGBB format for colors with alpha
            colorStr = QStringLiteral("#%1%2%3%4")
                .arg(color.alpha(), 2, 16, QLatin1Char('0'))
                .arg(color.red(), 2, 16, QLatin1Char('0'))
                .arg(color.green(), 2, 16, QLatin1Char('0'))
                .arg(color.blue(), 2, 16, QLatin1Char('0'));
        } else {
            // Use #RRGGBB format for opaque colors
            colorStr = color.name();
        }
        m_valueEdit->setText(colorStr);
    }
}

void VariablePanel::onTableSelectionChanged()
{
    updateButtonStates();
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
        // Add new row
        row = m_variableTable->rowCount();
        m_variableTable->insertRow(row);
        
        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        m_variableTable->setItem(row, COL_NAME, nameItem);
        
        QTableWidgetItem *valueItem = new QTableWidgetItem(value);
        m_variableTable->setItem(row, COL_VALUE, valueItem);
        
        QTableWidgetItem *colorItem = new QTableWidgetItem();
        colorItem->setFlags(colorItem->flags() & ~Qt::ItemIsEditable);
        m_variableTable->setItem(row, COL_COLOR, colorItem);
        
        updateColorSwatch(row, value);
    }
    
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
    updateButtonStates();
}

void VariablePanel::onVariablesCleared()
{
    m_updatingTable = true;
    m_variableTable->setRowCount(0);
    m_updatingTable = false;
    updateButtonStates();
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
    updateButtonStates();
}

void VariablePanel::updateColorSwatch(int row, const QString &value)
{
    QTableWidgetItem *colorItem = m_variableTable->item(row, COL_COLOR);
    if (!colorItem) {
        return;
    }
    
    if (VariableManager::isColorValue(value)) {
        QColor color = parseColor(value);
        if (color.isValid()) {
            colorItem->setBackground(color);
            colorItem->setText(QString());
            return;
        }
    }
    
    // Not a color - clear the swatch
    colorItem->setBackground(QBrush());
    colorItem->setText(QString());
}

void VariablePanel::updateButtonStates()
{
    bool hasSelection = m_variableTable->currentRow() >= 0;
    m_deleteButton->setEnabled(hasSelection);
    m_insertButton->setEnabled(hasSelection);
}

int VariablePanel::findRowByName(const QString &name) const
{
    for (int row = 0; row < m_variableTable->rowCount(); ++row) {
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
