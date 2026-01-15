#include "test_containerspage.h"
#include "ContainersPage.h"

#include <QToolBox>
#include <QTabBar>

void TestContainersPage::initTestCase()
{
    // Setup code if needed
}

void TestContainersPage::cleanupTestCase()
{
    // Cleanup code if needed
}

void TestContainersPage::testPageCreation()
{
    ContainersPage page;
    
    // Page should be created without errors
    // Verify the page has child widgets (indicates setup was successful)
    QVERIFY(page.findChildren<QWidget*>().size() > 0);
}

void TestContainersPage::testToolBoxPresent()
{
    ContainersPage page;
    
    // Find QToolBox children
    QList<QToolBox*> toolBoxes = page.findChildren<QToolBox*>();
    
    // Should have at least one tool box
    QVERIFY2(toolBoxes.size() >= 1, 
             qPrintable(QString("Expected at least 1 QToolBox, found %1").arg(toolBoxes.size())));
    
    // Verify the tool box has multiple sections
    QToolBox *toolBox = toolBoxes.first();
    QVERIFY2(toolBox->count() >= 3,
             qPrintable(QString("Expected at least 3 sections in QToolBox, found %1").arg(toolBox->count())));
}

void TestContainersPage::testTabBarPresent()
{
    ContainersPage page;
    
    // Find QTabBar children (excluding those inside QTabWidget)
    QList<QTabBar*> tabBars = page.findChildren<QTabBar*>();
    
    // Should have at least 3 standalone tab bars
    QVERIFY2(tabBars.size() >= 3, 
             qPrintable(QString("Expected at least 3 QTabBar widgets, found %1").arg(tabBars.size())));
}

void TestContainersPage::testSetWidgetsEnabled()
{
    ContainersPage page;
    
    // Disable all widgets
    page.setWidgetsEnabled(false);
    
    // Verify QToolBox is disabled
    QList<QToolBox*> toolBoxes = page.findChildren<QToolBox*>();
    for (QToolBox *toolBox : toolBoxes) {
        QVERIFY2(!toolBox->isEnabled(), "QToolBox should be disabled");
    }
    
    // Verify QTabBar widgets are disabled
    QList<QTabBar*> tabBars = page.findChildren<QTabBar*>();
    for (QTabBar *tabBar : tabBars) {
        QVERIFY2(!tabBar->isEnabled(), "QTabBar should be disabled");
    }
    
    // Enable all widgets
    page.setWidgetsEnabled(true);
    
    // Verify QToolBox is enabled
    for (QToolBox *toolBox : toolBoxes) {
        QVERIFY2(toolBox->isEnabled(), "QToolBox should be enabled");
    }
    
    // Verify QTabBar widgets are enabled
    for (QTabBar *tabBar : tabBars) {
        QVERIFY2(tabBar->isEnabled(), "QTabBar should be enabled");
    }
}
