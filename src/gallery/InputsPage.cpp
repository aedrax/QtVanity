#include "InputsPage.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QSlider>
#include <QScrollBar>
#include <QDial>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QKeySequenceEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QStyle>

InputsPage::InputsPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

InputsPage::~InputsPage()
{
    // Qt handles child widget deletion
}

void InputsPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_widgets) {
        widget->setEnabled(enabled);
    }
}

void InputsPage::setReadOnly(bool readOnly)
{
    for (QLineEdit *lineEdit : m_lineEdits) {
        lineEdit->setReadOnly(readOnly);
    }
    for (QTextEdit *textEdit : m_textEdits) {
        textEdit->setReadOnly(readOnly);
    }
    for (QPlainTextEdit *plainTextEdit : m_plainTextEdits) {
        plainTextEdit->setReadOnly(readOnly);
    }
}

void InputsPage::setupWidgets()
{
    setupLineEdits();
    setupTextEdits();
    setupSpinBoxes();
    setupComboBoxes();
    setupSliders();
    setupScrollBars();
    setupDateTimeEdits();
    setupAdvancedInputs();
}

void InputsPage::setupLineEdits()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Line Edits")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QFormLayout *form = new QFormLayout();
    form->setSpacing(8);
    
    // Normal line edit
    QLineEdit *normalEdit = new QLineEdit(group);
    normalEdit->setText(tr("Normal text"));
    normalEdit->setPlaceholderText(tr("Enter text..."));
    form->addRow(tr("Normal:"), normalEdit);
    m_widgets.append(normalEdit);
    m_lineEdits.append(normalEdit);
    
    // Password line edit
    QLineEdit *passwordEdit = new QLineEdit(group);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setText("password123");
    passwordEdit->setPlaceholderText(tr("Enter password..."));
    form->addRow(tr("Password:"), passwordEdit);
    m_widgets.append(passwordEdit);
    m_lineEdits.append(passwordEdit);
    
    // Read-only line edit
    QLineEdit *readOnlyEdit = new QLineEdit(group);
    readOnlyEdit->setText(tr("Read-only text"));
    readOnlyEdit->setReadOnly(true);
    form->addRow(tr("Read-only:"), readOnlyEdit);
    m_widgets.append(readOnlyEdit);
    // Note: Not adding to m_lineEdits since it's always read-only
    
    // Line edit with clear button
    QLineEdit *clearableEdit = new QLineEdit(group);
    clearableEdit->setText(tr("Clearable"));
    clearableEdit->setClearButtonEnabled(true);
    form->addRow(tr("Clearable:"), clearableEdit);
    m_widgets.append(clearableEdit);
    m_lineEdits.append(clearableEdit);
    
    groupLayout->addLayout(form);
}

void InputsPage::setupTextEdits()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Text Edits")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(12);
    
    // Normal text edit
    QVBoxLayout *normalCol = new QVBoxLayout();
    QLabel *normalLabel = new QLabel(tr("Normal:"), group);
    QTextEdit *normalEdit = new QTextEdit(group);
    normalEdit->setPlainText(tr("This is a normal text edit.\nYou can type multiple lines here."));
    normalEdit->setMaximumHeight(100);
    normalCol->addWidget(normalLabel);
    normalCol->addWidget(normalEdit);
    row->addLayout(normalCol);
    m_widgets.append(normalEdit);
    m_textEdits.append(normalEdit);
    
    // Read-only text edit
    QVBoxLayout *readOnlyCol = new QVBoxLayout();
    QLabel *readOnlyLabel = new QLabel(tr("Read-only:"), group);
    QTextEdit *readOnlyEdit = new QTextEdit(group);
    readOnlyEdit->setPlainText(tr("This text edit is read-only.\nYou cannot modify this text."));
    readOnlyEdit->setReadOnly(true);
    readOnlyEdit->setMaximumHeight(100);
    readOnlyCol->addWidget(readOnlyLabel);
    readOnlyCol->addWidget(readOnlyEdit);
    row->addLayout(readOnlyCol);
    m_widgets.append(readOnlyEdit);
    // Note: Not adding to m_textEdits since it's always read-only
    
    groupLayout->addLayout(row);
}

void InputsPage::setupSpinBoxes()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Spin Boxes")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QFormLayout *form = new QFormLayout();
    form->setSpacing(8);
    
    // Integer spin box
    QSpinBox *intSpin = new QSpinBox(group);
    intSpin->setRange(0, 100);
    intSpin->setValue(42);
    intSpin->setSuffix(tr(" items"));
    form->addRow(tr("Integer:"), intSpin);
    m_widgets.append(intSpin);
    
    // Double spin box
    QDoubleSpinBox *doubleSpin = new QDoubleSpinBox(group);
    doubleSpin->setRange(0.0, 100.0);
    doubleSpin->setValue(3.14159);
    doubleSpin->setDecimals(5);
    doubleSpin->setSingleStep(0.1);
    form->addRow(tr("Double:"), doubleSpin);
    m_widgets.append(doubleSpin);
    
    // Spin box with prefix
    QSpinBox *prefixSpin = new QSpinBox(group);
    prefixSpin->setRange(0, 1000);
    prefixSpin->setValue(250);
    prefixSpin->setPrefix(tr("$ "));
    form->addRow(tr("With prefix:"), prefixSpin);
    m_widgets.append(prefixSpin);
    
    groupLayout->addLayout(form);
}

