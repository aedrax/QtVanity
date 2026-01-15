#include "test_dialogspage.h"
#include "DialogsPage.h"

#include <QPushButton>

void TestDialogsPage::initTestCase()
{
    // Setup code if needed
}

void TestDialogsPage::cleanupTestCase()
{
    // Cleanup code if needed
}

void TestDialogsPage::testPageCreation()
{
    DialogsPage page;
    
    // Page should be created without errors
    // Verify the page has child widgets (indicates setup was successful)
    QVERIFY(page.findChildren<QWidget*>().size() > 0);
}

void TestDialogsPage::testWizardButtonPresent()
{
    DialogsPage page;
    
    // Find the wizard button by object name
    QPushButton *wizardBtn = page.findChild<QPushButton*>("wizardButton");
    
    QVERIFY2(wizardBtn != nullptr, "Wizard trigger button should exist");
    QVERIFY2(wizardBtn->text().contains("Wizard", Qt::CaseInsensitive),
             "Wizard button should have appropriate text");
}

void TestDialogsPage::testInputDialogButtonsPresent()
{
    DialogsPage page;
    
    // Find input dialog buttons by object name
    QPushButton *textBtn = page.findChild<QPushButton*>("textInputButton");
    QPushButton *intBtn = page.findChild<QPushButton*>("intInputButton");
    QPushButton *itemBtn = page.findChild<QPushButton*>("itemSelectionButton");
    
    QVERIFY2(textBtn != nullptr, "Text input dialog button should exist");
    QVERIFY2(intBtn != nullptr, "Integer input dialog button should exist");
    QVERIFY2(itemBtn != nullptr, "Item selection dialog button should exist");
}

void TestDialogsPage::testSetWidgetsEnabled()
{
    DialogsPage page;
    
    // Find the dialog trigger buttons
    QPushButton *wizardBtn = page.findChild<QPushButton*>("wizardButton");
    QPushButton *textBtn = page.findChild<QPushButton*>("textInputButton");
    QPushButton *intBtn = page.findChild<QPushButton*>("intInputButton");
    QPushButton *itemBtn = page.findChild<QPushButton*>("itemSelectionButton");
    
    // Disable all widgets
    page.setWidgetsEnabled(false);
    
    // Verify buttons are disabled
    QVERIFY2(!wizardBtn->isEnabled(), "Wizard button should be disabled");
    QVERIFY2(!textBtn->isEnabled(), "Text input button should be disabled");
    QVERIFY2(!intBtn->isEnabled(), "Integer input button should be disabled");
    QVERIFY2(!itemBtn->isEnabled(), "Item selection button should be disabled");
    
    // Enable all widgets
    page.setWidgetsEnabled(true);
    
    // Verify buttons are enabled
    QVERIFY2(wizardBtn->isEnabled(), "Wizard button should be enabled");
    QVERIFY2(textBtn->isEnabled(), "Text input button should be enabled");
    QVERIFY2(intBtn->isEnabled(), "Integer input button should be enabled");
    QVERIFY2(itemBtn->isEnabled(), "Item selection button should be enabled");
}
