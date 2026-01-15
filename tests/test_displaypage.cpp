#include "test_displaypage.h"
#include "DisplayPage.h"

#include <QLabel>
#include <QLCDNumber>
#include <QCalendarWidget>
#include <QLineEdit>

void TestDisplayPage::initTestCase()
{
    // Setup code if needed
}

void TestDisplayPage::cleanupTestCase()
{
    // Cleanup code if needed
}

void TestDisplayPage::testPageCreation()
{
    DisplayPage page;
    
    // Page should be created without errors
    // Verify the page has child widgets (indicates setup was successful)
    QVERIFY(page.findChildren<QWidget*>().size() > 0);
}

void TestDisplayPage::testLabelWidgetsPresent()
{
    DisplayPage page;
    
    // Find QLabel children
    QList<QLabel*> labels = page.findChildren<QLabel*>();
    
    // Should have multiple labels (plain text, rich text, pixmap, buddy)
    QVERIFY2(labels.size() >= 4, 
             qPrintable(QString("Expected at least 4 QLabel widgets, found %1").arg(labels.size())));
    
    // Verify at least one label has rich text format
    bool hasRichText = false;
    for (QLabel *label : labels) {
        if (label->textFormat() == Qt::RichText) {
            hasRichText = true;
            break;
        }
    }
    QVERIFY2(hasRichText, "Should have at least one rich text label");
    
    // Verify at least one label has a buddy (QLineEdit)
    QList<QLineEdit*> lineEdits = page.findChildren<QLineEdit*>();
    QVERIFY2(!lineEdits.isEmpty(), "Should have at least one QLineEdit as buddy target");
}

void TestDisplayPage::testLCDNumberWidgetsPresent()
{
    DisplayPage page;
    
    // Find QLCDNumber children
    QList<QLCDNumber*> lcdNumbers = page.findChildren<QLCDNumber*>();
    
    // Should have multiple LCD numbers (decimal, hex, binary, different styles)
    QVERIFY2(lcdNumbers.size() >= 3, 
             qPrintable(QString("Expected at least 3 QLCDNumber widgets, found %1").arg(lcdNumbers.size())));
    
    // Verify different display modes are present
    bool hasDecimal = false;
    bool hasHex = false;
    bool hasBinary = false;
    
    for (QLCDNumber *lcd : lcdNumbers) {
        switch (lcd->mode()) {
            case QLCDNumber::Dec:
                hasDecimal = true;
                break;
            case QLCDNumber::Hex:
                hasHex = true;
                break;
            case QLCDNumber::Bin:
                hasBinary = true;
                break;
            default:
                break;
        }
    }
    
    QVERIFY2(hasDecimal, "Should have at least one decimal mode LCD");
    QVERIFY2(hasHex, "Should have at least one hexadecimal mode LCD");
    QVERIFY2(hasBinary, "Should have at least one binary mode LCD");
}

void TestDisplayPage::testCalendarWidgetPresent()
{
    DisplayPage page;
    
    // Find QCalendarWidget children
    QList<QCalendarWidget*> calendars = page.findChildren<QCalendarWidget*>();
    
    // Should have at least one calendar widget
    QVERIFY2(calendars.size() >= 1, 
             qPrintable(QString("Expected at least 1 QCalendarWidget, found %1").arg(calendars.size())));
    
    // Verify calendar has selection enabled
    QCalendarWidget *calendar = calendars.first();
    QVERIFY2(calendar->selectionMode() == QCalendarWidget::SingleSelection,
             "Calendar should have single selection mode enabled");
}

void TestDisplayPage::testSetWidgetsEnabled()
{
    DisplayPage page;
    
    // Disable all widgets
    page.setWidgetsEnabled(false);
    
    // Verify widgets are disabled
    QList<QLCDNumber*> lcdNumbers = page.findChildren<QLCDNumber*>();
    for (QLCDNumber *lcd : lcdNumbers) {
        QVERIFY2(!lcd->isEnabled(), "QLCDNumber should be disabled");
    }
    
    QList<QCalendarWidget*> calendars = page.findChildren<QCalendarWidget*>();
    for (QCalendarWidget *calendar : calendars) {
        QVERIFY2(!calendar->isEnabled(), "QCalendarWidget should be disabled");
    }
    
    // Enable all widgets
    page.setWidgetsEnabled(true);
    
    // Verify widgets are enabled
    for (QLCDNumber *lcd : lcdNumbers) {
        QVERIFY2(lcd->isEnabled(), "QLCDNumber should be enabled");
    }
    
    for (QCalendarWidget *calendar : calendars) {
        QVERIFY2(calendar->isEnabled(), "QCalendarWidget should be enabled");
    }
}
