#include "ContainersPage.h"

#include <QGroupBox>
#include <QTabWidget>
#include <QStackedWidget>
#include <QScrollArea>
#include <QFrame>
#include <QToolBox>
#include <QTabBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>

ContainersPage::ContainersPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

ContainersPage::~ContainersPage()
{
    // Qt handles child widget deletion
}

void ContainersPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_widgets) {
        widget->setEnabled(enabled);
    }
}

void ContainersPage::setupWidgets()
{
    setupGroupBoxes();
    setupTabWidget();
    setupStackedWidget();
    setupScrollArea();
    setupFrames();
    setupToolBox();
    setupTabBar();
}

void ContainersPage::setupGroupBoxes()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Group Boxes")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(12);
    
    // Normal group box
    QGroupBox *normalGroup = new QGroupBox(tr("Normal Group"), group);
    QVBoxLayout *normalLayout = new QVBoxLayout(normalGroup);
    normalLayout->addWidget(new QLabel(tr("This is a normal group box"), normalGroup));
    normalLayout->addWidget(new QRadioButton(tr("Option A"), normalGroup));
    normalLayout->addWidget(new QRadioButton(tr("Option B"), normalGroup));
    row->addWidget(normalGroup);
    m_widgets.append(normalGroup);
    
    // Checkable group box
    QGroupBox *checkableGroup = new QGroupBox(tr("Checkable Group"), group);
    checkableGroup->setCheckable(true);
    checkableGroup->setChecked(true);
    QVBoxLayout *checkableLayout = new QVBoxLayout(checkableGroup);
    checkableLayout->addWidget(new QLabel(tr("This group can be toggled"), checkableGroup));
    checkableLayout->addWidget(new QCheckBox(tr("Enable feature"), checkableGroup));
    checkableLayout->addWidget(new QCheckBox(tr("Show details"), checkableGroup));
    row->addWidget(checkableGroup);
    m_widgets.append(checkableGroup);
    
    // Flat group box
    QGroupBox *flatGroup = new QGroupBox(tr("Flat Group"), group);
    flatGroup->setFlat(true);
    QVBoxLayout *flatLayout = new QVBoxLayout(flatGroup);
    flatLayout->addWidget(new QLabel(tr("This is a flat group box"), flatGroup));
    flatLayout->addWidget(new QLineEdit(flatGroup));
    row->addWidget(flatGroup);
    m_widgets.append(flatGroup);
    
    groupLayout->addLayout(row);
}

void ContainersPage::setupTabWidget()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Tab Widget")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QTabWidget *tabWidget = new QTabWidget(group);
    
    // Tab 1: General
    QWidget *tab1 = new QWidget();
    QVBoxLayout *tab1Layout = new QVBoxLayout(tab1);
    tab1Layout->addWidget(new QLabel(tr("General Settings"), tab1));
    tab1Layout->addWidget(new QCheckBox(tr("Enable notifications"), tab1));
    tab1Layout->addWidget(new QCheckBox(tr("Auto-save"), tab1));
    tab1Layout->addStretch();
    tabWidget->addTab(tab1, tr("General"));
    
    // Tab 2: Appearance
    QWidget *tab2 = new QWidget();
    QVBoxLayout *tab2Layout = new QVBoxLayout(tab2);
    tab2Layout->addWidget(new QLabel(tr("Appearance Settings"), tab2));
    QHBoxLayout *themeRow = new QHBoxLayout();
    themeRow->addWidget(new QLabel(tr("Theme:"), tab2));
    QComboBox *themeCombo = new QComboBox(tab2);
    themeCombo->addItems({tr("Light"), tr("Dark"), tr("System")});
    themeRow->addWidget(themeCombo);
    themeRow->addStretch();
    tab2Layout->addLayout(themeRow);
    tab2Layout->addStretch();
    tabWidget->addTab(tab2, tr("Appearance"));
    
    // Tab 3: Advanced
    QWidget *tab3 = new QWidget();
    QVBoxLayout *tab3Layout = new QVBoxLayout(tab3);
    tab3Layout->addWidget(new QLabel(tr("Advanced Settings"), tab3));
    tab3Layout->addWidget(new QLabel(tr("These settings are for advanced users."), tab3));
    tab3Layout->addStretch();
    tabWidget->addTab(tab3, tr("Advanced"));
    
    // Tab 4: Disabled tab
    QWidget *tab4 = new QWidget();
    QVBoxLayout *tab4Layout = new QVBoxLayout(tab4);
    tab4Layout->addWidget(new QLabel(tr("This tab is disabled"), tab4));
    tabWidget->addTab(tab4, tr("Disabled"));
    tabWidget->setTabEnabled(3, false);
    
    tabWidget->setMaximumHeight(200);
    groupLayout->addWidget(tabWidget);
    m_widgets.append(tabWidget);
}