void InputsPage::setupComboBoxes()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Combo Boxes")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QFormLayout *form = new QFormLayout();
    form->setSpacing(8);
    
    // Non-editable combo box
    QComboBox *normalCombo = new QComboBox(group);
    normalCombo->addItems({tr("Option 1"), tr("Option 2"), tr("Option 3"), tr("Option 4")});
    normalCombo->setCurrentIndex(1);
    form->addRow(tr("Normal:"), normalCombo);
    m_widgets.append(normalCombo);
    
    // Editable combo box
    QComboBox *editableCombo = new QComboBox(group);
    editableCombo->setEditable(true);
    editableCombo->addItems({tr("Apple"), tr("Banana"), tr("Cherry"), tr("Date")});
    editableCombo->setCurrentText(tr("Custom text"));
    form->addRow(tr("Editable:"), editableCombo);
    m_widgets.append(editableCombo);
    
    // Combo box with icons
    QComboBox *iconCombo = new QComboBox(group);
    iconCombo->addItem(style()->standardIcon(QStyle::SP_DirIcon), tr("Directory"));
    iconCombo->addItem(style()->standardIcon(QStyle::SP_FileIcon), tr("File"));
    iconCombo->addItem(style()->standardIcon(QStyle::SP_DriveHDIcon), tr("Drive"));
    form->addRow(tr("With icons:"), iconCombo);
    m_widgets.append(iconCombo);
    
    groupLayout->addLayout(form);
}

void InputsPage::setupSliders()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Sliders and Dials")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Horizontal sliders
    QHBoxLayout *hSliderRow = new QHBoxLayout();
    hSliderRow->setSpacing(12);
    
    QVBoxLayout *hSlider1Col = new QVBoxLayout();
    QLabel *hSlider1Label = new QLabel(tr("Horizontal:"), group);
    QSlider *hSlider1 = new QSlider(Qt::Horizontal, group);
    hSlider1->setRange(0, 100);
    hSlider1->setValue(50);
    hSlider1Col->addWidget(hSlider1Label);
    hSlider1Col->addWidget(hSlider1);
    hSliderRow->addLayout(hSlider1Col);
    m_widgets.append(hSlider1);
    
    QVBoxLayout *hSlider2Col = new QVBoxLayout();
    QLabel *hSlider2Label = new QLabel(tr("With ticks:"), group);
    QSlider *hSlider2 = new QSlider(Qt::Horizontal, group);
    hSlider2->setRange(0, 100);
    hSlider2->setValue(75);
    hSlider2->setTickPosition(QSlider::TicksBelow);
    hSlider2->setTickInterval(10);
    hSlider2Col->addWidget(hSlider2Label);
    hSlider2Col->addWidget(hSlider2);
    hSliderRow->addLayout(hSlider2Col);
    m_widgets.append(hSlider2);
    
    groupLayout->addLayout(hSliderRow);
    
    // Vertical sliders and dials
    QHBoxLayout *vRow = new QHBoxLayout();
    vRow->setSpacing(24);
    
    // Vertical slider
    QVBoxLayout *vSliderCol = new QVBoxLayout();
    QLabel *vSliderLabel = new QLabel(tr("Vertical:"), group);
    QSlider *vSlider = new QSlider(Qt::Vertical, group);
    vSlider->setRange(0, 100);
    vSlider->setValue(30);
    vSlider->setFixedHeight(100);
    vSliderCol->addWidget(vSliderLabel);
    vSliderCol->addWidget(vSlider, 0, Qt::AlignHCenter);
    vRow->addLayout(vSliderCol);
    m_widgets.append(vSlider);
    
    // Vertical slider with ticks
    QVBoxLayout *vSlider2Col = new QVBoxLayout();
    QLabel *vSlider2Label = new QLabel(tr("With ticks:"), group);
    QSlider *vSlider2 = new QSlider(Qt::Vertical, group);
    vSlider2->setRange(0, 100);
    vSlider2->setValue(60);
    vSlider2->setTickPosition(QSlider::TicksRight);
    vSlider2->setTickInterval(20);
    vSlider2->setFixedHeight(100);
    vSlider2Col->addWidget(vSlider2Label);
    vSlider2Col->addWidget(vSlider2, 0, Qt::AlignHCenter);
    vRow->addLayout(vSlider2Col);
    m_widgets.append(vSlider2);
    
    // Dial
    QVBoxLayout *dialCol = new QVBoxLayout();
    QLabel *dialLabel = new QLabel(tr("Dial:"), group);
    QDial *dial = new QDial(group);
    dial->setRange(0, 100);
    dial->setValue(45);
    dial->setFixedSize(80, 80);
    dialCol->addWidget(dialLabel);
    dialCol->addWidget(dial, 0, Qt::AlignHCenter);
    vRow->addLayout(dialCol);
    m_widgets.append(dial);
    
    // Dial with notches
    QVBoxLayout *dial2Col = new QVBoxLayout();
    QLabel *dial2Label = new QLabel(tr("With notches:"), group);
    QDial *dial2 = new QDial(group);
    dial2->setRange(0, 100);
    dial2->setValue(70);
    dial2->setNotchesVisible(true);
    dial2->setFixedSize(80, 80);
    dial2Col->addWidget(dial2Label);
    dial2Col->addWidget(dial2, 0, Qt::AlignHCenter);
    vRow->addLayout(dial2Col);
    m_widgets.append(dial2);
    
    vRow->addStretch();
    groupLayout->addLayout(vRow);
}

