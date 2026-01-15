#include "test_advancedpage.h"
#include "AdvancedPage.h"

#include <QMdiArea>
#include <QListView>
#include <QTreeView>
#include <QTableView>
#include <QColumnView>
#include <QGraphicsView>
#include <QStandardItemModel>

void TestAdvancedPage::initTestCase()
{
    // Setup code if needed
}

void TestAdvancedPage::cleanupTestCase()
{
    // Cleanup code if needed
}

void TestAdvancedPage::testPageCreation()
{
    AdvancedPage page;
    
    // Page should be created without errors
    // Verify the page has child widgets (indicates setup was successful)
    QVERIFY(page.findChildren<QWidget*>().size() > 0);
}

void TestAdvancedPage::testMdiAreaPresent()
{
    AdvancedPage page;
    
    // Find QMdiArea children
    QList<QMdiArea*> mdiAreas = page.findChildren<QMdiArea*>();
    
    // Should have at least one MDI area
    QVERIFY2(mdiAreas.size() >= 1,
             qPrintable(QString("Expected at least 1 QMdiArea, found %1").arg(mdiAreas.size())));
    
    // Verify MDI area has sub-windows
    QMdiArea *mdiArea = mdiAreas.first();
    QVERIFY2(mdiArea->subWindowList().size() >= 2,
             "QMdiArea should have at least 2 sub-windows");
}

void TestAdvancedPage::testListViewPresent()
{
    AdvancedPage page;
    
    // Find QListView children
    QList<QListView*> listViews = page.findChildren<QListView*>();
    
    // Should have at least one list view
    QVERIFY2(listViews.size() >= 1,
             qPrintable(QString("Expected at least 1 QListView, found %1").arg(listViews.size())));
    
    // Verify list view has a model
    QListView *listView = listViews.first();
    QVERIFY2(listView->model() != nullptr, "QListView should have a model");
}

void TestAdvancedPage::testTreeViewPresent()
{
    AdvancedPage page;
    
    // Find QTreeView children
    QList<QTreeView*> treeViews = page.findChildren<QTreeView*>();
    
    // Should have at least one tree view
    QVERIFY2(treeViews.size() >= 1,
             qPrintable(QString("Expected at least 1 QTreeView, found %1").arg(treeViews.size())));
    
    // Verify tree view has a model with hierarchical data
    QTreeView *treeView = treeViews.first();
    QVERIFY2(treeView->model() != nullptr, "QTreeView should have a model");
    
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(treeView->model());
    QVERIFY2(model != nullptr, "QTreeView model should be QStandardItemModel");
    QVERIFY2(model->rowCount() >= 1, "QTreeView model should have at least 1 row");
}

void TestAdvancedPage::testTableViewPresent()
{
    AdvancedPage page;
    
    // Find QTableView children
    QList<QTableView*> tableViews = page.findChildren<QTableView*>();
    
    // Should have at least one table view
    QVERIFY2(tableViews.size() >= 1,
             qPrintable(QString("Expected at least 1 QTableView, found %1").arg(tableViews.size())));
    
    // Verify table view has a model with tabular data
    QTableView *tableView = tableViews.first();
    QVERIFY2(tableView->model() != nullptr, "QTableView should have a model");
    
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(tableView->model());
    QVERIFY2(model != nullptr, "QTableView model should be QStandardItemModel");
    QVERIFY2(model->rowCount() >= 1, "QTableView model should have at least 1 row");
    QVERIFY2(model->columnCount() >= 2, "QTableView model should have at least 2 columns");
}

void TestAdvancedPage::testColumnViewPresent()
{
    AdvancedPage page;
    
    // Find QColumnView children
    QList<QColumnView*> columnViews = page.findChildren<QColumnView*>();
    
    // Should have at least one column view
    QVERIFY2(columnViews.size() >= 1,
             qPrintable(QString("Expected at least 1 QColumnView, found %1").arg(columnViews.size())));
    
    // Verify column view has a model
    QColumnView *columnView = columnViews.first();
    QVERIFY2(columnView->model() != nullptr, "QColumnView should have a model");
}

void TestAdvancedPage::testGraphicsViewPresent()
{
    AdvancedPage page;
    
    // Find QGraphicsView children
    QList<QGraphicsView*> graphicsViews = page.findChildren<QGraphicsView*>();
    
    // Should have at least one graphics view
    QVERIFY2(graphicsViews.size() >= 1,
             qPrintable(QString("Expected at least 1 QGraphicsView, found %1").arg(graphicsViews.size())));
    
    // Verify graphics view has a scene with items
    QGraphicsView *graphicsView = graphicsViews.first();
    QVERIFY2(graphicsView->scene() != nullptr, "QGraphicsView should have a scene");
    QVERIFY2(graphicsView->scene()->items().size() >= 3,
             "QGraphicsScene should have at least 3 items");
}

void TestAdvancedPage::testSetWidgetsEnabled()
{
    AdvancedPage page;
    
    // Disable all widgets
    page.setWidgetsEnabled(false);
    
    // Verify widgets are disabled
    QList<QMdiArea*> mdiAreas = page.findChildren<QMdiArea*>();
    for (QMdiArea *mdiArea : mdiAreas) {
        QVERIFY2(!mdiArea->isEnabled(), "QMdiArea should be disabled");
    }
    
    QList<QListView*> listViews = page.findChildren<QListView*>();
    for (QListView *listView : listViews) {
        QVERIFY2(!listView->isEnabled(), "QListView should be disabled");
    }
    
    QList<QGraphicsView*> graphicsViews = page.findChildren<QGraphicsView*>();
    for (QGraphicsView *graphicsView : graphicsViews) {
        QVERIFY2(!graphicsView->isEnabled(), "QGraphicsView should be disabled");
    }
    
    // Enable all widgets
    page.setWidgetsEnabled(true);
    
    // Verify widgets are enabled
    for (QMdiArea *mdiArea : mdiAreas) {
        QVERIFY2(mdiArea->isEnabled(), "QMdiArea should be enabled");
    }
    
    for (QListView *listView : listViews) {
        QVERIFY2(listView->isEnabled(), "QListView should be enabled");
    }
    
    for (QGraphicsView *graphicsView : graphicsViews) {
        QVERIFY2(graphicsView->isEnabled(), "QGraphicsView should be enabled");
    }
}
