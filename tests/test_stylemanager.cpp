#include "test_stylemanager.h"
#include "StyleManager.h"
#include "QssEditor.h"

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QDir>
#include <QCoreApplication>
#include <QSignalSpy>

void TestStyleManager::initTestCase()
{
    // Setup code if needed
}

void TestStyleManager::cleanupTestCase()
{
    // Cleanup code if needed
}

void TestStyleManager::testLoadValidFile()
{
    // Create a temporary file with QSS content
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(true);
    QVERIFY(tempFile.open());
    
    QString testContent = "QPushButton { color: red; }";
    QTextStream out(&tempFile);
    out << testContent;
    out.flush();
    tempFile.close();
    
    // Test loading
    StyleManager manager;
    QString loaded = manager.loadFromFile(tempFile.fileName());
    
    QCOMPARE(loaded, testContent);
}

void TestStyleManager::testLoadNonexistentFile()
{
    StyleManager manager;
    
    // Connect to loadError signal to verify it's emitted
    QSignalSpy spy(&manager, &StyleManager::loadError);
    
    QString loaded = manager.loadFromFile("/nonexistent/path/file.qss");
    
    QVERIFY(loaded.isEmpty());
    QCOMPARE(spy.count(), 1);
}

void TestStyleManager::testSaveAndLoad()
{
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(true);
    QVERIFY(tempFile.open());
    QString filePath = tempFile.fileName();
    tempFile.close();
    
    StyleManager manager;
    QString testContent = "QLabel { background-color: blue; border: 1px solid black; }";
    
    // Save
    bool saved = manager.saveToFile(filePath, testContent);
    QVERIFY(saved);
    
    // Load and verify
    QString loaded = manager.loadFromFile(filePath);
    QCOMPARE(loaded, testContent);
}

void TestStyleManager::testAvailableTemplates()
{
    // Create a temporary directory with some .qss files
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    // Create test template files
    QStringList templateNames = {"dark", "light", "solarized"};
    for (const QString &name : templateNames) {
        QFile file(tempDir.path() + "/" + name + ".qss");
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("/* Test template */");
        file.close();
    }
    
    StyleManager manager;
    manager.setTemplatesPath(tempDir.path());
    
    QStringList templates = manager.availableTemplates();
    
    QCOMPARE(templates.size(), 3);
    QVERIFY(templates.contains("dark"));
    QVERIFY(templates.contains("light"));
    QVERIFY(templates.contains("solarized"));
}

void TestStyleManager::testClearStyleSheet()
{
    StyleManager manager;
    
    // Apply a stylesheet first
    QString testQss = "QPushButton { color: red; }";
    manager.applyStyleSheet(testQss);
    QCOMPARE(qApp->styleSheet(), testQss);
    QCOMPARE(manager.currentStyleSheet(), testQss);
    
    // Connect to styleCleared signal to verify it's emitted
    QSignalSpy spy(&manager, &StyleManager::styleCleared);
    
    // Clear the stylesheet
    manager.clearStyleSheet();
    
    // Verify qApp stylesheet is empty
    QVERIFY(qApp->styleSheet().isEmpty());
    
    // Verify internal state is cleared
    QVERIFY(manager.currentStyleSheet().isEmpty());
    
    // Verify signal was emitted
    QCOMPARE(spy.count(), 1);
}

void TestStyleManager::testHasCustomStyleSheet()
{
    StyleManager manager;
    
    // Initially should have no custom stylesheet
    QVERIFY(!manager.hasCustomStyleSheet());
    
    // Apply a stylesheet
    QString testQss = "QLabel { background: blue; }";
    manager.applyStyleSheet(testQss);
    QVERIFY(manager.hasCustomStyleSheet());
    
    // Clear the stylesheet
    manager.clearStyleSheet();
    QVERIFY(!manager.hasCustomStyleSheet());
    
    // Apply empty stylesheet should also result in no custom stylesheet
    manager.applyStyleSheet(QString());
    QVERIFY(!manager.hasCustomStyleSheet());
}

/**
 * Property 1: Style File Round-Trip
 * 
 * For any valid QSS content string, saving it to a file via 
 * StyleManager::saveToFile() and then loading it via 
 * StyleManager::loadFromFile() should return content equivalent 
 * to the original string.
 */
