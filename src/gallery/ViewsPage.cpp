#include "ViewsPage.h"

#include <QListView>
#include <QListWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QTableView>
#include <QTableWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QStyle>

ViewsPage::ViewsPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

ViewsPage::~ViewsPage()
{
    // Qt handles child widget deletion
}

void ViewsPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_widgets) {
        widget->setEnabled(enabled);
    }
}

void ViewsPage::setupWidgets()
{
    setupListWidgets();
    setupTreeWidgets();
    setupTableWidgets();
}

void ViewsPage::setupListWidgets()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("List Widgets")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(12);
    
    // QListWidget (simple)
    QVBoxLayout *listCol = new QVBoxLayout();
    QLabel *listLabel = new QLabel(tr("QListWidget:"), group);
    QListWidget *listWidget = new QListWidget(group);
    listWidget->addItem(tr("Item 1"));
    listWidget->addItem(tr("Item 2"));
    listWidget->addItem(tr("Item 3"));
    listWidget->addItem(tr("Item 4"));
    listWidget->addItem(tr("Item 5"));
    listWidget->setCurrentRow(1);
    listWidget->setMaximumHeight(150);
    listCol->addWidget(listLabel);
    listCol->addWidget(listWidget);
    row->addLayout(listCol);
    m_widgets.append(listWidget);
    
    // QListWidget with icons
    QVBoxLayout *iconListCol = new QVBoxLayout();
    QLabel *iconListLabel = new QLabel(tr("With icons:"), group);
    QListWidget *iconListWidget = new QListWidget(group);
    iconListWidget->addItem(new QListWidgetItem(
        style()->standardIcon(QStyle::SP_DirIcon), tr("Documents")));
    iconListWidget->addItem(new QListWidgetItem(
        style()->standardIcon(QStyle::SP_DirIcon), tr("Downloads")));
    iconListWidget->addItem(new QListWidgetItem(
        style()->standardIcon(QStyle::SP_DirIcon), tr("Pictures")));
    iconListWidget->addItem(new QListWidgetItem(
        style()->standardIcon(QStyle::SP_FileIcon), tr("readme.txt")));
    iconListWidget->addItem(new QListWidgetItem(
        style()->standardIcon(QStyle::SP_FileIcon), tr("data.csv")));
    iconListWidget->setMaximumHeight(150);
    iconListCol->addWidget(iconListLabel);
    iconListCol->addWidget(iconListWidget);
    row->addLayout(iconListCol);
    m_widgets.append(iconListWidget);
    
    // QListWidget with checkboxes
    QVBoxLayout *checkListCol = new QVBoxLayout();
    QLabel *checkListLabel = new QLabel(tr("Checkable:"), group);
    QListWidget *checkListWidget = new QListWidget(group);
    for (int i = 1; i <= 5; ++i) {
        QListWidgetItem *item = new QListWidgetItem(tr("Task %1").arg(i));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(i % 2 == 0 ? Qt::Checked : Qt::Unchecked);
        checkListWidget->addItem(item);
    }
    checkListWidget->setMaximumHeight(150);
    checkListCol->addWidget(checkListLabel);
    checkListCol->addWidget(checkListWidget);
    row->addLayout(checkListCol);
    m_widgets.append(checkListWidget);
    
    groupLayout->addLayout(row);
}

