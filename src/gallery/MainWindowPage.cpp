#include "MainWindowPage.h"

#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>
#include <QSplitter>
#include <QAction>
#include <QToolButton>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QListWidget>
#include <QMenu>
#include <QStyle>
#include <QFrame>

MainWindowPage::MainWindowPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

MainWindowPage::~MainWindowPage()
{
    // Qt handles child widget deletion
}

void MainWindowPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_widgets) {
        widget->setEnabled(enabled);
    }
}

void MainWindowPage::setupWidgets()
{
    setupToolBar();
    setupStatusBar();
    setupMenuBar();
    setupDockWidget();
    setupSplitter();
}

void MainWindowPage::setupToolBar()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Toolbar")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Create a toolbar with various actions and styles
    QToolBar *toolbar = new QToolBar(tr("Sample Toolbar"), group);
    toolbar->setMovable(false);
    
    // Add actions with icons
    QAction *newAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_FileIcon), tr("New"));
    newAction->setToolTip(tr("Create new document"));
    
    QAction *openAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogOpenButton), tr("Open"));
    openAction->setToolTip(tr("Open existing document"));
    
    QAction *saveAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save"));
    saveAction->setToolTip(tr("Save document"));
    
    toolbar->addSeparator();
    
    QAction *cutAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("Cut"));
    cutAction->setToolTip(tr("Cut selection"));
    
    QAction *copyAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Copy"));
    copyAction->setToolTip(tr("Copy selection"));
    
    toolbar->addSeparator();
    
    // Add a checkable action
    QAction *toggleAction = toolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogHelpButton), tr("Toggle"));
    toggleAction->setCheckable(true);
    toggleAction->setChecked(true);
    toggleAction->setToolTip(tr("Checkable action"));
    
    groupLayout->addWidget(toolbar);
    m_widgets.append(toolbar);
    
    // Row 2: Different tool button styles
    QHBoxLayout *stylesRow = new QHBoxLayout();
    stylesRow->setSpacing(16);
    
    // Icon only toolbar
    QVBoxLayout *iconOnlyLayout = new QVBoxLayout();
    QLabel *iconOnlyLabel = new QLabel(tr("Icon Only"), group);
    QToolBar *iconOnlyToolbar = new QToolBar(group);
    iconOnlyToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    iconOnlyToolbar->setMovable(false);
    iconOnlyToolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), tr("Back"));
    iconOnlyToolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), tr("Forward"));
    iconOnlyToolbar->addAction(style()->standardIcon(QStyle::SP_ArrowUp), tr("Up"));
    iconOnlyLayout->addWidget(iconOnlyLabel);
    iconOnlyLayout->addWidget(iconOnlyToolbar);
    stylesRow->addLayout(iconOnlyLayout);
    m_widgets.append(iconOnlyLabel);
    m_widgets.append(iconOnlyToolbar);
    
    // Text only toolbar
    QVBoxLayout *textOnlyLayout = new QVBoxLayout();
    QLabel *textOnlyLabel = new QLabel(tr("Text Only"), group);
    QToolBar *textOnlyToolbar = new QToolBar(group);
    textOnlyToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    textOnlyToolbar->setMovable(false);
    textOnlyToolbar->addAction(tr("Back"));
    textOnlyToolbar->addAction(tr("Forward"));
    textOnlyToolbar->addAction(tr("Up"));
    textOnlyLayout->addWidget(textOnlyLabel);
    textOnlyLayout->addWidget(textOnlyToolbar);
    stylesRow->addLayout(textOnlyLayout);
    m_widgets.append(textOnlyLabel);
    m_widgets.append(textOnlyToolbar);
    
    // Text beside icon toolbar
    QVBoxLayout *textBesideLayout = new QVBoxLayout();
    QLabel *textBesideLabel = new QLabel(tr("Text Beside Icon"), group);
    QToolBar *textBesideToolbar = new QToolBar(group);
    textBesideToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    textBesideToolbar->setMovable(false);
    textBesideToolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), tr("Back"));
    textBesideToolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), tr("Forward"));
    textBesideLayout->addWidget(textBesideLabel);
    textBesideLayout->addWidget(textBesideToolbar);
    stylesRow->addLayout(textBesideLayout);
    m_widgets.append(textBesideLabel);
    m_widgets.append(textBesideToolbar);
    
    // Text under icon toolbar
    QVBoxLayout *textUnderLayout = new QVBoxLayout();
    QLabel *textUnderLabel = new QLabel(tr("Text Under Icon"), group);
    QToolBar *textUnderToolbar = new QToolBar(group);
    textUnderToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    textUnderToolbar->setMovable(false);
    textUnderToolbar->addAction(style()->standardIcon(QStyle::SP_ArrowBack), tr("Back"));
    textUnderToolbar->addAction(style()->standardIcon(QStyle::SP_ArrowForward), tr("Forward"));
    textUnderLayout->addWidget(textUnderLabel);
    textUnderLayout->addWidget(textUnderToolbar);
    stylesRow->addLayout(textUnderLayout);
    m_widgets.append(textUnderLabel);
    m_widgets.append(textUnderToolbar);
    
    stylesRow->addStretch();
    groupLayout->addLayout(stylesRow);
}


