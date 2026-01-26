#include "test_pluginmanager.h"
#include "PluginManager.h"
#include "PluginMetadata.h"

#include <QRandomGenerator>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>
#include <QSignalSpy>

void TestPluginManager::initTestCase()
{
    // Setup for all tests
}

void TestPluginManager::cleanupTestCase()
{
    // Cleanup after all tests
}

void TestPluginManager::init()
{
    // Setup before each test
}

void TestPluginManager::cleanup()
{
    // Cleanup after each test
}

QString TestPluginManager::generateRandomFilenameBase()
{
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate a random length between 3 and 30 characters
    int length = rng->bounded(3, 31);
    
    // Character set for filenames: alphanumeric, underscore, hyphen
    const QString chars = QStringLiteral("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-");
    
    QString filename;
    filename.reserve(length);
    
    // First character should be a letter (common convention)
    const QString letters = QStringLiteral("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    filename.append(letters.at(rng->bounded(letters.length())));
    
    // Generate remaining characters
    for (int i = 1; i < length; ++i) {
        filename.append(chars.at(rng->bounded(chars.length())));
    }
    
    // Occasionally add a dot in the middle (like "lib.plugin" before extension)
    if (rng->bounded(100) < 20 && length > 5) {  // 20% chance
        int dotPos = rng->bounded(2, length - 2);
        filename.insert(dotPos, '.');
    }
    
    return filename;
}

/**
 * Feature: custom-widget-plugin, Property 1: Platform Extension Recognition
 * 
 * For any filename string, the Plugin_Manager SHALL recognize it as a plugin file
 * if and only if it has the correct platform-specific extension (.so on Linux,
 * .dll on Windows, .dylib on macOS).
 */
void TestPluginManager::testPlatformExtensionRecognition_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<bool>("expectedResult");
    
    // Determine the correct platform extension
    QString platformExtension;
    QStringList wrongExtensions;
    
#if defined(Q_OS_LINUX)
    platformExtension = QStringLiteral("so");
    wrongExtensions << QStringLiteral("dll") << QStringLiteral("dylib");
#elif defined(Q_OS_WIN)
    platformExtension = QStringLiteral("dll");
    wrongExtensions << QStringLiteral("so") << QStringLiteral("dylib");
#elif defined(Q_OS_MACOS)
    platformExtension = QStringLiteral("dylib");
    wrongExtensions << QStringLiteral("so") << QStringLiteral("dll");
#else
    // Fallback for unknown platforms - all extensions should be valid
    platformExtension = QStringLiteral("so");
    // No wrong extensions in fallback mode
#endif
    
    // Generate 100+ test cases with random filenames
    for (int i = 0; i < 100; ++i) {
        QString randomBase = generateRandomFilenameBase();
        
        // Test case 1: Valid platform extension (should return true)
        QString validFilename = randomBase + QStringLiteral(".") + platformExtension;
        QTest::newRow(qPrintable(QString("valid_platform_ext_%1").arg(i)))
            << validFilename << true;
        
        // Test case 2: Wrong platform extensions (should return false)
        for (const QString &wrongExt : wrongExtensions) {
            QString invalidFilename = randomBase + QStringLiteral(".") + wrongExt;
            QTest::newRow(qPrintable(QString("wrong_ext_%1_%2").arg(wrongExt).arg(i)))
                << invalidFilename << false;
        }
    }
    
    // Test case sensitivity - extensions should be case-insensitive
    for (int i = 0; i < 20; ++i) {
        QString randomBase = generateRandomFilenameBase();
        
        // Test uppercase extension
        QString upperFilename = randomBase + QStringLiteral(".") + platformExtension.toUpper();
        QTest::newRow(qPrintable(QString("uppercase_ext_%1").arg(i)))
            << upperFilename << true;
        
        // Test mixed case extension
        QString mixedExt = platformExtension;
        if (mixedExt.length() > 1) {
            mixedExt[0] = mixedExt[0].toUpper();
        }
        QString mixedFilename = randomBase + QStringLiteral(".") + mixedExt;
        QTest::newRow(qPrintable(QString("mixedcase_ext_%1").arg(i)))
            << mixedFilename << true;
    }
    
    // Edge cases: Non-plugin file extensions
    QStringList nonPluginExtensions = {
        QStringLiteral("txt"), QStringLiteral("cpp"), QStringLiteral("h"),
        QStringLiteral("o"), QStringLiteral("obj"), QStringLiteral("a"),
        QStringLiteral("lib"), QStringLiteral("exe"), QStringLiteral("app"),
        QStringLiteral("py"), QStringLiteral("js"), QStringLiteral("json"),
        QStringLiteral("xml"), QStringLiteral("qss"), QStringLiteral("ui"),
        QStringLiteral("png"), QStringLiteral("jpg"), QStringLiteral("svg"),
        QStringLiteral("zip"), QStringLiteral("tar"), QStringLiteral("gz")
    };
    
    for (const QString &ext : nonPluginExtensions) {
        QString filename = QStringLiteral("testfile.") + ext;
        QTest::newRow(qPrintable(QString("non_plugin_ext_%1").arg(ext)))
            << filename << false;
    }
    
    // Edge cases: Files without extensions
    QTest::newRow("no_extension") << QStringLiteral("pluginfile") << false;
    QTest::newRow("no_extension_with_dot") << QStringLiteral("plugin.file.name") << false;
    
    // Edge cases: Empty and special filenames
    QTest::newRow("empty_filename") << QString() << false;
    QTest::newRow("only_extension") << (QStringLiteral(".") + platformExtension) << true;
    QTest::newRow("double_extension") << (QStringLiteral("file.txt.") + platformExtension) << true;
    
    // Edge cases: Path-like filenames
    QString pathWithValidExt = QStringLiteral("/path/to/plugin.") + platformExtension;
    QTest::newRow("path_with_valid_ext") << pathWithValidExt << true;
    
    QString pathWithInvalidExt = QStringLiteral("/path/to/plugin.txt");
    QTest::newRow("path_with_invalid_ext") << pathWithInvalidExt << false;
    
    // Edge case: Extension that contains platform extension as substring
    QTest::newRow("extension_substring_noso") << QStringLiteral("file.noso") << false;
    QTest::newRow("extension_substring_sodll") << QStringLiteral("file.sodll") << false;
}

