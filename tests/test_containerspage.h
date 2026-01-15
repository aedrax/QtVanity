#ifndef TEST_CONTAINERSPAGE_H
#define TEST_CONTAINERSPAGE_H

#include <QObject>
#include <QtTest>

/**
 * @brief Unit tests for ContainersPage widget presence.
 * 
 * Tests verify that the new container widgets are present:
 * - QToolBox
 * - QTabBar (standalone)
 */
class TestContainersPage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testPageCreation();
    void testToolBoxPresent();
    void testTabBarPresent();
    void testSetWidgetsEnabled();
};

#endif // TEST_CONTAINERSPAGE_H