void InputsPage::setupDateTimeEdits()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Date/Time Edits")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QFormLayout *form = new QFormLayout();
    form->setSpacing(8);
    
    // Date edit
    QDateEdit *dateEdit = new QDateEdit(group);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    form->addRow(tr("Date:"), dateEdit);
    m_widgets.append(dateEdit);
    
    // Time edit
    QTimeEdit *timeEdit = new QTimeEdit(group);
    timeEdit->setTime(QTime::currentTime());
    form->addRow(tr("Time:"), timeEdit);
    m_widgets.append(timeEdit);
    
    // DateTime edit
    QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(group);
    dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    dateTimeEdit->setCalendarPopup(true);
    form->addRow(tr("Date/Time:"), dateTimeEdit);
    m_widgets.append(dateTimeEdit);
    
    // Date edit with custom format
    QDateEdit *customDateEdit = new QDateEdit(group);
    customDateEdit->setDate(QDate::currentDate());
    customDateEdit->setDisplayFormat("yyyy-MM-dd");
    form->addRow(tr("Custom format:"), customDateEdit);
    m_widgets.append(customDateEdit);
    
    groupLayout->addLayout(form);
}

void InputsPage::setupAdvancedInputs()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Advanced Inputs")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QFormLayout *form = new QFormLayout();
    form->setSpacing(8);
    
    // Font combo box
    QFontComboBox *fontCombo = new QFontComboBox(group);
    fontCombo->setCurrentFont(QFont("Arial"));
    form->addRow(tr("Font:"), fontCombo);
    m_widgets.append(fontCombo);
    
    // Key sequence edit
    QKeySequenceEdit *keySeqEdit = new QKeySequenceEdit(group);
    keySeqEdit->setKeySequence(QKeySequence(tr("Ctrl+S")));
    form->addRow(tr("Shortcut:"), keySeqEdit);
    m_widgets.append(keySeqEdit);
    
    groupLayout->addLayout(form);
    
    // Plain text edit
    QHBoxLayout *textRow = new QHBoxLayout();
    textRow->setSpacing(12);
    
    QVBoxLayout *plainTextCol = new QVBoxLayout();
    QLabel *plainTextLabel = new QLabel(tr("Plain Text Edit:"), group);
    QPlainTextEdit *plainTextEdit = new QPlainTextEdit(group);
    plainTextEdit->setPlainText(tr("This is a plain text edit.\nIt's simpler than QTextEdit."));
    plainTextEdit->setMaximumHeight(100);
    plainTextCol->addWidget(plainTextLabel);
    plainTextCol->addWidget(plainTextEdit);
    textRow->addLayout(plainTextCol);
    m_widgets.append(plainTextEdit);
    m_plainTextEdits.append(plainTextEdit);
    
    groupLayout->addLayout(textRow);
}

void InputsPage::setupScrollBars()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Scroll Bars")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Horizontal scroll bar
    QVBoxLayout *hScrollCol = new QVBoxLayout();
    QLabel *hScrollLabel = new QLabel(tr("Horizontal:"), group);
    QScrollBar *hScrollBar = new QScrollBar(Qt::Horizontal, group);
    hScrollBar->setRange(0, 100);
    hScrollBar->setValue(50);
    hScrollCol->addWidget(hScrollLabel);
    hScrollCol->addWidget(hScrollBar);
    groupLayout->addLayout(hScrollCol);
    m_widgets.append(hScrollBar);
    
    // Vertical scroll bar row
    QHBoxLayout *vScrollRow = new QHBoxLayout();
    vScrollRow->setSpacing(24);
    
    QVBoxLayout *vScrollCol = new QVBoxLayout();
    QLabel *vScrollLabel = new QLabel(tr("Vertical:"), group);
    QScrollBar *vScrollBar = new QScrollBar(Qt::Vertical, group);
    vScrollBar->setRange(0, 100);
    vScrollBar->setValue(30);
    vScrollBar->setFixedHeight(100);
    vScrollCol->addWidget(vScrollLabel);
    vScrollCol->addWidget(vScrollBar, 0, Qt::AlignHCenter);
    vScrollRow->addLayout(vScrollCol);
    m_widgets.append(vScrollBar);
    
    vScrollRow->addStretch();
    groupLayout->addLayout(vScrollRow);
}