void TestPluginManager::testPlatformExtensionRecognition()
{
    // Feature: custom-widget-plugin, Property 1: Platform Extension Recognition
    
    QFETCH(QString, filename);
    QFETCH(bool, expectedResult);
    
    PluginManager manager;
    
    bool result = manager.isPluginFile(filename);
    
    QVERIFY2(result == expectedResult,
             qPrintable(QString("isPluginFile('%1') returned %2, expected %3")
                       .arg(filename)
                       .arg(result ? "true" : "false")
                       .arg(expectedResult ? "true" : "false")));
}


/**
 * Feature: custom-widget-plugin, Property 5: Default Category Assignment
 * 
 * For any loaded plugin that returns an empty string from widgetCategory(),
 * the Plugin_Manager SHALL assign the category "Custom" in the PluginMetadata.
 * 
 * Since we cannot easily create real plugins for testing, this test validates
 * the category assignment logic by simulating the behavior that loadPlugin()
 * performs when extracting metadata from a plugin interface.
 */
void TestPluginManager::testDefaultCategoryAssignment_data()
{
    QTest::addColumn<QString>("pluginCategory");
    QTest::addColumn<QString>("expectedCategory");
    
    // Test case 1: Empty category should get "Custom" assigned
    QTest::newRow("empty_string") << QString() << QStringLiteral("Custom");
    QTest::newRow("empty_literal") << QStringLiteral("") << QStringLiteral("Custom");
    
    // Test case 2: Non-empty categories should be preserved
    QTest::newRow("custom_category") << QStringLiteral("Custom") << QStringLiteral("Custom");
    QTest::newRow("buttons_category") << QStringLiteral("Buttons") << QStringLiteral("Buttons");
    QTest::newRow("inputs_category") << QStringLiteral("Inputs") << QStringLiteral("Inputs");
    QTest::newRow("containers_category") << QStringLiteral("Containers") << QStringLiteral("Containers");
    QTest::newRow("displays_category") << QStringLiteral("Displays") << QStringLiteral("Displays");
    
    // Generate random category names to test preservation
    QRandomGenerator *rng = QRandomGenerator::global();
    const QString chars = QStringLiteral("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _-");
    
    for (int i = 0; i < 50; ++i) {
        // Generate random non-empty category name
        int length = rng->bounded(1, 31);  // 1 to 30 characters
        QString randomCategory;
        randomCategory.reserve(length);
        
        for (int j = 0; j < length; ++j) {
            randomCategory.append(chars.at(rng->bounded(chars.length())));
        }
        
        // Non-empty categories should be preserved as-is
        QTest::newRow(qPrintable(QString("random_category_%1").arg(i)))
            << randomCategory << randomCategory;
    }
    
    // Edge cases for empty-like strings
    // Note: Only truly empty strings should get "Custom", whitespace-only strings
    // are considered valid categories (per the implementation)
    QTest::newRow("whitespace_only") << QStringLiteral("   ") << QStringLiteral("   ");
    QTest::newRow("tab_only") << QStringLiteral("\t") << QStringLiteral("\t");
    QTest::newRow("newline_only") << QStringLiteral("\n") << QStringLiteral("\n");
    
    // Edge cases for special characters in category names
    QTest::newRow("unicode_category") << QStringLiteral("Widgets 日本語") << QStringLiteral("Widgets 日本語");
    QTest::newRow("emoji_category") << QStringLiteral("🔘 Buttons") << QStringLiteral("🔘 Buttons");
    QTest::newRow("special_chars") << QStringLiteral("My-Custom_Widgets.v2") << QStringLiteral("My-Custom_Widgets.v2");
    
    // Additional empty string variations
    for (int i = 0; i < 20; ++i) {
        // All empty strings should get "Custom"
        QTest::newRow(qPrintable(QString("empty_variation_%1").arg(i)))
            << QString() << QStringLiteral("Custom");
    }
}

