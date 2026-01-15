#ifndef TEST_ADVANCEDPAGE_H
#define TEST_ADVANCEDPAGE_H

#include <QObject>
#include <QtTest>

/**
 * @brief Test class for AdvancedPage functionality.
 * 
 * Tests include:
 * - Widget presence verification (QMdiArea, QListView, QTreeView, 
 *   QTableView, QColumnView, QGraphicsView)
 * - Enabled state propagation
 */
class TestAdvancedPage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Unit tests
    void testPageCreation();
    void testMdiAreaPresent();
    void testListViewPresent();
    void testTreeViewPresent();
    void testTableViewPresent();
    void testColumnViewPresent();
    void testGraphicsViewPresent();
    void testSetWidgetsEnabled();
};

#endif // TEST_ADVANCEDPAGE_H
