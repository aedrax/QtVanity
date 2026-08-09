#include "WidgetGallery.h"

#include "GalleryPage.h"
#include "ButtonsPage.h"
#include "InputsPage.h"
#include "ViewsPage.h"
#include "ContainersPage.h"
#include "DialogsPage.h"
#include "DisplayPage.h"
#include "MainWindowPage.h"
#include "AdvancedPage.h"
#include "CustomWidgetsPage.h"
#include "PluginManager.h"

#include <QApplication>
#include <QTabWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QAbstractSpinBox>
#include <QAbstractButton>
#include <QSpinBox>
#include <functional>

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
    , m_customWidgetsPage(nullptr)
    , m_pluginManager(nullptr)
    , m_filterEdit(nullptr)
    , m_rightToLeftCheckBox(nullptr)
    , m_fontScaleSpin(nullptr)
    , m_checkedCheckBox(nullptr)
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
    // Named so that callers - and tests - can tell the gallery's own controls
    // apart from the specimens on display, structurally rather than by
    // matching against their label text.
    controlsGroup->setObjectName(QStringLiteral("GalleryControls"));
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

    // Pinned :checked. Of the pseudo-states people struggle to style, this is
    // one of the few that can be held; :hover and :pressed cannot be pinned on
    // live widgets without injecting events.
    m_checkedCheckBox = new QCheckBox(tr("Checkables Checked"), controlsGroup);
    m_checkedCheckBox->setChecked(false);
    m_checkedCheckBox->setToolTip(tr("Check every checkable widget, to inspect :checked"));
    connect(m_checkedCheckBox, &QCheckBox::toggled,
            this, &WidgetGallery::setCheckablesChecked);

    // Layout direction changes which side padding and sub-controls land on.
    m_rightToLeftCheckBox = new QCheckBox(tr("Right-to-Left"), controlsGroup);
    m_rightToLeftCheckBox->setChecked(false);
    m_rightToLeftCheckBox->setToolTip(tr("Mirror the gallery to check RTL layouts"));
    connect(m_rightToLeftCheckBox, &QCheckBox::toggled,
            this, &WidgetGallery::setRightToLeft);

    QLabel *scaleLabel = new QLabel(tr("Font:"), controlsGroup);
    m_fontScaleSpin = new QSpinBox(controlsGroup);
    m_fontScaleSpin->setRange(50, 250);
    m_fontScaleSpin->setSingleStep(10);
    m_fontScaleSpin->setValue(100);
    m_fontScaleSpin->setSuffix(tr("%"));
    m_fontScaleSpin->setToolTip(tr("Scale gallery fonts, to check layouts at other sizes"));
    m_fontScaleSpin->setAccessibleName(tr("Gallery font scale"));
    connect(m_fontScaleSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &WidgetGallery::setFontScale);
    scaleLabel->setBuddy(m_fontScaleSpin);

    controlsLayout->addWidget(m_enabledCheckBox);
    controlsLayout->addWidget(m_readOnlyCheckBox);
    controlsLayout->addWidget(m_checkedCheckBox);
    controlsLayout->addWidget(m_rightToLeftCheckBox);
    controlsLayout->addWidget(scaleLabel);
    controlsLayout->addWidget(m_fontScaleSpin);
    controlsLayout->addStretch();

    // Add to main layout (will be called before m_tabWidget is added)
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->addWidget(controlsGroup);
    }

    setupFilterRow();
}

void WidgetGallery::setupFilterRow()
{
    // Nine tabs and several hundred widgets, with no way to find one.
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setObjectName(QStringLiteral("GalleryControls_Filter"));
    m_filterEdit->setPlaceholderText(tr("Filter widgets across all tabs..."));
    m_filterEdit->setClearButtonEnabled(true);
    m_filterEdit->setAccessibleName(tr("Widget filter"));
    connect(m_filterEdit, &QLineEdit::textChanged, this, &WidgetGallery::setFilter);

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->addWidget(m_filterEdit);
    }
}

void WidgetGallery::setFilter(const QString &text)
{
    const QList<GalleryPage*> pages = {
        m_buttonsPage, m_inputsPage, m_viewsPage, m_containersPage, m_dialogsPage,
        m_displayPage, m_mainWindowPage, m_advancedPage, m_customWidgetsPage
    };

    for (int i = 0; i < pages.size(); ++i) {
        GalleryPage *page = pages.at(i);
        if (!page) {
            continue;
        }
        const int visible = page->applyFilter(text);

        // Grey out the tab of a page with nothing left to show, so the user
        // is not left clicking through empty tabs.
        const int tabIndex = m_tabWidget->indexOf(page);
        if (tabIndex >= 0) {
            m_tabWidget->setTabEnabled(tabIndex, visible > 0 || text.trimmed().isEmpty());
        }
    }
}