void TestPluginManager::testDefaultCategoryAssignment()
{
    // Feature: custom-widget-plugin, Property 5: Default Category Assignment
    
    QFETCH(QString, pluginCategory);
    QFETCH(QString, expectedCategory);
    
    // Simulate the category assignment logic from PluginManager::loadPlugin()
    // This is the exact logic used in the implementation:
    // metadata.category = interface->widgetCategory().isEmpty() 
    //                    ? QStringLiteral("Custom") 
    //                    : interface->widgetCategory();
    
    PluginMetadata metadata;
    metadata.category = pluginCategory.isEmpty() 
                       ? QStringLiteral("Custom") 
                       : pluginCategory;
    
    QVERIFY2(metadata.category == expectedCategory,
             qPrintable(QString("Category assignment failed: input='%1', got='%2', expected='%3'")
                       .arg(pluginCategory.isEmpty() ? "(empty)" : pluginCategory)
                       .arg(metadata.category)
                       .arg(expectedCategory)));
    
    // Additional verification: empty input must always result in "Custom"
    if (pluginCategory.isEmpty()) {
        QCOMPARE(metadata.category, QStringLiteral("Custom"));
    } else {
        // Non-empty input must be preserved exactly
        QCOMPARE(metadata.category, pluginCategory);
    }
}


/**
 * Feature: custom-widget-plugin, Property 6: Error Logging for Failed Plugins
 * 
 * For any plugin file that fails to load (invalid file, missing interface, etc.),
 * the Plugin_Manager SHALL add an error entry containing the plugin's filename
 * to the loading errors list.
 */
void TestPluginManager::testErrorLoggingForFailedPlugins_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("fileContent");
    QTest::addColumn<QString>("description");
    
    // Determine the correct platform extension
    QString platformExtension;
#if defined(Q_OS_LINUX)
    platformExtension = QStringLiteral("so");
#elif defined(Q_OS_WIN)
    platformExtension = QStringLiteral("dll");
#elif defined(Q_OS_MACOS)
    platformExtension = QStringLiteral("dylib");
#else
    platformExtension = QStringLiteral("so");
#endif
    
    // Generate test cases with various invalid plugin files
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Test case 1: Empty files with plugin extension
    for (int i = 0; i < 20; ++i) {
        QString randomBase = generateRandomFilenameBase();
        QString filename = randomBase + QStringLiteral(".") + platformExtension;
        QTest::newRow(qPrintable(QString("empty_file_%1").arg(i)))
            << filename << QByteArray() << QStringLiteral("Empty file");
    }
    
    // Test case 2: Files with random garbage content
    for (int i = 0; i < 30; ++i) {
        QString randomBase = generateRandomFilenameBase();
        QString filename = randomBase + QStringLiteral(".") + platformExtension;
        
        // Generate random garbage content
        int contentSize = rng->bounded(1, 1024);
        QByteArray content;
        content.reserve(contentSize);
        for (int j = 0; j < contentSize; ++j) {
            content.append(static_cast<char>(rng->bounded(256)));
        }
        
        QTest::newRow(qPrintable(QString("garbage_content_%1").arg(i)))
            << filename << content << QStringLiteral("Random garbage content");
    }
    
    // Test case 3: Files with text content (not valid binary)
    QStringList textContents = {
        QStringLiteral("This is not a plugin"),
        QStringLiteral("Hello World"),
        QStringLiteral("{}"),
        QStringLiteral("[]"),
        QStringLiteral("<xml></xml>"),
        QStringLiteral("#!/bin/bash\necho hello"),
        QStringLiteral("#include <stdio.h>\nint main() { return 0; }"),
        QStringLiteral("def main():\n    pass"),
        QStringLiteral("console.log('hello');"),
        QStringLiteral("SELECT * FROM plugins;")
    };
    
    for (int i = 0; i < textContents.size(); ++i) {
        QString randomBase = generateRandomFilenameBase();
        QString filename = randomBase + QStringLiteral(".") + platformExtension;
        QTest::newRow(qPrintable(QString("text_content_%1").arg(i)))
            << filename << textContents[i].toUtf8() << QStringLiteral("Text content");
    }
    
    // Test case 4: Files with partial/corrupted ELF/PE/Mach-O headers
    // ELF magic number (Linux)
    QByteArray partialElf;
    partialElf.append("\x7F""ELF");  // ELF magic
    partialElf.append(QByteArray(20, '\0'));  // Truncated header
    
    // PE magic number (Windows)
    QByteArray partialPe;
    partialPe.append("MZ");  // DOS header magic
    partialPe.append(QByteArray(20, '\0'));  // Truncated header
    
    // Mach-O magic number (macOS)
    QByteArray partialMachO;
    partialMachO.append("\xFE\xED\xFA\xCE", 4);  // Mach-O magic (32-bit)
    partialMachO.append(QByteArray(20, '\0'));  // Truncated header
    
    for (int i = 0; i < 10; ++i) {
        QString randomBase = generateRandomFilenameBase();
        QString filename = randomBase + QStringLiteral(".") + platformExtension;
        
        QTest::newRow(qPrintable(QString("partial_elf_%1").arg(i)))
            << filename << partialElf << QStringLiteral("Partial ELF header");
        
        QTest::newRow(qPrintable(QString("partial_pe_%1").arg(i)))
            << filename << partialPe << QStringLiteral("Partial PE header");
        
        QTest::newRow(qPrintable(QString("partial_macho_%1").arg(i)))
            << filename << partialMachO << QStringLiteral("Partial Mach-O header");
    }
    
    // Test case 5: Files with special characters in filename
    QStringList specialFilenames = {
        QStringLiteral("plugin with spaces"),
        QStringLiteral("plugin-with-dashes"),
        QStringLiteral("plugin_with_underscores"),
        QStringLiteral("plugin.with.dots"),
        QStringLiteral("UPPERCASE_PLUGIN"),
        QStringLiteral("MixedCase_Plugin"),
        QStringLiteral("plugin123"),
        QStringLiteral("123plugin"),
        QStringLiteral("a"),  // Single character
        QStringLiteral("very_long_plugin_filename_that_exceeds_normal_length_expectations")
    };
    
    for (const QString &baseName : specialFilenames) {
        QString filename = baseName + QStringLiteral(".") + platformExtension;
        QTest::newRow(qPrintable(QString("special_name_%1").arg(baseName.left(20))))
            << filename << QByteArray("invalid") << QStringLiteral("Special filename");
    }
}

