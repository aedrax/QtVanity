#include "ButtonsPage.h"

#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QCommandLinkButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QButtonGroup>
#include <QStyle>

ButtonsPage::ButtonsPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

ButtonsPage::~ButtonsPage()
{
    // Qt handles child widget deletion
}

void ButtonsPage::setWidgetsEnabled(bool enabled)
{
    for (QAbstractButton *button : m_buttons) {
        button->setEnabled(enabled);
    }
}

void ButtonsPage::setupWidgets()
{
    setupPushButtons();
    setupButtonVariants();
    setupToolButtons();
    setupRadioButtons();
    setupCheckBoxes();
    setupSwitches();
    setupCommandLinkButtons();
}

void ButtonsPage::setupPushButtons()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Push Buttons")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Normal buttons
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(8);
    
    QPushButton *normalBtn = new QPushButton(tr("Normal"), group);
    row1->addWidget(normalBtn);
    m_buttons.append(normalBtn);
    
    QPushButton *iconBtn = new QPushButton(tr("With Icon"), group);
    iconBtn->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
    row1->addWidget(iconBtn);
    m_buttons.append(iconBtn);
    
    QPushButton *defaultBtn = new QPushButton(tr("Default"), group);
    defaultBtn->setDefault(true);
    row1->addWidget(defaultBtn);
    m_buttons.append(defaultBtn);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
    
    // Row 2: Flat and checkable buttons
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(8);
    
    QPushButton *flatBtn = new QPushButton(tr("Flat"), group);
    flatBtn->setFlat(true);
    row2->addWidget(flatBtn);
    m_buttons.append(flatBtn);
    
    QPushButton *checkableBtn = new QPushButton(tr("Checkable"), group);
    checkableBtn->setCheckable(true);
    row2->addWidget(checkableBtn);
    m_buttons.append(checkableBtn);
    
    QPushButton *checkedBtn = new QPushButton(tr("Checked"), group);
    checkedBtn->setCheckable(true);
    checkedBtn->setChecked(true);
    row2->addWidget(checkedBtn);
    m_buttons.append(checkedBtn);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
    
    // Row 3: Button with menu
    QHBoxLayout *row3 = new QHBoxLayout();
    row3->setSpacing(8);
    
    QPushButton *menuBtn = new QPushButton(tr("With Menu"), group);
    QMenu *menu = new QMenu(menuBtn);
    menu->addAction(tr("Action 1"));
    menu->addAction(tr("Action 2"));
    menu->addSeparator();
    menu->addAction(tr("Action 3"));
    menuBtn->setMenu(menu);
    row3->addWidget(menuBtn);
    m_buttons.append(menuBtn);
    
    row3->addStretch();
    groupLayout->addLayout(row3);
}

void ButtonsPage::setupButtonVariants()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Button Variants (shadcn)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Primary variants
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(8);
    
    QPushButton *defaultBtn = new QPushButton(tr("Default"), group);
    row1->addWidget(defaultBtn);
    m_buttons.append(defaultBtn);
    
    QPushButton *secondaryBtn = new QPushButton(tr("Secondary"), group);
    secondaryBtn->setProperty("variant", "secondary");
    row1->addWidget(secondaryBtn);
    m_buttons.append(secondaryBtn);
    
    QPushButton *destructiveBtn = new QPushButton(tr("Destructive"), group);
    destructiveBtn->setProperty("variant", "destructive");
    row1->addWidget(destructiveBtn);
    m_buttons.append(destructiveBtn);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
    
    // Row 2: Outline, Ghost, Link variants
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(8);
    
    QPushButton *outlineBtn = new QPushButton(tr("Outline"), group);
    outlineBtn->setProperty("variant", "outline");
    row2->addWidget(outlineBtn);
    m_buttons.append(outlineBtn);
    
    QPushButton *ghostBtn = new QPushButton(tr("Ghost"), group);
    ghostBtn->setProperty("variant", "ghost");
    row2->addWidget(ghostBtn);
    m_buttons.append(ghostBtn);
    
    QPushButton *linkBtn = new QPushButton(tr("Link"), group);
    linkBtn->setProperty("variant", "link");
    row2->addWidget(linkBtn);
    m_buttons.append(linkBtn);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
    
    // Row 3: Variants with icons
    QHBoxLayout *row3 = new QHBoxLayout();
    row3->setSpacing(8);
    
    QPushButton *secondaryIconBtn = new QPushButton(tr("Secondary"), group);
    secondaryIconBtn->setProperty("variant", "secondary");
    secondaryIconBtn->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    row3->addWidget(secondaryIconBtn);
    m_buttons.append(secondaryIconBtn);
    
    QPushButton *destructiveIconBtn = new QPushButton(tr("Delete"), group);
    destructiveIconBtn->setProperty("variant", "destructive");
    destructiveIconBtn->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    row3->addWidget(destructiveIconBtn);
    m_buttons.append(destructiveIconBtn);
    
    QPushButton *outlineIconBtn = new QPushButton(tr("Settings"), group);
    outlineIconBtn->setProperty("variant", "outline");
    outlineIconBtn->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    row3->addWidget(outlineIconBtn);
    m_buttons.append(outlineIconBtn);
    
    row3->addStretch();
    groupLayout->addLayout(row3);
}