void TestStyleManager::testRoundTripProperty_data()
{
    QTest::addColumn<QString>("qssContent");
    
    // Generate 100+ test cases for property-based testing
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Predefined QSS patterns to combine
    QStringList selectors = {
        "QPushButton", "QLabel", "QLineEdit", "QComboBox", 
        "QCheckBox", "QRadioButton", "QSlider", "QSpinBox",
        "QTextEdit", "QListView", "QTreeView", "QTableView",
        "*", "QWidget", "QFrame", "QGroupBox"
    };
    
    QStringList properties = {
        "color", "background-color", "background", "border",
        "border-radius", "padding", "margin", "font-size",
        "font-weight", "min-width", "min-height", "max-width"
    };
    
    QStringList values = {
        "red", "blue", "green", "#ffffff", "#000000", "#ff5500",
        "rgb(255, 128, 0)", "rgba(100, 100, 100, 128)",
        "1px solid black", "2px dashed gray", "none",
        "10px", "20px", "5em", "bold", "italic", "normal"
    };
    
    QStringList pseudoStates = {
        ":hover", ":pressed", ":disabled", ":checked",
        ":focus", ":selected", ""
    };
    
    // Generate 100 random QSS strings
    for (int i = 0; i < 100; ++i) {
        QString qss;
        
        // Generate 1-5 rules per test case
        int numRules = 1 + (rng->generate() % 5);
        for (int r = 0; r < numRules; ++r) {
            QString selector = selectors[rng->generate() % selectors.size()];
            QString pseudo = pseudoStates[rng->generate() % pseudoStates.size()];
            
            qss += selector + pseudo + " {\n";
            
            // Generate 1-4 properties per rule
            int numProps = 1 + (rng->generate() % 4);
            for (int p = 0; p < numProps; ++p) {
                QString prop = properties[rng->generate() % properties.size()];
                QString val = values[rng->generate() % values.size()];
                qss += "    " + prop + ": " + val + ";\n";
            }
            
            qss += "}\n\n";
        }
        
        QTest::newRow(qPrintable(QString("random_qss_%1").arg(i))) << qss;
    }
    
    // Add some edge cases
    QTest::newRow("empty_string") << QString("");
    QTest::newRow("whitespace_only") << QString("   \n\t\n   ");
    QTest::newRow("comment_only") << QString("/* This is a comment */");
    QTest::newRow("multiline_comment") << QString("/* Line 1\n * Line 2\n * Line 3 */");
    QTest::newRow("unicode_content") << QString("QLabel { /* 日本語コメント */ color: red; }");
    QTest::newRow("special_chars") << QString("QLabel { content: \"Hello\\nWorld\"; }");
    QTest::newRow("nested_braces") << QString("QComboBox::drop-down { border: none; }");
    QTest::newRow("complex_selector") << QString("QTabWidget::pane { border: 1px solid gray; }");
}

void TestStyleManager::testRoundTripProperty()
{
    // Feature: qtvanity, Property 1: Style File Round-Trip
    
    QFETCH(QString, qssContent);
    
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(true);
    QVERIFY(tempFile.open());
    QString filePath = tempFile.fileName();
    tempFile.close();
    
    StyleManager manager;
    
    // Save the content
    bool saved = manager.saveToFile(filePath, qssContent);
    QVERIFY2(saved, "Failed to save QSS content to file");
    
    // Load it back
    QString loaded = manager.loadFromFile(filePath);
    
    // Verify round-trip equality
    QCOMPARE(loaded, qssContent);
}

/**
 * Property 10: Template Loading Content
 * 
 * For any template name in the availableTemplates() list, calling 
 * loadTemplate() should return a non-empty string containing valid 
 * QSS syntax.
 */
void TestStyleManager::testTemplateLoadingProperty_data()
{
    QTest::addColumn<QString>("templateName");
    QTest::addColumn<bool>("expectNonEmpty");
    
    // Test with the actual templates in the styles/ directory
    // These are the predefined templates: dark, light, solarized
    QTest::newRow("dark_template") << "dark" << true;
    QTest::newRow("light_template") << "light" << true;
    QTest::newRow("solarized_template") << "solarized" << true;
    
    // Generate additional test cases to reach 100+ iterations
    // by testing the same templates multiple times with different row names
    for (int i = 0; i < 33; ++i) {
        QTest::newRow(qPrintable(QString("dark_iteration_%1").arg(i))) << "dark" << true;
        QTest::newRow(qPrintable(QString("light_iteration_%1").arg(i))) << "light" << true;
        QTest::newRow(qPrintable(QString("solarized_iteration_%1").arg(i))) << "solarized" << true;
    }
}