void ContainersPage::setupStackedWidget()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Stacked Widget")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Navigation buttons
    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevBtn = new QPushButton(tr("< Previous"), group);
    QPushButton *nextBtn = new QPushButton(tr("Next >"), group);
    QLabel *pageLabel = new QLabel(tr("Page 1 of 3"), group);
    pageLabel->setAlignment(Qt::AlignCenter);
    navLayout->addWidget(prevBtn);
    navLayout->addWidget(pageLabel, 1);
    navLayout->addWidget(nextBtn);
    groupLayout->addLayout(navLayout);
    m_widgets.append(prevBtn);
    m_widgets.append(nextBtn);
    
    // Stacked widget
    QStackedWidget *stackedWidget = new QStackedWidget(group);
    
    // Page 1
    QWidget *page1 = new QWidget();
    QVBoxLayout *page1Layout = new QVBoxLayout(page1);
    page1Layout->addWidget(new QLabel(tr("Welcome to Page 1"), page1));
    page1Layout->addWidget(new QLabel(tr("This is the first page of the stacked widget."), page1));
    page1Layout->addStretch();
    stackedWidget->addWidget(page1);
    
    // Page 2
    QWidget *page2 = new QWidget();
    QVBoxLayout *page2Layout = new QVBoxLayout(page2);
    page2Layout->addWidget(new QLabel(tr("Welcome to Page 2"), page2));
    page2Layout->addWidget(new QLabel(tr("This is the second page."), page2));
    page2Layout->addWidget(new QLineEdit(page2));
    page2Layout->addStretch();
    stackedWidget->addWidget(page2);
    
    // Page 3
    QWidget *page3 = new QWidget();
    QVBoxLayout *page3Layout = new QVBoxLayout(page3);
    page3Layout->addWidget(new QLabel(tr("Welcome to Page 3"), page3));
    page3Layout->addWidget(new QLabel(tr("This is the final page."), page3));
    page3Layout->addWidget(new QPushButton(tr("Finish"), page3));
    page3Layout->addStretch();
    stackedWidget->addWidget(page3);
    
    stackedWidget->setMaximumHeight(150);
    groupLayout->addWidget(stackedWidget);
    m_widgets.append(stackedWidget);
    
    // Connect navigation
    connect(prevBtn, &QPushButton::clicked, [stackedWidget, pageLabel]() {
        int current = stackedWidget->currentIndex();
        if (current > 0) {
            stackedWidget->setCurrentIndex(current - 1);
            pageLabel->setText(tr("Page %1 of 3").arg(current));
        }
    });
    
    connect(nextBtn, &QPushButton::clicked, [stackedWidget, pageLabel]() {
        int current = stackedWidget->currentIndex();
        if (current < stackedWidget->count() - 1) {
            stackedWidget->setCurrentIndex(current + 1);
            pageLabel->setText(tr("Page %1 of 3").arg(current + 2));
        }
    });
}

void ContainersPage::setupScrollArea()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Scroll Area")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QScrollArea *scrollArea = new QScrollArea(group);
    scrollArea->setWidgetResizable(true);
    
    // Create content that's larger than the scroll area
    QWidget *content = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(8);
    
    contentLayout->addWidget(new QLabel(tr("Scroll Area Content"), content));
    contentLayout->addWidget(new QLabel(tr("This content is inside a scroll area."), content));
    
    for (int i = 1; i <= 10; ++i) {
        QHBoxLayout *itemRow = new QHBoxLayout();
        itemRow->addWidget(new QLabel(tr("Item %1:").arg(i), content));
        itemRow->addWidget(new QLineEdit(tr("Value %1").arg(i), content));
        itemRow->addStretch();
        contentLayout->addLayout(itemRow);
    }
    
    contentLayout->addStretch();
    
    scrollArea->setWidget(content);
    scrollArea->setMaximumHeight(200);
    groupLayout->addWidget(scrollArea);
    m_widgets.append(scrollArea);
}

