#include "test_qsseditor.h"
#include "QssEditor.h"

#include <QTextEdit>
#include <QTextCursor>
#include <QSignalSpy>
#include <QRandomGenerator>
#include <QElapsedTimer>

void TestQssEditor::initTestCase()
{
    // Setup code if needed
}

void TestQssEditor::cleanupTestCase()
{
    // Cleanup code if needed
}

// ============================================================================
// Unit Tests
// ============================================================================

void TestQssEditor::testSetStyleSheet()
{
    QssEditor editor;
    QString testContent = "QPushButton { color: red; }";
    
    editor.setStyleSheet(testContent);
    
    QCOMPARE(editor.styleSheet(), testContent);
}

void TestQssEditor::testGetStyleSheet()
{
    QssEditor editor;
    QString testContent = "QLabel { background-color: blue; }";
    
    editor.setStyleSheet(testContent);
    QString retrieved = editor.styleSheet();
    
    QCOMPARE(retrieved, testContent);
}

void TestQssEditor::testHasUnsavedChangesInitialState()
{
    QssEditor editor;
    
    // Initially should have no unsaved changes
    QVERIFY(!editor.hasUnsavedChanges());
}

void TestQssEditor::testHasUnsavedChangesAfterEdit()
{
    QssEditor editor;
    
    // Set initial content
    editor.setStyleSheet("/* Initial */");
    QVERIFY(!editor.hasUnsavedChanges());
    
    // Modify content via text edit
    editor.textEdit()->setPlainText("/* Modified */");
    
    // Should now have unsaved changes
    QVERIFY(editor.hasUnsavedChanges());
}

void TestQssEditor::testAutoApplyToggle()
{
    QssEditor editor;
    
    // Initially auto-apply should be disabled
    QVERIFY(!editor.isAutoApplyEnabled());
    
    // Enable auto-apply
    editor.setAutoApplyEnabled(true);
    QVERIFY(editor.isAutoApplyEnabled());
    
    // Disable auto-apply
    editor.setAutoApplyEnabled(false);
    QVERIFY(!editor.isAutoApplyEnabled());
}

// ============================================================================
// Property-Based Tests
// ============================================================================

/**
 * Property 8: Cursor Position Preservation
 * 
 * For any cursor position in the QSS_Editor text, after applying the 
 * stylesheet, the cursor position should remain at the same character offset.
 */
void TestQssEditor::testCursorPositionPreservation_data()
{
    QTest::addColumn<QString>("qssContent");
    QTest::addColumn<int>("cursorPosition");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Predefined QSS patterns
    QStringList selectors = {
        "QPushButton", "QLabel", "QLineEdit", "QComboBox", 
        "QCheckBox", "QRadioButton", "QSlider", "QSpinBox"
    };
    
    QStringList properties = {
        "color", "background-color", "border", "padding", "margin"
    };
    
    QStringList values = {
        "red", "blue", "#ffffff", "10px", "1px solid black"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        QString qss;
        
        // Generate 1-3 rules
        int numRules = 1 + (rng->generate() % 3);
        for (int r = 0; r < numRules; ++r) {
            QString selector = selectors[rng->generate() % selectors.size()];
            qss += selector + " {\n";
            
            int numProps = 1 + (rng->generate() % 3);
            for (int p = 0; p < numProps; ++p) {
                QString prop = properties[rng->generate() % properties.size()];
                QString val = values[rng->generate() % values.size()];
                qss += "    " + prop + ": " + val + ";\n";
            }
            qss += "}\n";
        }
        
        // Generate a random cursor position within the text
        int maxPos = qss.length();
        int cursorPos = maxPos > 0 ? (rng->generate() % (maxPos + 1)) : 0;
        
        QTest::newRow(qPrintable(QString("cursor_test_%1").arg(i))) 
            << qss << cursorPos;
    }
    
    // Edge cases
    QTest::newRow("empty_at_start") << QString("") << 0;
    QTest::newRow("single_char_at_start") << QString("*") << 0;
    QTest::newRow("single_char_at_end") << QString("*") << 1;
    QTest::newRow("cursor_at_end") << QString("QPushButton { color: red; }") << 27;
    QTest::newRow("cursor_in_middle") << QString("QPushButton { color: red; }") << 14;
}

void TestQssEditor::testCursorPositionPreservation()
{
    // Feature: qtvanity, Property 8: Cursor Position Preservation
    
    QFETCH(QString, qssContent);
    QFETCH(int, cursorPosition);
    
    QssEditor editor;
    editor.setStyleSheet(qssContent);
    
    // Set cursor position
    QTextCursor cursor = editor.textEdit()->textCursor();
    int maxPos = editor.textEdit()->toPlainText().length();
    int actualPos = qMin(cursorPosition, maxPos);
    cursor.setPosition(actualPos);
    editor.textEdit()->setTextCursor(cursor);
    
    // Verify cursor is at expected position before apply
    QCOMPARE(editor.textEdit()->textCursor().position(), actualPos);
    
    // Connect to applyRequested to simulate style application
    QSignalSpy spy(&editor, &QssEditor::applyRequested);
    
    // Trigger apply
    editor.apply();
    
    // Verify signal was emitted
    QCOMPARE(spy.count(), 1);
    
    // Verify cursor position is preserved
    QCOMPARE(editor.textEdit()->textCursor().position(), actualPos);
}