void MainWindowPage::setupStatusBar()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Status Bar")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Create a status bar with permanent and temporary widgets
    QStatusBar *statusBar = new QStatusBar(group);
    
    // Add permanent widgets (always visible on the right)
    QLabel *permanentLabel = new QLabel(tr("Ready"), statusBar);
    statusBar->addPermanentWidget(permanentLabel);
    
    QLabel *connectionLabel = new QLabel(tr("Connected"), statusBar);
    connectionLabel->setStyleSheet("color: green;");
    statusBar->addPermanentWidget(connectionLabel);
    
    // Show a temporary message (typically on the left)
    statusBar->showMessage(tr("Welcome to QtVanity - Status bar demonstration"), 0);
    
    groupLayout->addWidget(statusBar);
    m_widgets.append(statusBar);
    m_widgets.append(permanentLabel);
    m_widgets.append(connectionLabel);
    
    // Second status bar with different configuration
    QStatusBar *statusBar2 = new QStatusBar(group);
    
    // Add widgets to the left side
    QLabel *modeLabel = new QLabel(tr("Mode: Edit"), statusBar2);
    statusBar2->addWidget(modeLabel);
    
    QLabel *lineLabel = new QLabel(tr("Line: 1, Col: 1"), statusBar2);
    statusBar2->addWidget(lineLabel);
    
    // Add permanent widget on the right
    QLabel *encodingLabel = new QLabel(tr("UTF-8"), statusBar2);
    statusBar2->addPermanentWidget(encodingLabel);
    
    groupLayout->addWidget(statusBar2);
    m_widgets.append(statusBar2);
    m_widgets.append(modeLabel);
    m_widgets.append(lineLabel);
    m_widgets.append(encodingLabel);
}


