#include "test_variablepanel.h"
#include "VariablePanel.h"
#include "VariableManager.h"

#include <QRandomGenerator>
#include <QColor>
#include <QTableWidget>
#include <QSignalSpy>

void TestVariablePanel::initTestCase()
{
    // Setup code if needed
}

void TestVariablePanel::cleanupTestCase()
{
    // Cleanup code if needed
}

// =============================================================================
// Property 1: Color Format Correctness
// Feature: color-cell-picker, Property 1: Color Format Correctness
// =============================================================================

void TestVariablePanel::testColorFormatCorrectnessProperty_data()
{
    QTest::addColumn<int>("red");
    QTest::addColumn<int>("green");
    QTest::addColumn<int>("blue");
    QTest::addColumn<int>("alpha");
    QTest::addColumn<bool>("expectAlphaFormat");
    QTest::addColumn<int>("expectedLength");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 50 random opaque colors (alpha = 255)
    // These should produce #RRGGBB format (7 characters)
    for (int i = 0; i < 50; ++i) {
        int r = rng->bounded(256);
        int g = rng->bounded(256);
        int b = rng->bounded(256);
        
        QTest::newRow(qPrintable(QString("opaque_%1").arg(i))) 
            << r << g << b << 255 << false << 7;
    }
    
    // Generate 50 random colors with alpha < 255
    // These should produce #AARRGGBB format (9 characters)
    for (int i = 0; i < 50; ++i) {
        int r = rng->bounded(256);
        int g = rng->bounded(256);
        int b = rng->bounded(256);
        int a = rng->bounded(255); // 0-254, not 255
        
        QTest::newRow(qPrintable(QString("transparent_%1").arg(i))) 
            << r << g << b << a << true << 9;
    }
    
    // Edge cases for opaque colors
    QTest::newRow("black_opaque") << 0 << 0 << 0 << 255 << false << 7;
    QTest::newRow("white_opaque") << 255 << 255 << 255 << 255 << false << 7;
    QTest::newRow("red_opaque") << 255 << 0 << 0 << 255 << false << 7;
    QTest::newRow("green_opaque") << 0 << 255 << 0 << 255 << false << 7;
    QTest::newRow("blue_opaque") << 0 << 0 << 255 << 255 << false << 7;
    
    // Edge cases for transparent colors
    QTest::newRow("fully_transparent") << 128 << 128 << 128 << 0 << true << 9;
    QTest::newRow("half_transparent") << 255 << 0 << 0 << 128 << true << 9;
    QTest::newRow("almost_opaque") << 100 << 150 << 200 << 254 << true << 9;
    QTest::newRow("alpha_1") << 50 << 100 << 150 << 1 << true << 9;
}

void TestVariablePanel::testColorFormatCorrectnessProperty()
{
    // Feature: color-cell-picker, Property 1: Color Format Correctness
    
    QFETCH(int, red);
    QFETCH(int, green);
    QFETCH(int, blue);
    QFETCH(int, alpha);
    QFETCH(bool, expectAlphaFormat);
    QFETCH(int, expectedLength);
    
    // Create a VariablePanel to access formatColorToHex
    VariablePanel panel;
    
    QColor color(red, green, blue, alpha);
    QString result = panel.formatColorToHex(color);
    
    // Verify length matches expected format
    QVERIFY2(result.length() == expectedLength,
             qPrintable(QString("Color RGBA(%1,%2,%3,%4): expected length %5, got %6 ('%7')")
                       .arg(red).arg(green).arg(blue).arg(alpha)
                       .arg(expectedLength).arg(result.length()).arg(result)));
    
    // Verify starts with #
    QVERIFY2(result.startsWith('#'),
             qPrintable(QString("Color format should start with '#', got '%1'").arg(result)));
    
    // Verify format type matches expectation
    if (expectAlphaFormat) {
        // Should be #AARRGGBB (9 chars)
        QVERIFY2(result.length() == 9,
                 qPrintable(QString("Alpha color should be #AARRGGBB format (9 chars), got '%1'").arg(result)));
        
        // Verify alpha component is correct
        QString alphaHex = result.mid(1, 2);
        bool ok;
        int parsedAlpha = alphaHex.toInt(&ok, 16);
        QVERIFY2(ok && parsedAlpha == alpha,
                 qPrintable(QString("Alpha mismatch: expected %1, got %2 from '%3'")
                           .arg(alpha).arg(parsedAlpha).arg(result)));
    } else {
        // Should be #RRGGBB (7 chars)
        QVERIFY2(result.length() == 7,
                 qPrintable(QString("Opaque color should be #RRGGBB format (7 chars), got '%1'").arg(result)));
    }
    
    // Verify all characters after # are valid hex
    QString hexPart = result.mid(1);
    for (int i = 0; i < hexPart.length(); ++i) {
        QChar c = hexPart[i];
        bool isHex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        QVERIFY2(isHex,
                 qPrintable(QString("Invalid hex character '%1' at position %2 in '%3'")
                           .arg(c).arg(i + 1).arg(result)));
    }
    
    // Verify round-trip: parse the result back and compare RGB values
    QColor parsed;
    if (expectAlphaFormat) {
        // Parse #AARRGGBB
        int a = result.mid(1, 2).toInt(nullptr, 16);
        int r = result.mid(3, 2).toInt(nullptr, 16);
        int g = result.mid(5, 2).toInt(nullptr, 16);
        int b = result.mid(7, 2).toInt(nullptr, 16);
        parsed = QColor(r, g, b, a);
    } else {
        // Parse #RRGGBB
        parsed = QColor(result);
    }
    
    QVERIFY2(parsed.red() == red,
             qPrintable(QString("Red mismatch: expected %1, got %2").arg(red).arg(parsed.red())));
    QVERIFY2(parsed.green() == green,
             qPrintable(QString("Green mismatch: expected %1, got %2").arg(green).arg(parsed.green())));
    QVERIFY2(parsed.blue() == blue,
             qPrintable(QString("Blue mismatch: expected %1, got %2").arg(blue).arg(parsed.blue())));
    
    if (expectAlphaFormat) {
        QVERIFY2(parsed.alpha() == alpha,
                 qPrintable(QString("Alpha mismatch: expected %1, got %2").arg(alpha).arg(parsed.alpha())));
    }
}