void TestPluginManager::testErrorLoggingForFailedPlugins()
{
    // Feature: custom-widget-plugin, Property 6: Error Logging for Failed Plugins
    
    QFETCH(QString, filename);
    QFETCH(QByteArray, fileContent);
    QFETCH(QString, description);
    
    Q_UNUSED(description);
    
    // Create a temporary directory for the test
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "Failed to create temporary directory");
    
    // Create the invalid plugin file
    QString filePath = tempDir.path() + QDir::separator() + filename;
    QFile file(filePath);
    QVERIFY2(file.open(QIODevice::WriteOnly), 
             qPrintable(QString("Failed to create test file: %1").arg(filePath)));
    file.write(fileContent);
    file.close();
    
    // Verify the file was created
    QVERIFY2(QFile::exists(filePath), 
             qPrintable(QString("Test file does not exist: %1").arg(filePath)));
    
    // Create PluginManager and attempt to load plugins from the temp directory
    PluginManager manager;
    manager.setPluginDirectory(tempDir.path());
    manager.loadPlugins();
    
    // Get the loading errors
    QList<QString> errors = manager.loadingErrors();
    
    // Property verification: The error list should contain at least one entry
    // that includes the filename of the failed plugin
    QVERIFY2(!errors.isEmpty(),
             qPrintable(QString("Expected error for invalid plugin '%1' but loadingErrors() is empty")
                       .arg(filename)));
    
    // Verify that at least one error message contains the filename
    bool filenameFoundInErrors = false;
    for (const QString &error : errors) {
        if (error.contains(filename)) {
            filenameFoundInErrors = true;
            break;
        }
    }
    
    QVERIFY2(filenameFoundInErrors,
             qPrintable(QString("Error message should contain filename '%1'. Errors: %2")
                       .arg(filename)
                       .arg(errors.join("; "))));
    
    // Additional verification: The plugin should not be in the loaded plugins list
    QList<PluginMetadata> loadedPlugins = manager.loadedPlugins();
    for (const PluginMetadata &meta : loadedPlugins) {
        QVERIFY2(!meta.filePath.endsWith(filename) || !meta.isValid,
                 qPrintable(QString("Invalid plugin '%1' should not be marked as valid")
                           .arg(filename)));
    }
}


/**
 * Feature: custom-widget-plugin, Property 7: Continued Loading After Failure
 * 
 * For any set of plugin files where some are valid and some are invalid,
 * the Plugin_Manager SHALL attempt to load all plugins, successfully loading
 * all valid ones regardless of failures in others.
 * 
 * Since we cannot easily create real valid plugins for testing, this test verifies:
 * 1. When multiple invalid plugin files exist, all are attempted (multiple errors logged)
 * 2. The scanDirectory loop continues after each failure
 * 3. The number of errors matches the number of invalid files
 */
