#include "test_inputspage.h"
#include "InputsPage.h"

#include <QFontComboBox>
#include <QKeySequenceEdit>
#include <QPlainTextEdit>
#include <QScrollBar>

void TestInputsPage::initTestCase()
{
    // Setup code if needed
}

void TestInputsPage::cleanupTestCase()
{
    // Cleanup code if needed
}

void TestInputsPage::testPageCreation()
{
    InputsPage page;
    
    // Page should be created without errors
    // Verify the page has child widgets (indicates setup was successful)
    QVERIFY(page.findChildren<QWidget*>().size() > 0);
}

void TestInputsPage::testFontComboBoxPresent()
{
    InputsPage page;
    
    // Find QFontComboBox children
    QList<QFontComboBox*> fontCombos = page.findChildren<QFontComboBox*>();
    
    // Should have at least one font combo box
    QVERIFY2(fontCombos.size() >= 1, 
             qPrintable(QString("Expected at least 1 QFontComboBox, found %1").arg(fontCombos.size())));
}

void TestInputsPage::testKeySequenceEditPresent()
{
    InputsPage page;
    
    // Find QKeySequenceEdit children
    QList<QKeySequenceEdit*> keySeqEdits = page.findChildren<QKeySequenceEdit*>();
    
    // Should have at least one key sequence edit
    QVERIFY2(keySeqEdits.size() >= 1, 
             qPrintable(QString("Expected at least 1 QKeySequenceEdit, found %1").arg(keySeqEdits.size())));
}

void TestInputsPage::testPlainTextEditPresent()
{
    InputsPage page;
    
    // Find QPlainTextEdit children
    QList<QPlainTextEdit*> plainTextEdits = page.findChildren<QPlainTextEdit*>();
    
    // Should have at least one plain text edit
    QVERIFY2(plainTextEdits.size() >= 1, 
             qPrintable(QString("Expected at least 1 QPlainTextEdit, found %1").arg(plainTextEdits.size())));
}

void TestInputsPage::testScrollBarsPresent()
{
    InputsPage page;
    
    // Find QScrollBar children
    QList<QScrollBar*> scrollBars = page.findChildren<QScrollBar*>();
    
    // Should have at least 2 scroll bars (horizontal and vertical)
    QVERIFY2(scrollBars.size() >= 2, 
             qPrintable(QString("Expected at least 2 QScrollBar widgets, found %1").arg(scrollBars.size())));
    
    // Verify we have both horizontal and vertical orientations
    bool hasHorizontal = false;
    bool hasVertical = false;
    
    for (QScrollBar *scrollBar : scrollBars) {
        if (scrollBar->orientation() == Qt::Horizontal) {
            hasHorizontal = true;
        } else if (scrollBar->orientation() == Qt::Vertical) {
            hasVertical = true;
        }
    }
    
    QVERIFY2(hasHorizontal, "Should have at least one horizontal scroll bar");
    QVERIFY2(hasVertical, "Should have at least one vertical scroll bar");
}

void TestInputsPage::testSetWidgetsEnabled()
{
    InputsPage page;
    
    // Disable all widgets
    page.setWidgetsEnabled(false);
    
    // Verify widgets are disabled
    QList<QFontComboBox*> fontCombos = page.findChildren<QFontComboBox*>();
    for (QFontComboBox *combo : fontCombos) {
        QVERIFY2(!combo->isEnabled(), "QFontComboBox should be disabled");
    }
    
    QList<QKeySequenceEdit*> keySeqEdits = page.findChildren<QKeySequenceEdit*>();
    for (QKeySequenceEdit *edit : keySeqEdits) {
        QVERIFY2(!edit->isEnabled(), "QKeySequenceEdit should be disabled");
    }
    
    QList<QPlainTextEdit*> plainTextEdits = page.findChildren<QPlainTextEdit*>();
    for (QPlainTextEdit *edit : plainTextEdits) {
        QVERIFY2(!edit->isEnabled(), "QPlainTextEdit should be disabled");
    }
    
    // Enable all widgets
    page.setWidgetsEnabled(true);
    
    // Verify widgets are enabled
    for (QFontComboBox *combo : fontCombos) {
        QVERIFY2(combo->isEnabled(), "QFontComboBox should be enabled");
    }
    
    for (QKeySequenceEdit *edit : keySeqEdits) {
        QVERIFY2(edit->isEnabled(), "QKeySequenceEdit should be enabled");
    }
    
    for (QPlainTextEdit *edit : plainTextEdits) {
        QVERIFY2(edit->isEnabled(), "QPlainTextEdit should be enabled");
    }
}

void TestInputsPage::testSetReadOnly()
{
    InputsPage page;
    
    // Set read-only
    page.setReadOnly(true);
    
    // Verify QPlainTextEdit is read-only
    QList<QPlainTextEdit*> plainTextEdits = page.findChildren<QPlainTextEdit*>();
    for (QPlainTextEdit *edit : plainTextEdits) {
        QVERIFY2(edit->isReadOnly(), "QPlainTextEdit should be read-only");
    }
    
    // Set editable
    page.setReadOnly(false);
    
    // Verify QPlainTextEdit is editable
    for (QPlainTextEdit *edit : plainTextEdits) {
        QVERIFY2(!edit->isReadOnly(), "QPlainTextEdit should be editable");
    }
}