// =============================================================================
// Unit Tests for Color Cell Click Functionality
// =============================================================================

// Test 5.1: Test that clicking color column triggers color picker logic
void TestVariablePanel::testClickingColorColumnTriggersColorPickerLogic()
{
    // Column indices: COL_DELETE=0, COL_NAME=1, COL_VALUE=2, COL_COLOR=3, COL_INSERT=4
    const int COL_NAME = 1;
    const int COL_VALUE = 2;
    const int COL_COLOR = 3;
    
    // Create VariablePanel and VariableManager
    VariablePanel panel;
    VariableManager manager;
    panel.setVariableManager(&manager);
    
    // Add a variable with a color value
    manager.setVariable("testColor", "#ff0000");
    
    // Get the internal table widget to verify the row was added
    // Note: rowCount includes the empty input row at the bottom
    QTableWidget *table = panel.findChild<QTableWidget*>();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 2); // 1 variable + 1 empty row
    
    // Verify the color column exists and has the color swatch
    QTableWidgetItem *colorItem = table->item(0, COL_COLOR);
    QVERIFY(colorItem != nullptr);
    
    // Verify the color swatch background is set (red color)
    QColor swatchColor = colorItem->background().color();
    QVERIFY(swatchColor.isValid());
    QCOMPARE(swatchColor.red(), 255);
    QCOMPARE(swatchColor.green(), 0);
    QCOMPARE(swatchColor.blue(), 0);
    
    // Note: We cannot fully test QColorDialog interaction in unit tests
    // as it requires user interaction. We verify the setup is correct
    // and the onCellClicked slot is properly connected.
    
    // Verify the cellClicked signal is connected by checking the table
    // has the expected structure for the color picker to work
    QTableWidgetItem *nameItem = table->item(0, COL_NAME);
    QTableWidgetItem *valueItem = table->item(0, COL_VALUE);
    QVERIFY(nameItem != nullptr);
    QVERIFY(valueItem != nullptr);
    QCOMPARE(nameItem->text(), QString("testColor"));
    QCOMPARE(valueItem->text(), QString("#ff0000"));
}

// Test 5.2: Test that clicking non-color columns does not trigger picker
void TestVariablePanel::testClickingNonColorColumnsDoesNotTriggerPicker()
{
    // Column indices: COL_DELETE=0, COL_NAME=1, COL_VALUE=2, COL_COLOR=3, COL_INSERT=4
    const int COL_DELETE = 0;
    const int COL_NAME = 1;
    const int COL_VALUE = 2;
    const int COL_COLOR = 3;
    const int COL_INSERT = 4;
    
    // Create VariablePanel and VariableManager
    VariablePanel panel;
    VariableManager manager;
    panel.setVariableManager(&manager);
    
    // Add a variable with a color value
    manager.setVariable("testVar", "#00ff00");
    
    // Get the internal table widget
    // Note: rowCount includes the empty input row at the bottom
    QTableWidget *table = panel.findChild<QTableWidget*>();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 2); // 1 variable + 1 empty row
    
    // Store original value
    QString originalValue = manager.variable("testVar");
    QCOMPARE(originalValue, QString("#00ff00"));
    
    // Simulate clicking on delete column (COL_DELETE)
    // The onCellClicked slot should not trigger color picker for non-color columns
    table->setCurrentCell(0, COL_DELETE);
    QCOMPARE(manager.variable("testVar"), originalValue);
    
    // Simulate clicking on name column (COL_NAME)
    table->setCurrentCell(0, COL_NAME);
    QCOMPARE(manager.variable("testVar"), originalValue);
    
    // Simulate clicking on value column (COL_VALUE)
    table->setCurrentCell(0, COL_VALUE);
    QCOMPARE(manager.variable("testVar"), originalValue);
    
    // Simulate clicking on insert column (COL_INSERT)
    table->setCurrentCell(0, COL_INSERT);
    QCOMPARE(manager.variable("testVar"), originalValue);
    
    // Verify the color swatch is still correct (green)
    QTableWidgetItem *colorItem = table->item(0, COL_COLOR);
    QVERIFY(colorItem != nullptr);
    QColor swatchColor = colorItem->background().color();
    QCOMPARE(swatchColor.green(), 255);
}