void ButtonsPage::setupToolButtons()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Tool Buttons")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Normal tool buttons
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(8);
    
    QToolButton *normalTool = new QToolButton(group);
    normalTool->setText(tr("Normal"));
    normalTool->setToolButtonStyle(Qt::ToolButtonTextOnly);
    row1->addWidget(normalTool);
    m_buttons.append(normalTool);
    
    QToolButton *iconTool = new QToolButton(group);
    iconTool->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    iconTool->setToolTip(tr("Icon Only"));
    row1->addWidget(iconTool);
    m_buttons.append(iconTool);
    
    QToolButton *textIconTool = new QToolButton(group);
    textIconTool->setText(tr("Text+Icon"));
    textIconTool->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    textIconTool->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    row1->addWidget(textIconTool);
    m_buttons.append(textIconTool);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
    
    // Row 2: Checkable and with menu
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(8);
    
    QToolButton *checkableTool = new QToolButton(group);
    checkableTool->setText(tr("Checkable"));
    checkableTool->setToolButtonStyle(Qt::ToolButtonTextOnly);
    checkableTool->setCheckable(true);
    row2->addWidget(checkableTool);
    m_buttons.append(checkableTool);
    
    QToolButton *checkedTool = new QToolButton(group);
    checkedTool->setText(tr("Checked"));
    checkedTool->setToolButtonStyle(Qt::ToolButtonTextOnly);
    checkedTool->setCheckable(true);
    checkedTool->setChecked(true);
    row2->addWidget(checkedTool);
    m_buttons.append(checkedTool);
    
    QToolButton *menuTool = new QToolButton(group);
    menuTool->setText(tr("Menu"));
    menuTool->setToolButtonStyle(Qt::ToolButtonTextOnly);
    menuTool->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *toolMenu = new QMenu(menuTool);
    toolMenu->addAction(tr("Option A"));
    toolMenu->addAction(tr("Option B"));
    menuTool->setMenu(toolMenu);
    row2->addWidget(menuTool);
    m_buttons.append(menuTool);
    
    QToolButton *instantMenuTool = new QToolButton(group);
    instantMenuTool->setText(tr("Instant Menu"));
    instantMenuTool->setToolButtonStyle(Qt::ToolButtonTextOnly);
    instantMenuTool->setPopupMode(QToolButton::InstantPopup);
    QMenu *instantMenu = new QMenu(instantMenuTool);
    instantMenu->addAction(tr("Choice 1"));
    instantMenu->addAction(tr("Choice 2"));
    instantMenuTool->setMenu(instantMenu);
    row2->addWidget(instantMenuTool);
    m_buttons.append(instantMenuTool);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
}

