#include "AdvancedPage.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QListView>
#include <QTreeView>
#include <QTableView>
#include <QColumnView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QStandardItemModel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>

AdvancedPage::AdvancedPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

AdvancedPage::~AdvancedPage()
{
    // Qt handles child widget deletion
}

void AdvancedPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_widgets) {
        widget->setEnabled(enabled);
    }
}

void AdvancedPage::setupWidgets()
{
    setupMdiArea();
    setupModelViews();
    setupGraphicsView();
}

void AdvancedPage::setupMdiArea()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("MDI Area")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Create MDI area
    QMdiArea *mdiArea = new QMdiArea(group);
    mdiArea->setMinimumSize(400, 250);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Create sample MDI sub-windows
    QMdiSubWindow *subWindow1 = new QMdiSubWindow();
    QTextEdit *textEdit1 = new QTextEdit();
    textEdit1->setPlainText(tr("Document 1\n\nThis is a sample MDI sub-window with text content."));
    subWindow1->setWidget(textEdit1);
    subWindow1->setWindowTitle(tr("Document 1"));
    mdiArea->addSubWindow(subWindow1);
    
    QMdiSubWindow *subWindow2 = new QMdiSubWindow();
    QTextEdit *textEdit2 = new QTextEdit();
    textEdit2->setPlainText(tr("Document 2\n\nAnother MDI sub-window demonstrating multiple documents."));
    subWindow2->setWidget(textEdit2);
    subWindow2->setWindowTitle(tr("Document 2"));
    mdiArea->addSubWindow(subWindow2);
    
    QMdiSubWindow *subWindow3 = new QMdiSubWindow();
    QLabel *label = new QLabel(tr("This sub-window contains a label widget."));
    label->setAlignment(Qt::AlignCenter);
    label->setMargin(20);
    subWindow3->setWidget(label);
    subWindow3->setWindowTitle(tr("Info Window"));
    mdiArea->addSubWindow(subWindow3);
    
    // Cascade the windows for better visibility
    mdiArea->cascadeSubWindows();
    
    groupLayout->addWidget(mdiArea);
    m_widgets.append(mdiArea);
}