void ContainersPage::setupFrames()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Frames")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(12);
    
    // Box frame
    QFrame *boxFrame = new QFrame(group);
    boxFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    boxFrame->setLineWidth(1);
    QVBoxLayout *boxLayout = new QVBoxLayout(boxFrame);
    boxLayout->addWidget(new QLabel(tr("Box Frame"), boxFrame));
    boxFrame->setMinimumSize(100, 60);
    row->addWidget(boxFrame);
    m_widgets.append(boxFrame);
    
    // Panel frame (raised)
    QFrame *raisedFrame = new QFrame(group);
    raisedFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    raisedFrame->setLineWidth(2);
    QVBoxLayout *raisedLayout = new QVBoxLayout(raisedFrame);
    raisedLayout->addWidget(new QLabel(tr("Raised Panel"), raisedFrame));
    raisedFrame->setMinimumSize(100, 60);
    row->addWidget(raisedFrame);
    m_widgets.append(raisedFrame);
    
    // Panel frame (sunken)
    QFrame *sunkenFrame = new QFrame(group);
    sunkenFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    sunkenFrame->setLineWidth(2);
    QVBoxLayout *sunkenLayout = new QVBoxLayout(sunkenFrame);
    sunkenLayout->addWidget(new QLabel(tr("Sunken Panel"), sunkenFrame));
    sunkenFrame->setMinimumSize(100, 60);
    row->addWidget(sunkenFrame);
    m_widgets.append(sunkenFrame);
    
    // Styled panel
    QFrame *styledFrame = new QFrame(group);
    styledFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    QVBoxLayout *styledLayout = new QVBoxLayout(styledFrame);
    styledLayout->addWidget(new QLabel(tr("Styled Panel"), styledFrame));
    styledFrame->setMinimumSize(100, 60);
    row->addWidget(styledFrame);
    m_widgets.append(styledFrame);
    
    row->addStretch();
    groupLayout->addLayout(row);
    
    // Horizontal line
    QHBoxLayout *lineRow = new QHBoxLayout();
    QFrame *hLine = new QFrame(group);
    hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    lineRow->addWidget(new QLabel(tr("Horizontal Line:"), group));
    lineRow->addWidget(hLine, 1);
    groupLayout->addLayout(lineRow);
    m_widgets.append(hLine);
}

void ContainersPage::setupToolBox()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Tool Box")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QToolBox *toolBox = new QToolBox(group);
    
    // Section 1: General Settings
    QWidget *generalPage = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalPage);
    generalLayout->addWidget(new QLabel(tr("General Settings"), generalPage));
    generalLayout->addWidget(new QCheckBox(tr("Enable feature A"), generalPage));
    generalLayout->addWidget(new QCheckBox(tr("Enable feature B"), generalPage));
    generalLayout->addStretch();
    toolBox->addItem(generalPage, tr("General"));
    
    // Section 2: Appearance
    QWidget *appearancePage = new QWidget();
    QVBoxLayout *appearanceLayout = new QVBoxLayout(appearancePage);
    appearanceLayout->addWidget(new QLabel(tr("Appearance Settings"), appearancePage));
    QHBoxLayout *themeRow = new QHBoxLayout();
    themeRow->addWidget(new QLabel(tr("Theme:"), appearancePage));
    QComboBox *themeCombo = new QComboBox(appearancePage);
    themeCombo->addItems({tr("Light"), tr("Dark"), tr("System")});
    themeRow->addWidget(themeCombo);
    themeRow->addStretch();
    appearanceLayout->addLayout(themeRow);
    appearanceLayout->addStretch();
    toolBox->addItem(appearancePage, tr("Appearance"));
    
    // Section 3: Advanced
    QWidget *advancedPage = new QWidget();
    QVBoxLayout *advancedLayout = new QVBoxLayout(advancedPage);
    advancedLayout->addWidget(new QLabel(tr("Advanced Settings"), advancedPage));
    advancedLayout->addWidget(new QLabel(tr("These settings are for advanced users."), advancedPage));
    advancedLayout->addWidget(new QLineEdit(advancedPage));
    advancedLayout->addStretch();
    toolBox->addItem(advancedPage, tr("Advanced"));
    
    toolBox->setMaximumHeight(250);
    groupLayout->addWidget(toolBox);
    m_widgets.append(toolBox);
}

void ContainersPage::setupTabBar()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Tab Bar (Standalone)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    groupLayout->addWidget(new QLabel(tr("Standalone QTabBar for styling tabs without QTabWidget:"), group));
    
    // Horizontal tab bar (top position style)
    QTabBar *topTabBar = new QTabBar(group);
    topTabBar->addTab(tr("Home"));
    topTabBar->addTab(tr("Documents"));
    topTabBar->addTab(tr("Settings"));
    topTabBar->addTab(tr("Help"));
    topTabBar->setTabEnabled(3, false);  // Disabled tab for styling
    groupLayout->addWidget(topTabBar);
    m_widgets.append(topTabBar);
    
    // Tab bar with closable tabs
    QTabBar *closableTabBar = new QTabBar(group);
    closableTabBar->setTabsClosable(true);
    closableTabBar->addTab(tr("File 1"));
    closableTabBar->addTab(tr("File 2"));
    closableTabBar->addTab(tr("File 3"));
    groupLayout->addWidget(closableTabBar);
    m_widgets.append(closableTabBar);
    
    // Tab bar with movable tabs
    QTabBar *movableTabBar = new QTabBar(group);
    movableTabBar->setMovable(true);
    movableTabBar->addTab(tr("Drag Me"));
    movableTabBar->addTab(tr("Reorder"));
    movableTabBar->addTab(tr("Tabs"));
    groupLayout->addWidget(movableTabBar);
    m_widgets.append(movableTabBar);
}