void ViewsPage::setupTreeWidgets()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Tree Widgets")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(12);
    
    // QTreeWidget (simple hierarchy)
    QVBoxLayout *treeCol = new QVBoxLayout();
    QLabel *treeLabel = new QLabel(tr("QTreeWidget:"), group);
    QTreeWidget *treeWidget = new QTreeWidget(group);
    treeWidget->setHeaderLabel(tr("Items"));
    
    QTreeWidgetItem *root1 = new QTreeWidgetItem(treeWidget, QStringList() << tr("Root 1"));
    new QTreeWidgetItem(root1, QStringList() << tr("Child 1.1"));
    new QTreeWidgetItem(root1, QStringList() << tr("Child 1.2"));
    QTreeWidgetItem *child13 = new QTreeWidgetItem(root1, QStringList() << tr("Child 1.3"));
    new QTreeWidgetItem(child13, QStringList() << tr("Grandchild 1.3.1"));
    new QTreeWidgetItem(child13, QStringList() << tr("Grandchild 1.3.2"));
    
    QTreeWidgetItem *root2 = new QTreeWidgetItem(treeWidget, QStringList() << tr("Root 2"));
    new QTreeWidgetItem(root2, QStringList() << tr("Child 2.1"));
    new QTreeWidgetItem(root2, QStringList() << tr("Child 2.2"));
    
    treeWidget->expandAll();
    treeWidget->setMaximumHeight(200);
    treeCol->addWidget(treeLabel);
    treeCol->addWidget(treeWidget);
    row->addLayout(treeCol);
    m_widgets.append(treeWidget);
    
    // QTreeWidget with icons (file browser style)
    QVBoxLayout *iconTreeCol = new QVBoxLayout();
    QLabel *iconTreeLabel = new QLabel(tr("File browser style:"), group);
    QTreeWidget *iconTreeWidget = new QTreeWidget(group);
    iconTreeWidget->setHeaderLabel(tr("Files"));
    
    QTreeWidgetItem *homeDir = new QTreeWidgetItem(iconTreeWidget);
    homeDir->setText(0, tr("Home"));
    homeDir->setIcon(0, style()->standardIcon(QStyle::SP_DirHomeIcon));
    
    QTreeWidgetItem *docsDir = new QTreeWidgetItem(homeDir);
    docsDir->setText(0, tr("Documents"));
    docsDir->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
    
    QTreeWidgetItem *file1 = new QTreeWidgetItem(docsDir);
    file1->setText(0, tr("report.pdf"));
    file1->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
    
    QTreeWidgetItem *file2 = new QTreeWidgetItem(docsDir);
    file2->setText(0, tr("notes.txt"));
    file2->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
    
    QTreeWidgetItem *picDir = new QTreeWidgetItem(homeDir);
    picDir->setText(0, tr("Pictures"));
    picDir->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
    
    QTreeWidgetItem *pic1 = new QTreeWidgetItem(picDir);
    pic1->setText(0, tr("photo.jpg"));
    pic1->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
    
    iconTreeWidget->expandAll();
    iconTreeWidget->setMaximumHeight(200);
    iconTreeCol->addWidget(iconTreeLabel);
    iconTreeCol->addWidget(iconTreeWidget);
    row->addLayout(iconTreeCol);
    m_widgets.append(iconTreeWidget);
    
    groupLayout->addLayout(row);
}

void ViewsPage::setupTableWidgets()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Table Widgets")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(12);
    
    // QTableWidget (simple)
    QVBoxLayout *tableCol = new QVBoxLayout();
    QLabel *tableLabel = new QLabel(tr("QTableWidget:"), group);
    QTableWidget *tableWidget = new QTableWidget(5, 3, group);
    tableWidget->setHorizontalHeaderLabels({tr("Name"), tr("Value"), tr("Status")});
    
    QStringList names = {tr("Alpha"), tr("Beta"), tr("Gamma"), tr("Delta"), tr("Epsilon")};
    QStringList statuses = {tr("Active"), tr("Pending"), tr("Complete"), tr("Active"), tr("Pending")};
    
    for (int row = 0; row < 5; ++row) {
        tableWidget->setItem(row, 0, new QTableWidgetItem(names[row]));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number((row + 1) * 10)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(statuses[row]));
    }
    
    tableWidget->setMaximumHeight(180);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableCol->addWidget(tableLabel);
    tableCol->addWidget(tableWidget);
    row->addLayout(tableCol);
    m_widgets.append(tableWidget);
    
    // QTableWidget with alternating colors
    QVBoxLayout *altTableCol = new QVBoxLayout();
    QLabel *altTableLabel = new QLabel(tr("Alternating rows:"), group);
    QTableWidget *altTableWidget = new QTableWidget(6, 2, group);
    altTableWidget->setHorizontalHeaderLabels({tr("Product"), tr("Price")});
    altTableWidget->setAlternatingRowColors(true);
    
    QStringList products = {tr("Widget A"), tr("Widget B"), tr("Gadget X"), 
                           tr("Gadget Y"), tr("Tool 1"), tr("Tool 2")};
    QStringList prices = {"$19.99", "$29.99", "$49.99", "$59.99", "$9.99", "$14.99"};
    
    for (int row = 0; row < 6; ++row) {
        altTableWidget->setItem(row, 0, new QTableWidgetItem(products[row]));
        altTableWidget->setItem(row, 1, new QTableWidgetItem(prices[row]));
    }
    
    altTableWidget->setMaximumHeight(180);
    altTableWidget->horizontalHeader()->setStretchLastSection(true);
    altTableCol->addWidget(altTableLabel);
    altTableCol->addWidget(altTableWidget);
    row->addLayout(altTableCol);
    m_widgets.append(altTableWidget);
    
    groupLayout->addLayout(row);
}