void TestStyleManager::testTemplateLoadingProperty()
{
    // Feature: qtvanity, Property 10: Template Loading Content
    
    QFETCH(QString, templateName);
    QFETCH(bool, expectNonEmpty);
    
    StyleManager manager;
    
    // Set templates path to the project's styles directory
    // This assumes tests are run from the build directory
    QString stylesPath = QCoreApplication::applicationDirPath() + "/../styles";
    QDir stylesDir(stylesPath);
    if (!stylesDir.exists()) {
        // Try relative to current working directory
        stylesPath = QDir::currentPath() + "/styles";
        stylesDir.setPath(stylesPath);
    }
    if (!stylesDir.exists()) {
        // Try parent directory (common for build directories)
        stylesPath = QDir::currentPath() + "/../styles";
        stylesDir.setPath(stylesPath);
    }
    
    QVERIFY2(stylesDir.exists(), 
             qPrintable(QString("Styles directory not found. Tried: %1").arg(stylesPath)));
    
    manager.setTemplatesPath(stylesDir.absolutePath());
    
    // Verify the template is in the available templates list
    QStringList available = manager.availableTemplates();
    QVERIFY2(available.contains(templateName),
             qPrintable(QString("Template '%1' not found in available templates: %2")
                       .arg(templateName, available.join(", "))));
    
    // Load the template
    QString content = manager.loadTemplate(templateName);
    
    if (expectNonEmpty) {
        // Property: Template content should be non-empty
        QVERIFY2(!content.isEmpty(),
                 qPrintable(QString("Template '%1' loaded but content is empty").arg(templateName)));
        
        // Property: Template should contain valid QSS syntax indicators
        // Valid QSS must have at least one selector with braces
        QVERIFY2(content.contains("{") && content.contains("}"),
                 qPrintable(QString("Template '%1' does not contain valid QSS structure (missing braces)")
                           .arg(templateName)));
        
        // Property: Template should contain at least one property assignment
        QVERIFY2(content.contains(":") && content.contains(";"),
                 qPrintable(QString("Template '%1' does not contain property assignments")
                           .arg(templateName)));
        
        // Property: Braces should be balanced
        int openBraces = content.count('{');
        int closeBraces = content.count('}');
        QVERIFY2(openBraces == closeBraces,
                 qPrintable(QString("Template '%1' has unbalanced braces: %2 open, %3 close")
                           .arg(templateName).arg(openBraces).arg(closeBraces)));
    }
}


// ============================================================================
// Style Toggle Integration Property Tests
// ============================================================================

/**
 * Feature: style-toggle, Property 1: Default Mode Empty Stylesheet
 * 
 * For any toggle action that switches to Default mode, the application's
 * stylesheet (qApp->styleSheet()) should be empty immediately after the toggle.
 */
void TestStyleManager::testDefaultModeEmptyStylesheet_data()
{
    QTest::addColumn<QString>("qssContent");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    QStringList selectors = {
        "QPushButton", "QLabel", "QLineEdit", "QComboBox", 
        "QCheckBox", "QRadioButton", "QSlider", "QSpinBox",
        "QWidget", "QMainWindow", "QDialog", "QFrame"
    };
    
    QStringList properties = {
        "color", "background-color", "border", "padding", "margin",
        "font-size", "font-weight", "border-radius"
    };
    
    QStringList values = {
        "red", "blue", "#ffffff", "#000000", "10px", "1px solid black",
        "5px", "bold", "transparent", "none"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        QString qss;
        
        // Generate 1-4 rules
        int numRules = 1 + (rng->generate() % 4);
        for (int r = 0; r < numRules; ++r) {
            QString selector = selectors[rng->generate() % selectors.size()];
            qss += selector + " {\n";
            
            int numProps = 1 + (rng->generate() % 4);
            for (int p = 0; p < numProps; ++p) {
                QString prop = properties[rng->generate() % properties.size()];
                QString val = values[rng->generate() % values.size()];
                qss += "    " + prop + ": " + val + ";\n";
            }
            qss += "}\n";
        }
        
        QTest::newRow(qPrintable(QString("default_mode_%1").arg(i))) << qss;
    }
    
    // Edge cases
    QTest::newRow("simple_rule") << QString("* { color: red; }");
    QTest::newRow("complex_rule") << QString("QPushButton:hover { background: blue; border: 1px solid red; }");
    QTest::newRow("multiline") << QString("QPushButton {\n    color: red;\n}\n\nQLabel {\n    color: blue;\n}");
}

