#include "test_stylemanager.h"
#include "StyleManager.h"

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>

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