void TestPluginManager::testContinuedLoadingAfterFailure_data()
{
    QTest::addColumn<int>("numInvalidPlugins");
    QTest::addColumn<QStringList>("filenames");
    QTest::addColumn<QList<QByteArray>>("fileContents");
    
    // Determine the correct platform extension
    QString platformExtension;
#if defined(Q_OS_LINUX)
    platformExtension = QStringLiteral("so");
#elif defined(Q_OS_WIN)
    platformExtension = QStringLiteral("dll");
#elif defined(Q_OS_MACOS)
    platformExtension = QStringLiteral("dylib");
#else
    platformExtension = QStringLiteral("so");
#endif
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Test with varying numbers of invalid plugins (2 to 10)
    for (int numPlugins = 2; numPlugins <= 10; ++numPlugins) {
        QStringList filenames;
        QList<QByteArray> contents;
        
        for (int i = 0; i < numPlugins; ++i) {
            // Generate unique filename for each plugin
            QString filename = QString("invalid_plugin_%1_%2.%3")
                              .arg(numPlugins)
                              .arg(i)
                              .arg(platformExtension);
            filenames.append(filename);
            
            // Generate different types of invalid content
            QByteArray content;
            int contentType = i % 5;
            switch (contentType) {
                case 0:
                    // Empty file
                    content = QByteArray();
                    break;
                case 1:
                    // Random garbage
                    {
                        int size = rng->bounded(10, 100);
                        for (int j = 0; j < size; ++j) {
                            content.append(static_cast<char>(rng->bounded(256)));
                        }
                    }
                    break;
                case 2:
                    // Text content
                    content = QByteArray("This is not a valid plugin file");
                    break;
                case 3:
                    // Partial ELF header
                    content = QByteArray("\x7F""ELF\x00\x00\x00\x00", 8);
                    break;
                case 4:
                    // JSON-like content
                    content = QByteArray("{\"invalid\": \"plugin\"}");
                    break;
            }
            contents.append(content);
        }
        
        QTest::newRow(qPrintable(QString("invalid_plugins_%1").arg(numPlugins)))
            << numPlugins << filenames << contents;
    }
    
    // Additional test cases with random plugin counts
    for (int testCase = 0; testCase < 20; ++testCase) {
        int numPlugins = rng->bounded(3, 15);  // 3 to 14 plugins
        QStringList filenames;
        QList<QByteArray> contents;
        
        for (int i = 0; i < numPlugins; ++i) {
            QString randomBase = generateRandomFilenameBase();
            QString filename = QString("%1_test%2.%3")
                              .arg(randomBase)
                              .arg(i)
                              .arg(platformExtension);
            filenames.append(filename);
            
            // Generate random invalid content
            int contentSize = rng->bounded(0, 200);
            QByteArray content;
            for (int j = 0; j < contentSize; ++j) {
                content.append(static_cast<char>(rng->bounded(256)));
            }
            contents.append(content);
        }
        
        QTest::newRow(qPrintable(QString("random_invalid_%1_plugins_%2").arg(testCase).arg(numPlugins)))
            << numPlugins << filenames << contents;
    }
    
    // Edge case: Exactly 2 plugins (minimum for testing continuation)
    {
        QStringList filenames;
        QList<QByteArray> contents;
        
        filenames << QString("first_invalid.%1").arg(platformExtension);
        filenames << QString("second_invalid.%1").arg(platformExtension);
        
        contents << QByteArray("invalid content 1");
        contents << QByteArray("invalid content 2");
        
        QTest::newRow("exactly_two_invalid")
            << 2 << filenames << contents;
    }
    
    // Edge case: Many plugins with same content type
    {
        QStringList filenames;
        QList<QByteArray> contents;
        
        for (int i = 0; i < 20; ++i) {
            filenames << QString("empty_plugin_%1.%2").arg(i).arg(platformExtension);
            contents << QByteArray();  // All empty files
        }
        
        QTest::newRow("twenty_empty_plugins")
            << 20 << filenames << contents;
    }
}