void ButtonsPage::setupRadioButtons()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Radio Buttons")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Create a button group for exclusive selection
    QButtonGroup *buttonGroup = new QButtonGroup(group);
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(16);
    
    QRadioButton *radio1 = new QRadioButton(tr("Option 1"), group);
    radio1->setChecked(true);
    buttonGroup->addButton(radio1);
    row->addWidget(radio1);
    m_buttons.append(radio1);
    
    QRadioButton *radio2 = new QRadioButton(tr("Option 2"), group);
    buttonGroup->addButton(radio2);
    row->addWidget(radio2);
    m_buttons.append(radio2);
    
    QRadioButton *radio3 = new QRadioButton(tr("Option 3"), group);
    buttonGroup->addButton(radio3);
    row->addWidget(radio3);
    m_buttons.append(radio3);
    
    row->addStretch();
    groupLayout->addLayout(row);
    
    // Second row with another group
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(16);
    
    QButtonGroup *buttonGroup2 = new QButtonGroup(group);
    
    QRadioButton *radioA = new QRadioButton(tr("Choice A"), group);
    buttonGroup2->addButton(radioA);
    row2->addWidget(radioA);
    m_buttons.append(radioA);
    
    QRadioButton *radioB = new QRadioButton(tr("Choice B (selected)"), group);
    radioB->setChecked(true);
    buttonGroup2->addButton(radioB);
    row2->addWidget(radioB);
    m_buttons.append(radioB);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
}

void ButtonsPage::setupCheckBoxes()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Check Boxes")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Basic checkboxes
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(16);
    
    QCheckBox *unchecked = new QCheckBox(tr("Unchecked"), group);
    row1->addWidget(unchecked);
    m_buttons.append(unchecked);
    
    QCheckBox *checked = new QCheckBox(tr("Checked"), group);
    checked->setChecked(true);
    row1->addWidget(checked);
    m_buttons.append(checked);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
    
    // Row 2: Tristate checkbox
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(16);
    
    QCheckBox *tristate = new QCheckBox(tr("Tristate (partial)"), group);
    tristate->setTristate(true);
    tristate->setCheckState(Qt::PartiallyChecked);
    row2->addWidget(tristate);
    m_buttons.append(tristate);
    
    QCheckBox *tristateUnchecked = new QCheckBox(tr("Tristate (unchecked)"), group);
    tristateUnchecked->setTristate(true);
    tristateUnchecked->setCheckState(Qt::Unchecked);
    row2->addWidget(tristateUnchecked);
    m_buttons.append(tristateUnchecked);
    
    QCheckBox *tristateChecked = new QCheckBox(tr("Tristate (checked)"), group);
    tristateChecked->setTristate(true);
    tristateChecked->setCheckState(Qt::Checked);
    row2->addWidget(tristateChecked);
    m_buttons.append(tristateChecked);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
}

void ButtonsPage::setupSwitches()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Switches (shadcn)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Switch-style checkboxes
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(16);
    
    QCheckBox *switchOff = new QCheckBox(tr("Airplane Mode"), group);
    switchOff->setProperty("switch", "true");
    row1->addWidget(switchOff);
    m_buttons.append(switchOff);
    
    QCheckBox *switchOn = new QCheckBox(tr("Wi-Fi"), group);
    switchOn->setProperty("switch", "true");
    switchOn->setChecked(true);
    row1->addWidget(switchOn);
    m_buttons.append(switchOn);
    
    QCheckBox *switchDisabled = new QCheckBox(tr("Bluetooth (disabled)"), group);
    switchDisabled->setProperty("switch", "true");
    switchDisabled->setEnabled(false);
    row1->addWidget(switchDisabled);
    m_buttons.append(switchDisabled);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
}

void ButtonsPage::setupCommandLinkButtons()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Command Link Buttons")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Normal command link button
    QCommandLinkButton *cmdLink1 = new QCommandLinkButton(tr("Open Settings"), group);
    cmdLink1->setDescription(tr("Configure application preferences and options"));
    groupLayout->addWidget(cmdLink1);
    m_buttons.append(cmdLink1);
    
    // Command link button with icon
    QCommandLinkButton *cmdLink2 = new QCommandLinkButton(tr("Browse Files"), group);
    cmdLink2->setDescription(tr("Open file browser to select documents"));
    cmdLink2->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    groupLayout->addWidget(cmdLink2);
    m_buttons.append(cmdLink2);
    
    // Checkable command link button
    QCommandLinkButton *cmdLink3 = new QCommandLinkButton(tr("Enable Feature"), group);
    cmdLink3->setDescription(tr("Toggle this feature on or off"));
    cmdLink3->setCheckable(true);
    groupLayout->addWidget(cmdLink3);
    m_buttons.append(cmdLink3);
}