void MainWindowPage::setupMenuBar()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Menu Bar")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Create a menu bar with sample menus
    QMenuBar *menuBar = new QMenuBar(group);
    
    // File menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    
    QAction *newAction = fileMenu->addAction(style()->standardIcon(QStyle::SP_FileIcon), tr("&New"));
    newAction->setShortcut(QKeySequence::New);
    
    QAction *openAction = fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogOpenButton), tr("&Open..."));
    openAction->setShortcut(QKeySequence::Open);
    
    QAction *saveAction = fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Save"));
    saveAction->setShortcut(QKeySequence::Save);
    
    QAction *saveAsAction = fileMenu->addAction(tr("Save &As..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    
    fileMenu->addSeparator();
    
    // Recent files submenu
    QMenu *recentMenu = fileMenu->addMenu(tr("Recent Files"));
    recentMenu->addAction(tr("document1.txt"));
    recentMenu->addAction(tr("document2.txt"));
    recentMenu->addAction(tr("document3.txt"));
    recentMenu->addSeparator();
    recentMenu->addAction(tr("Clear Recent"));
    
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    
    // Edit menu
    QMenu *editMenu = menuBar->addMenu(tr("&Edit"));
    QAction *undoAction = editMenu->addAction(tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    QAction *redoAction = editMenu->addAction(tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addSeparator();
    QAction *cutAction = editMenu->addAction(tr("Cu&t"));
    cutAction->setShortcut(QKeySequence::Cut);
    QAction *copyAction = editMenu->addAction(tr("&Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    QAction *pasteAction = editMenu->addAction(tr("&Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);
    editMenu->addSeparator();
    QAction *selectAllAction = editMenu->addAction(tr("Select &All"));
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    
    // View menu with checkable items
    QMenu *viewMenu = menuBar->addMenu(tr("&View"));
    QAction *toolbarAction = viewMenu->addAction(tr("&Toolbar"));
    toolbarAction->setCheckable(true);
    toolbarAction->setChecked(true);
    QAction *statusbarAction = viewMenu->addAction(tr("&Status Bar"));
    statusbarAction->setCheckable(true);
    statusbarAction->setChecked(true);
    viewMenu->addSeparator();
    
    // Zoom submenu
    QMenu *zoomMenu = viewMenu->addMenu(tr("&Zoom"));
    QAction *zoomInAction = zoomMenu->addAction(tr("Zoom &In"));
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    QAction *zoomOutAction = zoomMenu->addAction(tr("Zoom &Out"));
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    zoomMenu->addAction(tr("&Reset Zoom"));
    
    // Help menu
    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));
    QAction *contentsAction = helpMenu->addAction(style()->standardIcon(QStyle::SP_DialogHelpButton), 
                        tr("&Contents"));
    contentsAction->setShortcut(QKeySequence::HelpContents);
    helpMenu->addSeparator();
    helpMenu->addAction(style()->standardIcon(QStyle::SP_MessageBoxInformation), 
                        tr("&About"));
    helpMenu->addAction(tr("About &Qt"));
    
    groupLayout->addWidget(menuBar);
    m_widgets.append(menuBar);
}


void MainWindowPage::setupDockWidget()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Dock Widget")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *dockRow = new QHBoxLayout();
    dockRow->setSpacing(16);
    
    // Create a dock widget with a list widget as content
    QDockWidget *dockWidget1 = new QDockWidget(tr("Navigator"), group);
    dockWidget1->setFeatures(QDockWidget::DockWidgetClosable | 
                             QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetFloatable);
    
    QListWidget *listWidget = new QListWidget(dockWidget1);
    listWidget->addItem(tr("Item 1"));
    listWidget->addItem(tr("Item 2"));
    listWidget->addItem(tr("Item 3"));
    listWidget->addItem(tr("Item 4"));
    listWidget->setMaximumHeight(120);
    dockWidget1->setWidget(listWidget);
    dockWidget1->setMaximumWidth(200);
    
    dockRow->addWidget(dockWidget1);
    m_widgets.append(dockWidget1);
    m_widgets.append(listWidget);
    
    // Create another dock widget with a text edit as content
    QDockWidget *dockWidget2 = new QDockWidget(tr("Properties"), group);
    dockWidget2->setFeatures(QDockWidget::DockWidgetClosable | 
                             QDockWidget::DockWidgetMovable);
    
    QTextEdit *textEdit = new QTextEdit(dockWidget2);
    textEdit->setPlainText(tr("Property: Value\nName: Sample\nType: Widget"));
    textEdit->setMaximumHeight(120);
    textEdit->setReadOnly(true);
    dockWidget2->setWidget(textEdit);
    dockWidget2->setMaximumWidth(200);
    
    dockRow->addWidget(dockWidget2);
    m_widgets.append(dockWidget2);
    m_widgets.append(textEdit);
    
    // Create a dock widget with no features (non-closable, non-movable)
    QDockWidget *dockWidget3 = new QDockWidget(tr("Fixed Panel"), group);
    dockWidget3->setFeatures(QDockWidget::NoDockWidgetFeatures);
    
    QLabel *fixedLabel = new QLabel(tr("This dock widget\ncannot be closed\nor moved."), dockWidget3);
    fixedLabel->setAlignment(Qt::AlignCenter);
    fixedLabel->setMaximumHeight(120);
    dockWidget3->setWidget(fixedLabel);
    dockWidget3->setMaximumWidth(150);
    
    dockRow->addWidget(dockWidget3);
    m_widgets.append(dockWidget3);
    m_widgets.append(fixedLabel);
    
    dockRow->addStretch();
    groupLayout->addLayout(dockRow);
}


void MainWindowPage::setupSplitter()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Splitters")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Horizontal splitter
    QVBoxLayout *hSplitterLayout = new QVBoxLayout();
    QLabel *hSplitterLabel = new QLabel(tr("Horizontal Splitter"), group);
    
    QSplitter *hSplitter = new QSplitter(Qt::Horizontal, group);
    hSplitter->setChildrenCollapsible(false);
    
    QFrame *leftFrame = new QFrame(hSplitter);
    leftFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftFrame);
    QLabel *leftLabel = new QLabel(tr("Left Pane"), leftFrame);
    leftLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(leftLabel);
    
    QFrame *centerFrame = new QFrame(hSplitter);
    centerFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *centerLayout = new QVBoxLayout(centerFrame);
    QLabel *centerLabel = new QLabel(tr("Center Pane"), centerFrame);
    centerLabel->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(centerLabel);
    
    QFrame *rightFrame = new QFrame(hSplitter);
    rightFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightFrame);
    QLabel *rightLabel = new QLabel(tr("Right Pane"), rightFrame);
    rightLabel->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(rightLabel);
    
    hSplitter->addWidget(leftFrame);
    hSplitter->addWidget(centerFrame);
    hSplitter->addWidget(rightFrame);
    hSplitter->setSizes({100, 200, 100});
    hSplitter->setMaximumHeight(100);
    
    hSplitterLayout->addWidget(hSplitterLabel);
    hSplitterLayout->addWidget(hSplitter);
    groupLayout->addLayout(hSplitterLayout);
    m_widgets.append(hSplitterLabel);
    m_widgets.append(hSplitter);
    
    // Row 2: Vertical splitter
    QHBoxLayout *vSplitterRow = new QHBoxLayout();
    
    QVBoxLayout *vSplitterLayout = new QVBoxLayout();
    QLabel *vSplitterLabel = new QLabel(tr("Vertical Splitter"), group);
    
    QSplitter *vSplitter = new QSplitter(Qt::Vertical, group);
    vSplitter->setChildrenCollapsible(false);
    
    QFrame *topFrame = new QFrame(vSplitter);
    topFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *topLayout = new QVBoxLayout(topFrame);
    QLabel *topLabel = new QLabel(tr("Top Pane"), topFrame);
    topLabel->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(topLabel);
    
    QFrame *bottomFrame = new QFrame(vSplitter);
    bottomFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomFrame);
    QLabel *bottomLabel = new QLabel(tr("Bottom Pane"), bottomFrame);
    bottomLabel->setAlignment(Qt::AlignCenter);
    bottomLayout->addWidget(bottomLabel);
    
    vSplitter->addWidget(topFrame);
    vSplitter->addWidget(bottomFrame);
    vSplitter->setSizes({80, 80});
    vSplitter->setMaximumHeight(160);
    vSplitter->setMaximumWidth(200);
    
    vSplitterLayout->addWidget(vSplitterLabel);
    vSplitterLayout->addWidget(vSplitter);
    vSplitterRow->addLayout(vSplitterLayout);
    m_widgets.append(vSplitterLabel);
    m_widgets.append(vSplitter);
    
    // Nested splitter (horizontal containing vertical)
    QVBoxLayout *nestedLayout = new QVBoxLayout();
    QLabel *nestedLabel = new QLabel(tr("Nested Splitter"), group);
    
    QSplitter *nestedSplitter = new QSplitter(Qt::Horizontal, group);
    nestedSplitter->setChildrenCollapsible(false);
    
    QFrame *nestedLeftFrame = new QFrame(nestedSplitter);
    nestedLeftFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *nestedLeftLayout = new QVBoxLayout(nestedLeftFrame);
    QLabel *nestedLeftLabel = new QLabel(tr("Left"), nestedLeftFrame);
    nestedLeftLabel->setAlignment(Qt::AlignCenter);
    nestedLeftLayout->addWidget(nestedLeftLabel);
    
    QSplitter *innerSplitter = new QSplitter(Qt::Vertical, nestedSplitter);
    innerSplitter->setChildrenCollapsible(false);
    
    QFrame *innerTopFrame = new QFrame(innerSplitter);
    innerTopFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *innerTopLayout = new QVBoxLayout(innerTopFrame);
    QLabel *innerTopLabel = new QLabel(tr("Top"), innerTopFrame);
    innerTopLabel->setAlignment(Qt::AlignCenter);
    innerTopLayout->addWidget(innerTopLabel);
    
    QFrame *innerBottomFrame = new QFrame(innerSplitter);
    innerBottomFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *innerBottomLayout = new QVBoxLayout(innerBottomFrame);
    QLabel *innerBottomLabel = new QLabel(tr("Bottom"), innerBottomFrame);
    innerBottomLabel->setAlignment(Qt::AlignCenter);
    innerBottomLayout->addWidget(innerBottomLabel);
    
    innerSplitter->addWidget(innerTopFrame);
    innerSplitter->addWidget(innerBottomFrame);
    
    nestedSplitter->addWidget(nestedLeftFrame);
    nestedSplitter->addWidget(innerSplitter);
    nestedSplitter->setSizes({80, 120});
    nestedSplitter->setMaximumHeight(160);
    nestedSplitter->setMaximumWidth(200);
    
    nestedLayout->addWidget(nestedLabel);
    nestedLayout->addWidget(nestedSplitter);
    vSplitterRow->addLayout(nestedLayout);
    m_widgets.append(nestedLabel);
    m_widgets.append(nestedSplitter);
    
    vSplitterRow->addStretch();
    groupLayout->addLayout(vSplitterRow);
}