/**
 * Property 9: Unsaved Changes Tracking
 * 
 * For any modification to the QSS_Editor content after loading or saving, 
 * the hasUnsavedChanges() method should return true. After saving, it 
 * should return false.
 */
void TestQssEditor::testUnsavedChangesTracking_data()
{
    QTest::addColumn<QString>("initialContent");
    QTest::addColumn<QString>("modifiedContent");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    QStringList selectors = {
        "QPushButton", "QLabel", "QLineEdit", "QComboBox"
    };
    
    QStringList properties = {
        "color", "background-color", "border", "padding"
    };
    
    QStringList values = {
        "red", "blue", "green", "#ffffff", "10px"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        // Generate initial content
        QString initial;
        QString selector = selectors[rng->generate() % selectors.size()];
        QString prop = properties[rng->generate() % properties.size()];
        QString val = values[rng->generate() % values.size()];
        initial = selector + " { " + prop + ": " + val + "; }";
        
        // Generate modified content (different from initial)
        QString modified;
        QString selector2 = selectors[rng->generate() % selectors.size()];
        QString prop2 = properties[rng->generate() % properties.size()];
        QString val2 = values[rng->generate() % values.size()];
        modified = selector2 + " { " + prop2 + ": " + val2 + "; } /* modified */";
        
        QTest::newRow(qPrintable(QString("unsaved_test_%1").arg(i))) 
            << initial << modified;
    }
    
    // Edge cases
    QTest::newRow("empty_to_content") << QString("") << QString("/* new */");
    QTest::newRow("content_to_empty") << QString("/* old */") << QString("");
    QTest::newRow("whitespace_change") << QString("a") << QString("a ");
}

void TestQssEditor::testUnsavedChangesTracking()
{
    // Feature: qtvanity, Property 9: Unsaved Changes Tracking
    
    QFETCH(QString, initialContent);
    QFETCH(QString, modifiedContent);
    
    QssEditor editor;
    
    // Set initial content - should have no unsaved changes
    editor.setStyleSheet(initialContent);
    QVERIFY2(!editor.hasUnsavedChanges(), 
             "Should have no unsaved changes after setStyleSheet");
    
    // Modify content via text edit
    editor.textEdit()->setPlainText(modifiedContent);
    
    // Should now have unsaved changes
    QVERIFY2(editor.hasUnsavedChanges(), 
             "Should have unsaved changes after modification");
    
    // Mark as saved
    editor.markAsSaved();
    
    // Should no longer have unsaved changes
    QVERIFY2(!editor.hasUnsavedChanges(), 
             "Should have no unsaved changes after markAsSaved");
}

/**
 * Property 5: Auto-Apply Timing Behavior
 * 
 * For any text change in the QSS_Editor when auto-apply is enabled, 
 * the style should be applied to the application within the configured 
 * delay period (500ms) after the last text change.
 */
void TestQssEditor::testAutoApplyTiming_data()
{
    QTest::addColumn<QString>("qssContent");
    QTest::addColumn<int>("delayMs");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    QStringList selectors = {
        "QPushButton", "QLabel", "QLineEdit", "QComboBox"
    };
    
    QStringList properties = {
        "color", "background-color", "border"
    };
    
    QStringList values = {
        "red", "blue", "#ffffff", "10px"
    };
    
    // Generate 100 random test cases with varying delays
    for (int i = 0; i < 100; ++i) {
        QString qss;
        QString selector = selectors[rng->generate() % selectors.size()];
        QString prop = properties[rng->generate() % properties.size()];
        QString val = values[rng->generate() % values.size()];
        qss = selector + " { " + prop + ": " + val + "; }";
        
        // Use shorter delays for testing (50-200ms)
        int delay = 50 + (rng->generate() % 150);
        
        QTest::newRow(qPrintable(QString("timing_test_%1").arg(i))) 
            << qss << delay;
    }
    
    // Edge cases
    QTest::newRow("minimum_delay") << QString("* { color: red; }") << 10;
    QTest::newRow("default_delay") << QString("* { color: blue; }") << 500;
}

void TestQssEditor::testAutoApplyTiming()
{
    // Feature: qtvanity, Property 5: Auto-Apply Timing Behavior
    
    QFETCH(QString, qssContent);
    QFETCH(int, delayMs);
    
    QssEditor editor;
    editor.setAutoApplyDelay(delayMs);
    editor.setAutoApplyEnabled(true);
    
    QSignalSpy spy(&editor, &QssEditor::applyRequested);
    
    // Record start time
    QElapsedTimer timer;
    timer.start();
    
    // Modify content to trigger auto-apply
    editor.textEdit()->setPlainText(qssContent);
    
    // Should not have applied immediately
    QCOMPARE(spy.count(), 0);
    
    // Wait for auto-apply to trigger (delay + some margin)
    // Use processEvents to allow timer to fire
    int waitTime = delayMs + 100; // Add margin for processing
    QTest::qWait(waitTime);
    
    // Should have applied after the delay
    QVERIFY2(spy.count() >= 1, 
             qPrintable(QString("Expected applyRequested signal after %1ms delay, "
                               "waited %2ms, got %3 signals")
                       .arg(delayMs).arg(waitTime).arg(spy.count())));
    
    // Verify the elapsed time is at least the delay
    qint64 elapsed = timer.elapsed();
    QVERIFY2(elapsed >= delayMs, 
             qPrintable(QString("Auto-apply triggered too early: %1ms < %2ms")
                       .arg(elapsed).arg(delayMs)));
}