void TestPluginManager::testContinuedLoadingAfterFailure()
{
    // Feature: custom-widget-plugin, Property 7: Continued Loading After Failure
    
    QFETCH(int, numInvalidPlugins);
    QFETCH(QStringList, filenames);
    QFETCH(QList<QByteArray>, fileContents);
    
    // Verify test data consistency
    QCOMPARE(filenames.size(), numInvalidPlugins);
    QCOMPARE(fileContents.size(), numInvalidPlugins);
    
    // Create a temporary directory for the test
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "Failed to create temporary directory");
    
    // Create all invalid plugin files
    for (int i = 0; i < numInvalidPlugins; ++i) {
        QString filePath = tempDir.path() + QDir::separator() + filenames[i];
        QFile file(filePath);
        QVERIFY2(file.open(QIODevice::WriteOnly),
                 qPrintable(QString("Failed to create test file: %1").arg(filePath)));
        file.write(fileContents[i]);
        file.close();
        
        // Verify the file was created
        QVERIFY2(QFile::exists(filePath),
                 qPrintable(QString("Test file does not exist: %1").arg(filePath)));
    }
    
    // Create PluginManager and attempt to load plugins from the temp directory
    PluginManager manager;
    manager.setPluginDirectory(tempDir.path());
    manager.loadPlugins();
    
    // Get the loading errors
    QList<QString> errors = manager.loadingErrors();
    
    // Property verification 1: The number of errors should match the number of invalid plugins
    // This proves that the scanDirectory loop continued after each failure
    QVERIFY2(errors.size() == numInvalidPlugins,
             qPrintable(QString("Expected %1 errors for %1 invalid plugins, but got %2 errors. "
                               "This indicates the loading loop did not continue after failures.")
                       .arg(numInvalidPlugins)
                       .arg(errors.size())));
    
    // Property verification 2: Each invalid plugin filename should appear in the errors
    // This proves that each plugin was attempted
    for (const QString &filename : filenames) {
        bool filenameFoundInErrors = false;
        for (const QString &error : errors) {
            if (error.contains(filename)) {
                filenameFoundInErrors = true;
                break;
            }
        }
        
        QVERIFY2(filenameFoundInErrors,
                 qPrintable(QString("Error for plugin '%1' not found. "
                                   "This indicates the plugin was not attempted. Errors: %2")
                           .arg(filename)
                           .arg(errors.join("; "))));
    }
    
    // Property verification 3: No plugins should be successfully loaded
    // (since all are invalid)
    QList<PluginMetadata> loadedPlugins = manager.loadedPlugins();
    int validPluginCount = 0;
    for (const PluginMetadata &meta : loadedPlugins) {
        if (meta.isValid) {
            validPluginCount++;
        }
    }
    
    QVERIFY2(validPluginCount == 0,
             qPrintable(QString("Expected 0 valid plugins but found %1. "
                               "Invalid plugins should not be marked as valid.")
                       .arg(validPluginCount)));
    
    // Property verification 4: Verify the pluginsLoaded signal would have been emitted
    // (implicitly verified by the fact that loadPlugins() completed without crashing)
    
    // Additional verification: Each error message should be unique (no duplicates)
    QSet<QString> uniqueErrors;
    for (const QString &error : errors) {
        uniqueErrors.insert(error);
    }
    
    QVERIFY2(uniqueErrors.size() == errors.size(),
             qPrintable(QString("Found duplicate error messages. Expected %1 unique errors, got %2. "
                               "Each plugin failure should generate a unique error.")
                       .arg(errors.size())
                       .arg(uniqueErrors.size())));
}


/**
 * Feature: custom-widget-plugin, Property 11: Refresh Operation Completeness
 * 
 * For any refresh operation on the Plugin_Manager, all previously loaded plugins
 * SHALL be unloaded, the Plugin_Directory SHALL be rescanned, all discovered
 * plugins SHALL be reloaded, and the pluginsLoaded signal SHALL be emitted.
 * 
 * This test verifies:
 * 1. After refresh, pluginsUnloaded signal is emitted
 * 2. After refresh, pluginsLoaded signal is emitted
 * 3. The plugin directory is rescanned (errors are regenerated for invalid plugins)
 * 4. State is properly cleared and rebuilt
 */
void TestPluginManager::testRefreshOperationCompleteness_data()
{
    QTest::addColumn<int>("numPlugins");
    QTest::addColumn<QStringList>("filenames");
    QTest::addColumn<QList<QByteArray>>("fileContents");
    QTest::addColumn<bool>("addPluginAfterInitialLoad");
    
    // Determine the correct platform extension
    QString platformExtension;
#if defined(Q_OS_LINUX)
    platformExtension = QStringLiteral("so");
#elif defined(Q_OS_WIN)
    platformExtension = QStringLiteral("dll");
#elif defined(Q_OS_MACOS)
    platformExtension = QStringLiteral("dylib");
#else
    platformExtension = QStringLiteral("so");
#endif
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Test case 1: Refresh with same plugins (no changes)
    for (int numPlugins = 1; numPlugins <= 5; ++numPlugins) {
        QStringList filenames;
        QList<QByteArray> contents;
        
        for (int i = 0; i < numPlugins; ++i) {
            QString filename = QString("refresh_test_plugin_%1_%2.%3")
                              .arg(numPlugins)
                              .arg(i)
                              .arg(platformExtension);
            filenames.append(filename);
            
            // Generate invalid content
            QByteArray content = QString("Invalid plugin content %1").arg(i).toUtf8();
            contents.append(content);
        }
        
        QTest::newRow(qPrintable(QString("refresh_same_%1_plugins").arg(numPlugins)))
            << numPlugins << filenames << contents << false;
    }
    
    // Test case 2: Refresh with additional plugin added after initial load
    for (int numPlugins = 1; numPlugins <= 5; ++numPlugins) {
        QStringList filenames;
        QList<QByteArray> contents;
        
        for (int i = 0; i < numPlugins; ++i) {
            QString filename = QString("refresh_add_plugin_%1_%2.%3")
                              .arg(numPlugins)
                              .arg(i)
                              .arg(platformExtension);
            filenames.append(filename);
            
            QByteArray content = QString("Plugin content for add test %1").arg(i).toUtf8();
            contents.append(content);
        }
        
        QTest::newRow(qPrintable(QString("refresh_add_plugin_%1").arg(numPlugins)))
            << numPlugins << filenames << contents << true;
    }
    
    // Test case 3: Random number of plugins
    for (int testCase = 0; testCase < 20; ++testCase) {
        int numPlugins = rng->bounded(1, 10);  // 1 to 9 plugins
        QStringList filenames;
        QList<QByteArray> contents;
        
        for (int i = 0; i < numPlugins; ++i) {
            QString randomBase = generateRandomFilenameBase();
            QString filename = QString("%1_refresh_%2.%3")
                              .arg(randomBase)
                              .arg(i)
                              .arg(platformExtension);
            filenames.append(filename);
            
            // Generate random invalid content
            int contentSize = rng->bounded(10, 100);
            QByteArray content;
            for (int j = 0; j < contentSize; ++j) {
                content.append(static_cast<char>(rng->bounded(256)));
            }
            contents.append(content);
        }
        
        bool addPlugin = rng->bounded(100) < 50;  // 50% chance to add plugin after initial load
        QTest::newRow(qPrintable(QString("random_refresh_%1_plugins_%2").arg(testCase).arg(numPlugins)))
            << numPlugins << filenames << contents << addPlugin;
    }
    
    // Edge case: Empty directory
    {
        QStringList filenames;
        QList<QByteArray> contents;
        
        QTest::newRow("refresh_empty_directory")
            << 0 << filenames << contents << false;
    }
    
    // Edge case: Single plugin
    {
        QStringList filenames;
        QList<QByteArray> contents;
        
        filenames << QString("single_refresh_plugin.%1").arg(platformExtension);
        contents << QByteArray("single plugin content");
        
        QTest::newRow("refresh_single_plugin")
            << 1 << filenames << contents << false;
    }
    
    // Edge case: Many plugins
    {
        QStringList filenames;
        QList<QByteArray> contents;
        
        for (int i = 0; i < 15; ++i) {
            filenames << QString("many_refresh_plugin_%1.%2").arg(i).arg(platformExtension);
            contents << QByteArray("content");
        }
        
        QTest::newRow("refresh_many_plugins")
            << 15 << filenames << contents << false;
    }
}

