#include "WidgetGallery.h"

#include "ButtonsPage.h"
#include "InputsPage.h"
#include "ViewsPage.h"
#include "ContainersPage.h"
#include "DialogsPage.h"
#include "DisplayPage.h"
#include "MainWindowPage.h"
#include "AdvancedPage.h"

#include <QTabWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

WidgetGallery::WidgetGallery(QWidget *parent)
    : QWidget(parent)
    , m_tabWidget(nullptr)
    , m_enabledCheckBox(nullptr)
    , m_readOnlyCheckBox(nullptr)
    , m_buttonsPage(nullptr)
    , m_inputsPage(nullptr)
    , m_viewsPage(nullptr)
    , m_containersPage(nullptr)
    , m_dialogsPage(nullptr)
    , m_displayPage(nullptr)
    , m_mainWindowPage(nullptr)
    , m_advancedPage(nullptr)
{
    setupUi();
}

WidgetGallery::~WidgetGallery()
{
    // Qt handles child widget deletion
}

void WidgetGallery::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(8);

    // Setup toggle controls at the top
    setupToggleControls();

    // Create tab widget for gallery pages
    m_tabWidget = new QTabWidget(this);
    
    // Setup all gallery pages
    setupPages();

    mainLayout->addWidget(m_tabWidget);
}

void WidgetGallery::setupPages()
{
    // Create all gallery pages
    m_buttonsPage = new ButtonsPage(m_tabWidget);
    m_inputsPage = new InputsPage(m_tabWidget);
    m_viewsPage = new ViewsPage(m_tabWidget);
    m_containersPage = new ContainersPage(m_tabWidget);
    m_dialogsPage = new DialogsPage(m_tabWidget);
    m_displayPage = new DisplayPage(m_tabWidget);
    m_mainWindowPage = new MainWindowPage(m_tabWidget);
    m_advancedPage = new AdvancedPage(m_tabWidget);

    // Add pages as tabs
    m_tabWidget->addTab(m_buttonsPage, tr("Buttons"));
    m_tabWidget->addTab(m_inputsPage, tr("Inputs"));
    m_tabWidget->addTab(m_viewsPage, tr("Views"));
    m_tabWidget->addTab(m_containersPage, tr("Containers"));
    m_tabWidget->addTab(m_dialogsPage, tr("Dialogs"));
    m_tabWidget->addTab(m_displayPage, tr("Display"));
    m_tabWidget->addTab(m_mainWindowPage, tr("Main Window"));
    m_tabWidget->addTab(m_advancedPage, tr("Advanced"));
}

void WidgetGallery::setupToggleControls()
{
    QGroupBox *controlsGroup = new QGroupBox(tr("Widget State Controls"), this);
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsGroup);
    controlsLayout->setContentsMargins(8, 8, 8, 8);
    controlsLayout->setSpacing(16);

    // Enabled/Disabled toggle
    m_enabledCheckBox = new QCheckBox(tr("Widgets Enabled"), controlsGroup);
    m_enabledCheckBox->setChecked(true);
    m_enabledCheckBox->setToolTip(tr("Toggle enabled/disabled state for all gallery widgets"));
    connect(m_enabledCheckBox, &QCheckBox::toggled, this, &WidgetGallery::onEnabledToggled);

    // Read-only toggle
    m_readOnlyCheckBox = new QCheckBox(tr("Inputs Read-Only"), controlsGroup);
    m_readOnlyCheckBox->setChecked(false);
    m_readOnlyCheckBox->setToolTip(tr("Toggle read-only state for input widgets"));
    connect(m_readOnlyCheckBox, &QCheckBox::toggled, this, &WidgetGallery::onReadOnlyToggled);

    controlsLayout->addWidget(m_enabledCheckBox);
    controlsLayout->addWidget(m_readOnlyCheckBox);
    controlsLayout->addStretch();

    // Add to main layout (will be called before m_tabWidget is added)
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->addWidget(controlsGroup);
    }
}

void WidgetGallery::setWidgetsEnabled(bool enabled)
{
    // Propagate to all gallery pages
    if (m_buttonsPage) {
        m_buttonsPage->setWidgetsEnabled(enabled);
    }
    if (m_inputsPage) {
        m_inputsPage->setWidgetsEnabled(enabled);
    }
    if (m_viewsPage) {
        m_viewsPage->setWidgetsEnabled(enabled);
    }
    if (m_containersPage) {
        m_containersPage->setWidgetsEnabled(enabled);
    }
    if (m_dialogsPage) {
        m_dialogsPage->setWidgetsEnabled(enabled);
    }
    if (m_displayPage) {
        m_displayPage->setWidgetsEnabled(enabled);
    }
    if (m_mainWindowPage) {
        m_mainWindowPage->setWidgetsEnabled(enabled);
    }
    if (m_advancedPage) {
        m_advancedPage->setWidgetsEnabled(enabled);
    }

    // Update checkbox state if called programmatically
    if (m_enabledCheckBox && m_enabledCheckBox->isChecked() != enabled) {
        m_enabledCheckBox->blockSignals(true);
        m_enabledCheckBox->setChecked(enabled);
        m_enabledCheckBox->blockSignals(false);
    }

    emit widgetsEnabledChanged(enabled);
}

void WidgetGallery::setInputsReadOnly(bool readOnly)
{
    // Set read-only state on all QLineEdit widgets in the gallery
    QList<QLineEdit*> lineEdits = findChildren<QLineEdit*>();
    for (QLineEdit *lineEdit : lineEdits) {
        lineEdit->setReadOnly(readOnly);
    }
    
    // Set read-only state on all QTextEdit widgets in the gallery
    QList<QTextEdit*> textEdits = findChildren<QTextEdit*>();
    for (QTextEdit *textEdit : textEdits) {
        textEdit->setReadOnly(readOnly);
    }

    // Update checkbox state if called programmatically
    if (m_readOnlyCheckBox && m_readOnlyCheckBox->isChecked() != readOnly) {
        m_readOnlyCheckBox->blockSignals(true);
        m_readOnlyCheckBox->setChecked(readOnly);
        m_readOnlyCheckBox->blockSignals(false);
    }

    emit inputsReadOnlyChanged(readOnly);
}

void WidgetGallery::onEnabledToggled(bool checked)
{
    setWidgetsEnabled(checked);
}

void WidgetGallery::onReadOnlyToggled(bool checked)
{
    setInputsReadOnly(checked);
}