bool WidgetGallery::isGalleryControl(const QWidget *widget)
{
    for (const QObject *o = widget; o; o = o->parent()) {
        if (o->objectName().startsWith(QLatin1String("GalleryControls"))) {
            return true;
        }
    }
    return false;
}

void WidgetGallery::setRightToLeft(bool rightToLeft)
{
    m_tabWidget->setLayoutDirection(rightToLeft ? Qt::RightToLeft : Qt::LeftToRight);

    if (m_rightToLeftCheckBox && m_rightToLeftCheckBox->isChecked() != rightToLeft) {
        m_rightToLeftCheckBox->blockSignals(true);
        m_rightToLeftCheckBox->setChecked(rightToLeft);
        m_rightToLeftCheckBox->blockSignals(false);
    }
}

void WidgetGallery::setFontScale(int percent)
{
    QFont scaled = QApplication::font();
    scaled.setPointSizeF(QApplication::font().pointSizeF() * percent / 100.0);
    m_tabWidget->setFont(scaled);

    // The pages set no font of their own, so clearing theirs lets the tab
    // widget's font propagate down.
    for (QWidget *child : m_tabWidget->findChildren<QWidget*>()) {
        child->setFont(scaled);
    }

    if (m_fontScaleSpin && m_fontScaleSpin->value() != percent) {
        m_fontScaleSpin->blockSignals(true);
        m_fontScaleSpin->setValue(percent);
        m_fontScaleSpin->blockSignals(false);
    }
}

void WidgetGallery::setCheckablesChecked(bool checked)
{
    for (QAbstractButton *button : m_tabWidget->findChildren<QAbstractButton*>()) {
        if (button->isCheckable()) {
            button->setChecked(checked);
        }
    }
    for (QGroupBox *group : m_tabWidget->findChildren<QGroupBox*>()) {
        if (group->isCheckable()) {
            group->setChecked(checked);
        }
    }

    if (m_checkedCheckBox && m_checkedCheckBox->isChecked() != checked) {
        m_checkedCheckBox->blockSignals(true);
        m_checkedCheckBox->setChecked(checked);
        m_checkedCheckBox->blockSignals(false);
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
    if (m_customWidgetsPage) {
        m_customWidgetsPage->setWidgetsEnabled(enabled);
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
    // Some demo widgets are read-only on purpose - the "Read-only" specimens
    // in Inputs, and the document view in the Main Window page. Blanket
    // clearing used to make them writable and they never came back, so record
    // each widget's original state the first time and restore that.
    auto applyReadOnly = [this, readOnly](QWidget *widget, bool current,
                                          const std::function<void(bool)> &setter) {
        if (!m_originalReadOnly.contains(widget)) {
            m_originalReadOnly.insert(widget, current);
        }
        setter(readOnly ? true : m_originalReadOnly.value(widget));
    };

    for (QLineEdit *w : findChildren<QLineEdit*>()) {
        applyReadOnly(w, w->isReadOnly(), [w](bool ro) { w->setReadOnly(ro); });
    }
    for (QTextEdit *w : findChildren<QTextEdit*>()) {
        applyReadOnly(w, w->isReadOnly(), [w](bool ro) { w->setReadOnly(ro); });
    }
    // Previously missed entirely:
    for (QPlainTextEdit *w : findChildren<QPlainTextEdit*>()) {
        applyReadOnly(w, w->isReadOnly(), [w](bool ro) { w->setReadOnly(ro); });
    }
    for (QAbstractSpinBox *w : findChildren<QAbstractSpinBox*>()) {
        applyReadOnly(w, w->isReadOnly(), [w](bool ro) { w->setReadOnly(ro); });
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

void WidgetGallery::setPluginManager(PluginManager *pluginManager)
{
    if (m_pluginManager == pluginManager) {
        return;
    }

    if (m_pluginManager) {
        disconnect(m_pluginManager, nullptr, m_customWidgetsPage, nullptr);
    }

    m_pluginManager = pluginManager;

    if (!m_pluginManager) {
        return;
    }

    // Create the page once; a second call must re-target the existing tab
    // rather than adding a duplicate one and leaking the first.
    if (!m_customWidgetsPage) {
        m_customWidgetsPage = new CustomWidgetsPage(m_pluginManager, m_tabWidget);
        m_tabWidget->addTab(m_customWidgetsPage, tr("Custom Widgets"));
    } else {
        m_customWidgetsPage->setPluginManager(m_pluginManager);
    }

    // Connect pluginsLoaded signal to rebuildWidgets for automatic updates
    connect(m_pluginManager, &PluginManager::pluginsLoaded,
            m_customWidgetsPage, &CustomWidgetsPage::rebuildWidgets);

    // Destroy plugin-created widgets before their libraries are unmapped.
    connect(m_pluginManager, &PluginManager::pluginsAboutToUnload,
            m_customWidgetsPage, &CustomWidgetsPage::releaseWidgets);
}