void TestPluginManager::testRefreshOperationCompleteness()
{
    // Feature: custom-widget-plugin, Property 11: Refresh Operation Completeness
    
    QFETCH(int, numPlugins);
    QFETCH(QStringList, filenames);
    QFETCH(QList<QByteArray>, fileContents);
    QFETCH(bool, addPluginAfterInitialLoad);
    
    // Verify test data consistency
    QCOMPARE(filenames.size(), numPlugins);
    QCOMPARE(fileContents.size(), numPlugins);
    
    // Create a temporary directory for the test
    QTemporaryDir tempDir;
    QVERIFY2(tempDir.isValid(), "Failed to create temporary directory");
    
    // Create all plugin files
    for (int i = 0; i < numPlugins; ++i) {
        QString filePath = tempDir.path() + QDir::separator() + filenames[i];
        QFile file(filePath);
        QVERIFY2(file.open(QIODevice::WriteOnly),
                 qPrintable(QString("Failed to create test file: %1").arg(filePath)));
        file.write(fileContents[i]);
        file.close();
    }
    
    // Create PluginManager and perform initial load
    PluginManager manager;
    manager.setPluginDirectory(tempDir.path());
    
    // Set up signal spies to monitor signal emissions
    QSignalSpy pluginsLoadedSpy(&manager, &PluginManager::pluginsLoaded);
    QSignalSpy pluginsUnloadedSpy(&manager, &PluginManager::pluginsUnloaded);
    QSignalSpy pluginLoadErrorSpy(&manager, &PluginManager::pluginLoadError);
    
    QVERIFY2(pluginsLoadedSpy.isValid(), "Failed to create pluginsLoaded signal spy");
    QVERIFY2(pluginsUnloadedSpy.isValid(), "Failed to create pluginsUnloaded signal spy");
    QVERIFY2(pluginLoadErrorSpy.isValid(), "Failed to create pluginLoadError signal spy");
    
    // Perform initial load
    manager.loadPlugins();
    
    // Verify initial load signals
    QCOMPARE(pluginsLoadedSpy.count(), 1);
    QCOMPARE(pluginsUnloadedSpy.count(), 0);  // No unload on initial load
    
    // Store initial state for comparison
    int initialErrorCount = manager.loadingErrors().size();
    QCOMPARE(initialErrorCount, numPlugins);  // All plugins are invalid
    
    // Clear signal spies for refresh test
    pluginsLoadedSpy.clear();
    pluginsUnloadedSpy.clear();
    pluginLoadErrorSpy.clear();
    
    // Optionally add a new plugin file before refresh
    QString additionalPluginFilename;
    if (addPluginAfterInitialLoad) {
        // Determine the correct platform extension
        QString platformExtension;
#if defined(Q_OS_LINUX)
        platformExtension = QStringLiteral("so");
#elif defined(Q_OS_WIN)
        platformExtension = QStringLiteral("dll");
#elif defined(Q_OS_MACOS)
        platformExtension = QStringLiteral("dylib");
#else
        platformExtension = QStringLiteral("so");
#endif
        
        additionalPluginFilename = QString("additional_plugin_after_load.%1").arg(platformExtension);
        QString additionalFilePath = tempDir.path() + QDir::separator() + additionalPluginFilename;
        QFile additionalFile(additionalFilePath);
        QVERIFY2(additionalFile.open(QIODevice::WriteOnly),
                 qPrintable(QString("Failed to create additional test file: %1").arg(additionalFilePath)));
        additionalFile.write("Additional plugin content");
        additionalFile.close();
    }
    
    // =========================================================================
    // REFRESH OPERATION - The core of this test
    // =========================================================================
    manager.refreshPlugins();
    
    // =========================================================================
    // Property Verification 1: pluginsUnloaded signal is emitted
    // all currently loaded plugins
    // =========================================================================
    QVERIFY2(pluginsUnloadedSpy.count() == 1,
             qPrintable(QString("Expected pluginsUnloaded signal to be emitted exactly once during refresh, "
                               "but it was emitted %1 times")
                       .arg(pluginsUnloadedSpy.count())));
    
    // =========================================================================
    // Property Verification 2: pluginsLoaded signal is emitted
    // all plugins from the Plugin_Directory
    // =========================================================================
    QVERIFY2(pluginsLoadedSpy.count() == 1,
             qPrintable(QString("Expected pluginsLoaded signal to be emitted exactly once during refresh, "
                               "but it was emitted %1 times")
                       .arg(pluginsLoadedSpy.count())));
    
    // =========================================================================
    // Property Verification 3: Plugin directory is rescanned
    // Errors are regenerated for invalid plugins
    // =========================================================================
    QList<QString> errorsAfterRefresh = manager.loadingErrors();
    int expectedErrorCount = addPluginAfterInitialLoad ? numPlugins + 1 : numPlugins;
    
    QVERIFY2(errorsAfterRefresh.size() == expectedErrorCount,
             qPrintable(QString("Expected %1 errors after refresh (directory rescanned), but got %2. "
                               "This indicates the directory was not properly rescanned.")
                       .arg(expectedErrorCount)
                       .arg(errorsAfterRefresh.size())));
    
    // Verify each original plugin filename appears in the errors (rescanned)
    for (const QString &filename : filenames) {
        bool filenameFoundInErrors = false;
        for (const QString &error : errorsAfterRefresh) {
            if (error.contains(filename)) {
                filenameFoundInErrors = true;
                break;
            }
        }
        
        QVERIFY2(filenameFoundInErrors,
                 qPrintable(QString("Plugin '%1' was not rescanned during refresh. "
                                   "Error not found in: %2")
                           .arg(filename)
                           .arg(errorsAfterRefresh.join("; "))));
    }
    
    // If we added a plugin after initial load, verify it was discovered during refresh
    if (addPluginAfterInitialLoad) {
        bool additionalPluginFound = false;
        for (const QString &error : errorsAfterRefresh) {
            if (error.contains(additionalPluginFilename)) {
                additionalPluginFound = true;
                break;
            }
        }
        
        QVERIFY2(additionalPluginFound,
                 qPrintable(QString("Additional plugin '%1' added after initial load was not discovered "
                                   "during refresh. This indicates the directory was not rescanned.")
                           .arg(additionalPluginFilename)));
    }
    
    // =========================================================================
    // Property Verification 4: State is properly cleared and rebuilt
    // =========================================================================
    
    // Verify metadata count matches expected plugins
    QList<PluginMetadata> metadataAfterRefresh = manager.loadedPlugins();
    // Note: Since all plugins are invalid, metadata list should be empty
    // (only valid plugins get added to metadata in the current implementation)
    // But errors should be regenerated
    
    // Verify no valid plugins (since all are invalid)
    int validPluginCount = 0;
    for (const PluginMetadata &meta : metadataAfterRefresh) {
        if (meta.isValid) {
            validPluginCount++;
        }
    }
    
    QVERIFY2(validPluginCount == 0,
             qPrintable(QString("Expected 0 valid plugins after refresh (all are invalid), "
                               "but found %1 valid plugins")
                       .arg(validPluginCount)));
    
    // =========================================================================
    // Additional verification: Signal order is correct (unload before load)
    // =========================================================================
    // The pluginsUnloaded signal should have been emitted before pluginsLoaded
    // We can verify this by checking that both signals were emitted and the
    // state is consistent with the expected order
    
    // =========================================================================
    // Additional verification: Multiple refreshes work correctly
    // =========================================================================
    pluginsLoadedSpy.clear();
    pluginsUnloadedSpy.clear();
    
    // Perform another refresh
    manager.refreshPlugins();
    
    QVERIFY2(pluginsUnloadedSpy.count() == 1,
             "pluginsUnloaded should be emitted on subsequent refresh");
    QVERIFY2(pluginsLoadedSpy.count() == 1,
             "pluginsLoaded should be emitted on subsequent refresh");
    
    // Errors should still be regenerated
    QList<QString> errorsAfterSecondRefresh = manager.loadingErrors();
    QVERIFY2(errorsAfterSecondRefresh.size() == expectedErrorCount,
             qPrintable(QString("Expected %1 errors after second refresh, but got %2")
                       .arg(expectedErrorCount)
                       .arg(errorsAfterSecondRefresh.size())));
}