void AdvancedPage::setupModelViews()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Model-Based Views")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: QListView and QTreeView
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(16);
    
    // QListView with QStandardItemModel
    QVBoxLayout *listViewLayout = new QVBoxLayout();
    QLabel *listViewLabel = new QLabel(tr("QListView"), group);
    QListView *listView = new QListView(group);
    listView->setMinimumSize(150, 150);
    
    QStandardItemModel *listModel = new QStandardItemModel(listView);
    listModel->appendRow(new QStandardItem(tr("Item 1")));
    listModel->appendRow(new QStandardItem(tr("Item 2")));
    listModel->appendRow(new QStandardItem(tr("Item 3")));
    listModel->appendRow(new QStandardItem(tr("Item 4")));
    listModel->appendRow(new QStandardItem(tr("Item 5")));
    listView->setModel(listModel);
    
    listViewLayout->addWidget(listViewLabel);
    listViewLayout->addWidget(listView);
    row1->addLayout(listViewLayout);
    m_widgets.append(listViewLabel);
    m_widgets.append(listView);
    
    // QTreeView with hierarchical QStandardItemModel
    QVBoxLayout *treeViewLayout = new QVBoxLayout();
    QLabel *treeViewLabel = new QLabel(tr("QTreeView"), group);
    QTreeView *treeView = new QTreeView(group);
    treeView->setMinimumSize(200, 150);
    
    QStandardItemModel *treeModel = new QStandardItemModel(treeView);
    treeModel->setHorizontalHeaderLabels({tr("Name"), tr("Type"), tr("Size")});
    
    // Documents folder
    QStandardItem *documents = new QStandardItem(tr("Documents"));
    QStandardItem *docType = new QStandardItem(tr("Folder"));
    QStandardItem *docSize = new QStandardItem(tr("--"));
    treeModel->appendRow({documents, docType, docSize});
    
    QStandardItem *report = new QStandardItem(tr("report.pdf"));
    QStandardItem *reportType = new QStandardItem(tr("PDF"));
    QStandardItem *reportSize = new QStandardItem(tr("1.2 MB"));
    documents->appendRow({report, reportType, reportSize});
    
    QStandardItem *notes = new QStandardItem(tr("notes.txt"));
    QStandardItem *notesType = new QStandardItem(tr("Text"));
    QStandardItem *notesSize = new QStandardItem(tr("4 KB"));
    documents->appendRow({notes, notesType, notesSize});
    
    // Pictures folder
    QStandardItem *pictures = new QStandardItem(tr("Pictures"));
    QStandardItem *picType = new QStandardItem(tr("Folder"));
    QStandardItem *picSize = new QStandardItem(tr("--"));
    treeModel->appendRow({pictures, picType, picSize});
    
    QStandardItem *photo = new QStandardItem(tr("photo.jpg"));
    QStandardItem *photoType = new QStandardItem(tr("JPEG"));
    QStandardItem *photoSize = new QStandardItem(tr("2.5 MB"));
    pictures->appendRow({photo, photoType, photoSize});
    
    treeView->setModel(treeModel);
    treeView->expandAll();
    
    treeViewLayout->addWidget(treeViewLabel);
    treeViewLayout->addWidget(treeView);
    row1->addLayout(treeViewLayout);
    m_widgets.append(treeViewLabel);
    m_widgets.append(treeView);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
    
    // Row 2: QTableView and QColumnView
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(16);
    
    // QTableView with tabular QStandardItemModel
    QVBoxLayout *tableViewLayout = new QVBoxLayout();
    QLabel *tableViewLabel = new QLabel(tr("QTableView"), group);
    QTableView *tableView = new QTableView(group);
    tableView->setMinimumSize(250, 150);
    
    QStandardItemModel *tableModel = new QStandardItemModel(4, 3, tableView);
    tableModel->setHorizontalHeaderLabels({tr("Name"), tr("Age"), tr("City")});
    
    tableModel->setItem(0, 0, new QStandardItem(tr("Alice")));
    tableModel->setItem(0, 1, new QStandardItem(tr("28")));
    tableModel->setItem(0, 2, new QStandardItem(tr("New York")));
    
    tableModel->setItem(1, 0, new QStandardItem(tr("Bob")));
    tableModel->setItem(1, 1, new QStandardItem(tr("35")));
    tableModel->setItem(1, 2, new QStandardItem(tr("London")));
    
    tableModel->setItem(2, 0, new QStandardItem(tr("Charlie")));
    tableModel->setItem(2, 1, new QStandardItem(tr("42")));
    tableModel->setItem(2, 2, new QStandardItem(tr("Paris")));
    
    tableModel->setItem(3, 0, new QStandardItem(tr("Diana")));
    tableModel->setItem(3, 1, new QStandardItem(tr("31")));
    tableModel->setItem(3, 2, new QStandardItem(tr("Tokyo")));
    
    tableView->setModel(tableModel);
    tableView->resizeColumnsToContents();
    
    tableViewLayout->addWidget(tableViewLabel);
    tableViewLayout->addWidget(tableView);
    row2->addLayout(tableViewLayout);
    m_widgets.append(tableViewLabel);
    m_widgets.append(tableView);
    
    // QColumnView with hierarchical navigation
    QVBoxLayout *columnViewLayout = new QVBoxLayout();
    QLabel *columnViewLabel = new QLabel(tr("QColumnView"), group);
    QColumnView *columnView = new QColumnView(group);
    columnView->setMinimumSize(300, 150);
    
    QStandardItemModel *columnModel = new QStandardItemModel(columnView);
    
    // Root level items
    QStandardItem *root1 = new QStandardItem(tr("Category A"));
    QStandardItem *root2 = new QStandardItem(tr("Category B"));
    QStandardItem *root3 = new QStandardItem(tr("Category C"));
    
    // Sub-items for Category A
    QStandardItem *subA1 = new QStandardItem(tr("Sub A1"));
    QStandardItem *subA2 = new QStandardItem(tr("Sub A2"));
    root1->appendRow(subA1);
    root1->appendRow(subA2);
    
    // Sub-sub-items
    subA1->appendRow(new QStandardItem(tr("Detail 1")));
    subA1->appendRow(new QStandardItem(tr("Detail 2")));
    
    // Sub-items for Category B
    root2->appendRow(new QStandardItem(tr("Sub B1")));
    root2->appendRow(new QStandardItem(tr("Sub B2")));
    root2->appendRow(new QStandardItem(tr("Sub B3")));
    
    columnModel->appendRow(root1);
    columnModel->appendRow(root2);
    columnModel->appendRow(root3);
    
    columnView->setModel(columnModel);
    
    columnViewLayout->addWidget(columnViewLabel);
    columnViewLayout->addWidget(columnView);
    row2->addLayout(columnViewLayout);
    m_widgets.append(columnViewLabel);
    m_widgets.append(columnView);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
}


void AdvancedPage::setupGraphicsView()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Graphics View")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(16);
    
    // Create QGraphicsScene with simple shapes
    QGraphicsScene *scene = new QGraphicsScene(group);
    scene->setSceneRect(0, 0, 300, 200);
    
    // Add a rectangle
    QGraphicsRectItem *rect = scene->addRect(20, 20, 80, 60, 
        QPen(Qt::blue, 2), QBrush(Qt::lightGray));
    rect->setToolTip(tr("Rectangle"));
    
    // Add an ellipse
    QGraphicsEllipseItem *ellipse = scene->addEllipse(120, 30, 70, 70,
        QPen(Qt::red, 2), QBrush(Qt::yellow));
    ellipse->setToolTip(tr("Ellipse"));
    
    // Add another rectangle
    QGraphicsRectItem *rect2 = scene->addRect(200, 50, 60, 80,
        QPen(Qt::darkGreen, 2), QBrush(Qt::cyan));
    rect2->setToolTip(tr("Rectangle 2"));
    
    // Add text
    QGraphicsTextItem *text = scene->addText(tr("Graphics View Demo"));
    text->setPos(80, 130);
    text->setDefaultTextColor(Qt::darkBlue);
    
    // Add a line
    scene->addLine(20, 170, 280, 170, QPen(Qt::darkMagenta, 2, Qt::DashLine));
    
    // Create QGraphicsView
    QGraphicsView *graphicsView = new QGraphicsView(scene, group);
    graphicsView->setMinimumSize(350, 250);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    
    row->addWidget(graphicsView);
    m_widgets.append(graphicsView);
    
    row->addStretch();
    groupLayout->addLayout(row);
}