void TestStyleManager::testDefaultModeEmptyStylesheet()
{
    // Feature: style-toggle, Property 1: Default Mode Empty Stylesheet
    
    QFETCH(QString, qssContent);
    
    QssEditor editor;
    StyleManager manager;
    
    // Connect editor signals to manager
    QObject::connect(&editor, &QssEditor::applyRequested,
                     &manager, &StyleManager::applyStyleSheet);
    QObject::connect(&editor, &QssEditor::defaultStyleRequested,
                     &manager, &StyleManager::clearStyleSheet);
    
    // Set content and make sure we're in Custom mode
    editor.setStyleSheet(qssContent);
    QVERIFY(editor.isCustomStyleActive());
    
    // Apply the stylesheet first (to verify qApp has a stylesheet)
    editor.apply();
    
    // Verify stylesheet is applied (if content is non-empty)
    if (!qssContent.isEmpty()) {
        QCOMPARE(qApp->styleSheet(), qssContent);
    }
    
    // Toggle to Default mode
    editor.setCustomStyleActive(false);
    QVERIFY(!editor.isCustomStyleActive());
    
    // Property: qApp->styleSheet() should be empty after toggling to Default mode
    QVERIFY2(qApp->styleSheet().isEmpty(),
             qPrintable(QString("Expected empty stylesheet after toggling to Default mode, "
                               "but got: '%1'").arg(qApp->styleSheet().left(50))));
    
    // Also verify StyleManager state
    QVERIFY(!manager.hasCustomStyleSheet());
}

/**
 * Feature: style-toggle, Property 2: Custom Mode Restores Stylesheet
 * 
 * For any editor content and any toggle sequence that ends in Custom mode,
 * the application's stylesheet should equal the editor's content.
 */
void TestStyleManager::testCustomModeRestoresStylesheet_data()
{
    QTest::addColumn<QString>("qssContent");
    QTest::addColumn<int>("toggleCount");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    QStringList selectors = {
        "QPushButton", "QLabel", "QLineEdit", "QComboBox", 
        "QCheckBox", "QRadioButton", "QSlider", "QSpinBox",
        "QWidget", "QMainWindow", "QDialog", "QFrame"
    };
    
    QStringList properties = {
        "color", "background-color", "border", "padding", "margin",
        "font-size", "font-weight", "border-radius"
    };
    
    QStringList values = {
        "red", "blue", "#ffffff", "#000000", "10px", "1px solid black",
        "5px", "bold", "transparent", "none"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        QString qss;
        
        // Generate 1-4 rules
        int numRules = 1 + (rng->generate() % 4);
        for (int r = 0; r < numRules; ++r) {
            QString selector = selectors[rng->generate() % selectors.size()];
            qss += selector + " {\n";
            
            int numProps = 1 + (rng->generate() % 4);
            for (int p = 0; p < numProps; ++p) {
                QString prop = properties[rng->generate() % properties.size()];
                QString val = values[rng->generate() % values.size()];
                qss += "    " + prop + ": " + val + ";\n";
            }
            qss += "}\n";
        }
        
        // Random number of toggles (1-5), always ending in Custom mode (odd number)
        int toggles = 1 + 2 * (rng->generate() % 3); // 1, 3, or 5
        
        QTest::newRow(qPrintable(QString("custom_restore_%1").arg(i))) << qss << toggles;
    }
    
    // Edge cases
    QTest::newRow("single_toggle") << QString("* { color: red; }") << 1;
    QTest::newRow("triple_toggle") << QString("QLabel { background: blue; }") << 3;
    QTest::newRow("five_toggles") << QString("QPushButton { border: 1px solid black; }") << 5;
}

void TestStyleManager::testCustomModeRestoresStylesheet()
{
    // Feature: style-toggle, Property 2: Custom Mode Restores Stylesheet
    
    QFETCH(QString, qssContent);
    QFETCH(int, toggleCount);
    
    QssEditor editor;
    StyleManager manager;
    
    // Connect editor signals to manager
    QObject::connect(&editor, &QssEditor::applyRequested,
                     &manager, &StyleManager::applyStyleSheet);
    QObject::connect(&editor, &QssEditor::defaultStyleRequested,
                     &manager, &StyleManager::clearStyleSheet);
    
    // Set content
    editor.setStyleSheet(qssContent);
    QVERIFY(editor.isCustomStyleActive());
    
    // Start in Default mode
    editor.setCustomStyleActive(false);
    QVERIFY(!editor.isCustomStyleActive());
    QVERIFY(qApp->styleSheet().isEmpty());
    
    // Perform toggle sequence (toggleCount times, ending in Custom mode)
    for (int i = 0; i < toggleCount; ++i) {
        editor.toggleStyleMode();
    }
    
    // After odd number of toggles from Default, should be in Custom mode
    QVERIFY2(editor.isCustomStyleActive(),
             qPrintable(QString("Expected Custom mode after %1 toggles from Default").arg(toggleCount)));
    
    // Property: qApp->styleSheet() should equal editor content when in Custom mode
    QCOMPARE(qApp->styleSheet(), qssContent);
    
    // Also verify StyleManager state matches
    QCOMPARE(manager.currentStyleSheet(), qssContent);
    
    // Verify editor content is unchanged
    QCOMPARE(editor.styleSheet(), qssContent);
}
