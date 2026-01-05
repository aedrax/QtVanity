#include "DialogsPage.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QProgressBar>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

DialogsPage::DialogsPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

DialogsPage::~DialogsPage()
{
    // Qt handles child widget deletion
}

void DialogsPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_widgets) {
        widget->setEnabled(enabled);
    }
}

void DialogsPage::setupWidgets()
{
    setupMessageBoxes();
    setupDialogs();
    setupProgressBars();
}

void DialogsPage::setupMessageBoxes()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Message Boxes")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QLabel *infoLabel = new QLabel(
        tr("Click buttons below to show different message box types:"), group);
    groupLayout->addWidget(infoLabel);
    
    // Row 1: Standard message boxes
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(8);
    
    QPushButton *infoBtn = new QPushButton(tr("Information"), group);
    connect(infoBtn, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, tr("Information"),
            tr("This is an information message box."));
    });
    row1->addWidget(infoBtn);
    m_widgets.append(infoBtn);
    
    QPushButton *warningBtn = new QPushButton(tr("Warning"), group);
    connect(warningBtn, &QPushButton::clicked, [this]() {
        QMessageBox::warning(this, tr("Warning"),
            tr("This is a warning message box."));
    });
    row1->addWidget(warningBtn);
    m_widgets.append(warningBtn);
    
    QPushButton *criticalBtn = new QPushButton(tr("Critical"), group);
    connect(criticalBtn, &QPushButton::clicked, [this]() {
        QMessageBox::critical(this, tr("Critical Error"),
            tr("This is a critical error message box."));
    });
    row1->addWidget(criticalBtn);
    m_widgets.append(criticalBtn);
    
    QPushButton *questionBtn = new QPushButton(tr("Question"), group);
    connect(questionBtn, &QPushButton::clicked, [this]() {
        QMessageBox::question(this, tr("Question"),
            tr("This is a question message box.\nDo you want to continue?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    });
    row1->addWidget(questionBtn);
    m_widgets.append(questionBtn);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
    
    // Row 2: Custom message box
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(8);
    
    QPushButton *customBtn = new QPushButton(tr("Custom Buttons"), group);
    connect(customBtn, &QPushButton::clicked, [this]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Custom Message Box"));
        msgBox.setText(tr("This message box has custom buttons."));
        msgBox.setInformativeText(tr("Choose an action:"));
        msgBox.addButton(tr("Save"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("Discard"), QMessageBox::DestructiveRole);
        msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        msgBox.exec();
    });
    row2->addWidget(customBtn);
    m_widgets.append(customBtn);
    
    QPushButton *detailedBtn = new QPushButton(tr("With Details"), group);
    connect(detailedBtn, &QPushButton::clicked, [this]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Detailed Message"));
        msgBox.setText(tr("An error occurred during the operation."));
        msgBox.setInformativeText(tr("Click 'Show Details' for more information."));
        msgBox.setDetailedText(tr("Error Code: 0x12345\n"
            "Module: FileSystem\n"
            "Description: Unable to access the specified resource.\n"
            "Timestamp: 2025-01-16 10:30:00"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    });
    row2->addWidget(detailedBtn);
    m_widgets.append(detailedBtn);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
}

void DialogsPage::setupDialogs()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Standard Dialogs")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QLabel *infoLabel = new QLabel(
        tr("Click buttons below to show standard dialogs:"), group);
    groupLayout->addWidget(infoLabel);
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(8);
    
    // File dialog - Open
    QPushButton *openFileBtn = new QPushButton(tr("Open File..."), group);
    connect(openFileBtn, &QPushButton::clicked, [this]() {
        QFileDialog::getOpenFileName(this, tr("Open File"),
            QString(), tr("All Files (*);;Text Files (*.txt)"));
    });
    row->addWidget(openFileBtn);
    m_widgets.append(openFileBtn);
    
    // File dialog - Save
    QPushButton *saveFileBtn = new QPushButton(tr("Save File..."), group);
    connect(saveFileBtn, &QPushButton::clicked, [this]() {
        QFileDialog::getSaveFileName(this, tr("Save File"),
            QString(), tr("All Files (*);;Text Files (*.txt)"));
    });
    row->addWidget(saveFileBtn);
    m_widgets.append(saveFileBtn);
    
    // Directory dialog
    QPushButton *dirBtn = new QPushButton(tr("Select Directory..."), group);
    connect(dirBtn, &QPushButton::clicked, [this]() {
        QFileDialog::getExistingDirectory(this, tr("Select Directory"));
    });
    row->addWidget(dirBtn);
    m_widgets.append(dirBtn);
    
    // Color dialog
    QPushButton *colorBtn = new QPushButton(tr("Choose Color..."), group);
    connect(colorBtn, &QPushButton::clicked, [this]() {
        QColorDialog::getColor(Qt::blue, this, tr("Choose Color"));
    });
    row->addWidget(colorBtn);
    m_widgets.append(colorBtn);
    
    // Font dialog
    QPushButton *fontBtn = new QPushButton(tr("Choose Font..."), group);
    connect(fontBtn, &QPushButton::clicked, [this]() {
        bool ok;
        QFontDialog::getFont(&ok, this->font(), this, tr("Choose Font"));
    });
    row->addWidget(fontBtn);
    m_widgets.append(fontBtn);
    
    row->addStretch();
    groupLayout->addLayout(row);
}

void DialogsPage::setupProgressBars()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Progress Bars")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Determinate progress bar
    QHBoxLayout *detRow = new QHBoxLayout();
    QLabel *detLabel = new QLabel(tr("Determinate (50%):"), group);
    detLabel->setMinimumWidth(150);
    QProgressBar *detProgress = new QProgressBar(group);
    detProgress->setRange(0, 100);
    detProgress->setValue(50);
    detRow->addWidget(detLabel);
    detRow->addWidget(detProgress, 1);
    groupLayout->addLayout(detRow);
    m_widgets.append(detProgress);
    
    // Determinate progress bar with text
    QHBoxLayout *textRow = new QHBoxLayout();
    QLabel *textLabel = new QLabel(tr("With text (75%):"), group);
    textLabel->setMinimumWidth(150);
    QProgressBar *textProgress = new QProgressBar(group);
    textProgress->setRange(0, 100);
    textProgress->setValue(75);
    textProgress->setFormat(tr("%v of %m (%p%)"));
    textRow->addWidget(textLabel);
    textRow->addWidget(textProgress, 1);
    groupLayout->addLayout(textRow);
    m_widgets.append(textProgress);
    
    // Indeterminate progress bar
    QHBoxLayout *indRow = new QHBoxLayout();
    QLabel *indLabel = new QLabel(tr("Indeterminate:"), group);
    indLabel->setMinimumWidth(150);
    QProgressBar *indProgress = new QProgressBar(group);
    indProgress->setRange(0, 0); // Indeterminate mode
    indRow->addWidget(indLabel);
    indRow->addWidget(indProgress, 1);
    groupLayout->addLayout(indRow);
    m_widgets.append(indProgress);
    
    // Animated progress bar
    QHBoxLayout *animRow = new QHBoxLayout();
    QLabel *animLabel = new QLabel(tr("Animated:"), group);
    animLabel->setMinimumWidth(150);
    QProgressBar *animProgress = new QProgressBar(group);
    animProgress->setRange(0, 100);
    animProgress->setValue(0);
    animRow->addWidget(animLabel);
    animRow->addWidget(animProgress, 1);
    groupLayout->addLayout(animRow);
    m_widgets.append(animProgress);
    
    // Timer to animate the progress bar
    QTimer *animTimer = new QTimer(group);
    connect(animTimer, &QTimer::timeout, [animProgress]() {
        int value = animProgress->value();
        value = (value + 2) % 101;
        animProgress->setValue(value);
    });
    animTimer->start(100);
    
    // Control buttons
    QHBoxLayout *ctrlRow = new QHBoxLayout();
    QPushButton *resetBtn = new QPushButton(tr("Reset All"), group);
    connect(resetBtn, &QPushButton::clicked, [detProgress, textProgress, animProgress]() {
        detProgress->setValue(0);
        textProgress->setValue(0);
        animProgress->setValue(0);
    });
    ctrlRow->addWidget(resetBtn);
    m_widgets.append(resetBtn);
    
    QPushButton *completeBtn = new QPushButton(tr("Complete All"), group);
    connect(completeBtn, &QPushButton::clicked, [detProgress, textProgress, animProgress]() {
        detProgress->setValue(100);
        textProgress->setValue(100);
        animProgress->setValue(100);
    });
    ctrlRow->addWidget(completeBtn);
    m_widgets.append(completeBtn);
    
    ctrlRow->addStretch();
    groupLayout->addLayout(ctrlRow);
}