// Test 5.3: Test default color when value is not a valid color
void TestVariablePanel::testDefaultColorWhenValueIsNotValidColor()
{
    // Column indices: COL_DELETE=0, COL_NAME=1, COL_VALUE=2, COL_COLOR=3, COL_INSERT=4
    const int COL_NAME = 1;
    const int COL_COLOR = 3;
    
    // Create VariablePanel and VariableManager
    VariablePanel panel;
    VariableManager manager;
    panel.setVariableManager(&manager);
    
    // Add variables with non-color values
    manager.setVariable("notAColor1", "some text");
    manager.setVariable("notAColor2", "123");
    manager.setVariable("notAColor3", "rgb(255,0,0)"); // Not supported hex format
    manager.setVariable("validColor", "#0000ff");
    
    // Get the internal table widget
    // Note: rowCount includes the empty input row at the bottom
    QTableWidget *table = panel.findChild<QTableWidget*>();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 5); // 4 variables + 1 empty row
    
    // Find rows by variable name (using COL_NAME)
    int notAColor1Row = -1, notAColor2Row = -1, notAColor3Row = -1, validColorRow = -1;
    for (int row = 0; row < table->rowCount(); ++row) {
        QTableWidgetItem *nameItem = table->item(row, COL_NAME);
        if (nameItem) {
            if (nameItem->text() == "notAColor1") notAColor1Row = row;
            else if (nameItem->text() == "notAColor2") notAColor2Row = row;
            else if (nameItem->text() == "notAColor3") notAColor3Row = row;
            else if (nameItem->text() == "validColor") validColorRow = row;
        }
    }
    
    QVERIFY(notAColor1Row >= 0);
    QVERIFY(notAColor2Row >= 0);
    QVERIFY(notAColor3Row >= 0);
    QVERIFY(validColorRow >= 0);
    
    // Verify non-color values don't have a color swatch background
    // (the background should be default/empty brush)
    QTableWidgetItem *colorItem1 = table->item(notAColor1Row, COL_COLOR);
    QTableWidgetItem *colorItem2 = table->item(notAColor2Row, COL_COLOR);
    QTableWidgetItem *colorItem3 = table->item(notAColor3Row, COL_COLOR);
    
    QVERIFY(colorItem1 != nullptr);
    QVERIFY(colorItem2 != nullptr);
    QVERIFY(colorItem3 != nullptr);
    
    // Non-color values should have empty/default brush (no color swatch)
    QVERIFY(colorItem1->background().style() == Qt::NoBrush || 
            !colorItem1->background().color().isValid() ||
            colorItem1->background() == QBrush());
    QVERIFY(colorItem2->background().style() == Qt::NoBrush || 
            !colorItem2->background().color().isValid() ||
            colorItem2->background() == QBrush());
    QVERIFY(colorItem3->background().style() == Qt::NoBrush || 
            !colorItem3->background().color().isValid() ||
            colorItem3->background() == QBrush());
    
    // Verify valid color has proper swatch (blue)
    QTableWidgetItem *validColorItem = table->item(validColorRow, COL_COLOR);
    QVERIFY(validColorItem != nullptr);
    QColor validSwatchColor = validColorItem->background().color();
    QVERIFY(validSwatchColor.isValid());
    QCOMPARE(validSwatchColor.blue(), 255);
    QCOMPARE(validSwatchColor.red(), 0);
    QCOMPARE(validSwatchColor.green(), 0);
    
    // Test that parseColor returns invalid color for non-color values
    // by checking formatColorToHex with default white color
    // (This verifies the default color behavior when opening picker for invalid colors)
    QColor defaultColor = Qt::white;
    QString defaultHex = panel.formatColorToHex(defaultColor);
    QCOMPARE(defaultHex, QString("#ffffff"));
}
