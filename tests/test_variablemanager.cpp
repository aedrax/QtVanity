#include "test_variablemanager.h"
#include "VariableManager.h"

#include <QRandomGenerator>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

void TestVariableManager::initTestCase()
{
    // Setup code if needed
}

void TestVariableManager::cleanupTestCase()
{
    // Cleanup code if needed
}

// =============================================================================
// Property 1: Variable Name Validation
// Feature: qss-variables, Property 1: Variable Name Validation
// =============================================================================

void TestVariableManager::testVariableNameValidationProperty_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("expectedValid");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Characters that are valid at the start of a variable name
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    // Characters that are valid in the rest of a variable name
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    // Invalid characters for testing
    const QString invalidChars = "!@#$%^&*()+=[]{}|\\:;\"'<>,./? \t\n";
    
    // Generate valid variable names (should return true)
    for (int i = 0; i < 50; ++i) {
        QString name;
        // Start with a valid start character
        name += validStartChars[rng->bounded(validStartChars.length())];
        
        // Add 0-20 more valid characters
        int length = rng->bounded(21);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        
        QTest::newRow(qPrintable(QString("valid_%1").arg(i))) << name << true;
    }
    
    // Generate invalid names starting with a digit (should return false)
    for (int i = 0; i < 15; ++i) {
        QString name;
        name += QString::number(rng->bounded(10)); // Start with digit
        int length = 1 + rng->bounded(10);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        QTest::newRow(qPrintable(QString("starts_with_digit_%1").arg(i))) << name << false;
    }

    // Generate invalid names starting with hyphen (should return false)
    for (int i = 0; i < 10; ++i) {
        QString name = "-";
        int length = 1 + rng->bounded(10);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        QTest::newRow(qPrintable(QString("starts_with_hyphen_%1").arg(i))) << name << false;
    }
    
    // Generate invalid names containing invalid characters (should return false)
    for (int i = 0; i < 15; ++i) {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = 1 + rng->bounded(5);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        // Insert an invalid character
        name += invalidChars[rng->bounded(invalidChars.length())];
        name += validChars[rng->bounded(validChars.length())];
        
        QTest::newRow(qPrintable(QString("contains_invalid_%1").arg(i))) << name << false;
    }
    
    // Edge cases
    QTest::newRow("empty_string") << QString("") << false;
    QTest::newRow("single_letter") << QString("a") << true;
    QTest::newRow("single_underscore") << QString("_") << true;
    QTest::newRow("underscore_start") << QString("_myVar") << true;
    QTest::newRow("with_hyphen") << QString("my-variable") << true;
    QTest::newRow("with_numbers") << QString("var123") << true;
    QTest::newRow("all_caps") << QString("MY_VARIABLE") << true;
    QTest::newRow("mixed_case") << QString("myVariable") << true;
    QTest::newRow("complex_valid") << QString("_my-Var_123") << true;
    QTest::newRow("space_in_middle") << QString("my var") << false;
    QTest::newRow("starts_with_space") << QString(" myvar") << false;
    QTest::newRow("only_digits") << QString("123") << false;
    QTest::newRow("special_chars") << QString("my@var") << false;
}

void TestVariableManager::testVariableNameValidationProperty()
{
    // Feature: qss-variables, Property 1: Variable Name Validation
    
    QFETCH(QString, name);
    QFETCH(bool, expectedValid);
    
    bool actualValid = VariableManager::isValidVariableName(name);
    
    QVERIFY2(actualValid == expectedValid,
             qPrintable(QString("Variable name '%1': expected %2, got %3")
                       .arg(name)
                       .arg(expectedValid ? "valid" : "invalid")
                       .arg(actualValid ? "valid" : "invalid")));
}

// =============================================================================
// Property 4: Color Value Detection
// Feature: qss-variables, Property 4: Color Value Detection
// =============================================================================

void TestVariableManager::testColorValueDetectionProperty_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedIsColor");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    const QString hexChars = "0123456789ABCDEFabcdef";
    
    // Generate valid #RGB colors (3 hex digits)
    for (int i = 0; i < 20; ++i) {
        QString color = "#";
        for (int j = 0; j < 3; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        QTest::newRow(qPrintable(QString("valid_rgb_%1").arg(i))) << color << true;
    }
    
    // Generate valid #RRGGBB colors (6 hex digits)
    for (int i = 0; i < 30; ++i) {
        QString color = "#";
        for (int j = 0; j < 6; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        QTest::newRow(qPrintable(QString("valid_rrggbb_%1").arg(i))) << color << true;
    }
    
    // Generate valid #AARRGGBB colors (8 hex digits)
    for (int i = 0; i < 20; ++i) {
        QString color = "#";
        for (int j = 0; j < 8; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        QTest::newRow(qPrintable(QString("valid_aarrggbb_%1").arg(i))) << color << true;
    }
    
    // Generate invalid colors - wrong number of digits
    for (int i = 0; i < 10; ++i) {
        int numDigits;
        do {
            numDigits = rng->bounded(1, 12);
        } while (numDigits == 3 || numDigits == 6 || numDigits == 8);
        
        QString color = "#";
        for (int j = 0; j < numDigits; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        QTest::newRow(qPrintable(QString("invalid_length_%1").arg(i))) << color << false;
    }
    
    // Generate invalid colors - non-hex characters
    const QString nonHexChars = "ghijklmnopqrstuvwxyzGHIJKLMNOPQRSTUVWXYZ!@#$%";
    for (int i = 0; i < 10; ++i) {
        QString color = "#";
        for (int j = 0; j < 5; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        // Add a non-hex character
        color += nonHexChars[rng->bounded(nonHexChars.length())];
        QTest::newRow(qPrintable(QString("invalid_nonhex_%1").arg(i))) << color << false;
    }
    
    // Edge cases
    QTest::newRow("empty_string") << QString("") << false;
    QTest::newRow("no_hash") << QString("ffffff") << false;
    QTest::newRow("double_hash") << QString("##ffffff") << false;
    QTest::newRow("hash_only") << QString("#") << false;
    QTest::newRow("valid_black") << QString("#000000") << true;
    QTest::newRow("valid_white") << QString("#FFFFFF") << true;
    QTest::newRow("valid_red") << QString("#FF0000") << true;
    QTest::newRow("valid_short_red") << QString("#F00") << true;
    QTest::newRow("valid_with_alpha") << QString("#80FFFFFF") << true;
    QTest::newRow("lowercase_valid") << QString("#aabbcc") << true;
    QTest::newRow("mixed_case_valid") << QString("#AaBbCc") << true;
    QTest::newRow("rgb_function") << QString("rgb(255,0,0)") << false;
    QTest::newRow("named_color") << QString("red") << false;
    QTest::newRow("spaces") << QString("# ffffff") << false;
}

void TestVariableManager::testColorValueDetectionProperty()
{
    // Feature: qss-variables, Property 4: Color Value Detection
    
    QFETCH(QString, value);
    QFETCH(bool, expectedIsColor);
    
    bool actualIsColor = VariableManager::isColorValue(value);
    
    QVERIFY2(actualIsColor == expectedIsColor,
             qPrintable(QString("Value '%1': expected %2, got %3")
                       .arg(value)
                       .arg(expectedIsColor ? "color" : "not color")
                       .arg(actualIsColor ? "color" : "not color")));
}

// =============================================================================
// Property 3: Variable CRUD Consistency
// Feature: qss-variables, Property 3: Variable CRUD Consistency
// =============================================================================

void TestVariableManager::testVariableCRUDConsistencyProperty_data()
{
    QTest::addColumn<QStringList>("operations");
    QTest::addColumn<QStringList>("names");
    QTest::addColumn<QStringList>("values");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    
    // Helper lambda to generate a random valid variable name
    auto generateName = [&]() -> QString {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = rng->bounded(1, 10);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        return name;
    };
    
    // Helper lambda to generate a random value
    auto generateValue = [&]() -> QString {
        // Mix of colors and other values
        if (rng->bounded(2) == 0) {
            // Generate a color
            QString color = "#";
            const QString hexChars = "0123456789ABCDEF";
            for (int j = 0; j < 6; ++j) {
                color += hexChars[rng->bounded(hexChars.length())];
            }
            return color;
        } else {
            // Generate a random string value
            QString value;
            int length = rng->bounded(1, 20);
            for (int j = 0; j < length; ++j) {
                value += QChar('a' + rng->bounded(26));
            }
            return value;
        }
    };
    
    // Generate 100 random operation sequences
    for (int i = 0; i < 100; ++i) {
        QStringList operations;
        QStringList names;
        QStringList values;
        
        // Generate a pool of variable names to use
        QStringList namePool;
        int poolSize = 3 + rng->bounded(5);
        for (int j = 0; j < poolSize; ++j) {
            namePool.append(generateName());
        }
        
        // Generate 5-15 operations
        int numOps = 5 + rng->bounded(11);
        for (int j = 0; j < numOps; ++j) {
            int opType = rng->bounded(4); // 0=create, 1=update, 2=delete, 3=clear
            QString name = namePool[rng->bounded(namePool.size())];
            QString value = generateValue();
            
            switch (opType) {
                case 0: // Create/Set
                case 1: // Update (same as set)
                    operations.append("set");
                    names.append(name);
                    values.append(value);
                    break;
                case 2: // Delete
                    operations.append("remove");
                    names.append(name);
                    values.append("");
                    break;
                case 3: // Clear all
                    operations.append("clear");
                    names.append("");
                    values.append("");
                    break;
            }
        }
        
        QTest::newRow(qPrintable(QString("crud_sequence_%1").arg(i))) 
            << operations << names << values;
    }
}

void TestVariableManager::testVariableCRUDConsistencyProperty()
{
    // Feature: qss-variables, Property 3: Variable CRUD Consistency
    
    QFETCH(QStringList, operations);
    QFETCH(QStringList, names);
    QFETCH(QStringList, values);
    
    VariableManager manager;
    
    // Track expected state
    QMap<QString, QString> expectedState;
    
    // Execute operations and verify consistency
    for (int i = 0; i < operations.size(); ++i) {
        const QString &op = operations[i];
        const QString &name = names[i];
        const QString &value = values[i];
        
        if (op == "set") {
            manager.setVariable(name, value);
            expectedState[name] = value;
            
            // Verify: After creating/updating, variable exists with correct value
            QVERIFY2(manager.hasVariable(name),
                     qPrintable(QString("After set, variable '%1' should exist").arg(name)));
            QVERIFY2(manager.variable(name) == value,
                     qPrintable(QString("After set, variable '%1' should have value '%2', got '%3'")
                               .arg(name, value, manager.variable(name))));
        }
        else if (op == "remove") {
            manager.removeVariable(name);
            expectedState.remove(name);
            
            // Verify: After deleting, variable no longer exists
            QVERIFY2(!manager.hasVariable(name),
                     qPrintable(QString("After remove, variable '%1' should not exist").arg(name)));
        }
        else if (op == "clear") {
            manager.clearVariables();
            expectedState.clear();
            
            // Verify: After clear, no variables exist
            QVERIFY2(manager.variableNames().isEmpty(),
                     "After clear, variableNames() should be empty");
        }
        
        // Verify overall consistency: variableNames() returns exactly expected variables
        QStringList actualNames = manager.variableNames();
        QStringList expectedNames = expectedState.keys();
        
        // Sort for comparison
        actualNames.sort();
        expectedNames.sort();
        
        QVERIFY2(actualNames == expectedNames,
                 qPrintable(QString("Variable names mismatch. Expected: [%1], Got: [%2]")
                           .arg(expectedNames.join(", "), actualNames.join(", "))));
        
        // Verify all values match
        for (const QString &varName : expectedNames) {
            QVERIFY2(manager.variable(varName) == expectedState[varName],
                     qPrintable(QString("Value mismatch for '%1'. Expected: '%2', Got: '%3'")
                               .arg(varName, expectedState[varName], manager.variable(varName))));
        }
    }
}


// =============================================================================
// Property 5: Variable Reference Pattern Recognition
// Feature: qss-variables, Property 5: Variable Reference Pattern Recognition
// =============================================================================

void TestVariableManager::testVariableReferencePatternRecognitionProperty_data()
{
    QTest::addColumn<QString>("qssTemplate");
    QTest::addColumn<QStringList>("expectedReferences");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    
    // Helper lambda to generate a random valid variable name
    auto generateName = [&]() -> QString {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = rng->bounded(1, 10);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        return name;
    };
    
    // Generate 100 random templates with known variable references
    for (int i = 0; i < 100; ++i) {
        QStringList expectedRefs;
        QString qssTemplate;
        
        // Generate some QSS-like content with embedded variable references
        int numSegments = 2 + rng->bounded(5);
        for (int j = 0; j < numSegments; ++j) {
            // Add some random QSS-like text
            qssTemplate += "QWidget { color: ";
            
            // Randomly decide to add a variable reference
            if (rng->bounded(2) == 0) {
                QString varName = generateName();
                qssTemplate += "${" + varName + "}";
                if (!expectedRefs.contains(varName)) {
                    expectedRefs.append(varName);
                }
            } else {
                qssTemplate += "#ffffff";
            }
            
            qssTemplate += "; background: ";
            
            // Another chance for a variable reference
            if (rng->bounded(2) == 0) {
                QString varName = generateName();
                qssTemplate += "${" + varName + "}";
                if (!expectedRefs.contains(varName)) {
                    expectedRefs.append(varName);
                }
            } else {
                qssTemplate += "#000000";
            }
            
            qssTemplate += "; }\n";
        }
        
        QTest::newRow(qPrintable(QString("template_%1").arg(i))) 
            << qssTemplate << expectedRefs;
    }
    
    // Edge cases
    QTest::newRow("empty_template") << QString("") << QStringList();
    QTest::newRow("no_references") << QString("QWidget { color: #fff; }") << QStringList();
    QTest::newRow("single_reference") << QString("color: ${myColor};") << QStringList({"myColor"});
    QTest::newRow("duplicate_reference") << QString("${a} ${a} ${a}") << QStringList({"a"});
    QTest::newRow("multiple_unique") << QString("${a} ${b} ${c}") << QStringList({"a", "b", "c"});
    QTest::newRow("underscore_start") << QString("${_private}") << QStringList({"_private"});
    QTest::newRow("with_hyphen") << QString("${my-var}") << QStringList({"my-var"});
    QTest::newRow("with_numbers") << QString("${var123}") << QStringList({"var123"});
    QTest::newRow("complex_name") << QString("${_my-Var_123}") << QStringList({"_my-Var_123"});
    QTest::newRow("adjacent_refs") << QString("${a}${b}") << QStringList({"a", "b"});
    QTest::newRow("nested_braces_invalid") << QString("${{nested}}") << QStringList();
    QTest::newRow("incomplete_ref") << QString("${incomplete") << QStringList();
    QTest::newRow("dollar_only") << QString("$ {notavar}") << QStringList();
    QTest::newRow("starts_with_digit_invalid") << QString("${123invalid}") << QStringList();
}

void TestVariableManager::testVariableReferencePatternRecognitionProperty()
{
    // Feature: qss-variables, Property 5: Variable Reference Pattern Recognition
    
    QFETCH(QString, qssTemplate);
    QFETCH(QStringList, expectedReferences);
    
    VariableManager manager;
    QStringList actualReferences = manager.findVariableReferences(qssTemplate);
    
    // Sort both lists for comparison
    QStringList sortedExpected = expectedReferences;
    QStringList sortedActual = actualReferences;
    sortedExpected.sort();
    sortedActual.sort();
    
    QVERIFY2(sortedActual == sortedExpected,
             qPrintable(QString("Template: '%1'\nExpected refs: [%2]\nActual refs: [%3]")
                       .arg(qssTemplate.left(100))
                       .arg(sortedExpected.join(", "))
                       .arg(sortedActual.join(", "))));
}

// =============================================================================
// Property 6: Variable Substitution Correctness
// Feature: qss-variables, Property 6: Variable Substitution Correctness
// =============================================================================

// Type alias for QMap to work with QFETCH macro
typedef QMap<QString, QString> StringMap;

void TestVariableManager::testVariableSubstitutionCorrectnessProperty_data()
{
    QTest::addColumn<QString>("qssTemplate");
    QTest::addColumn<StringMap>("variables");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    const QString hexChars = "0123456789ABCDEF";
    
    // Helper lambda to generate a random valid variable name
    auto generateName = [&]() -> QString {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = rng->bounded(1, 8);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        return name;
    };
    
    // Helper lambda to generate a random color value
    auto generateColor = [&]() -> QString {
        QString color = "#";
        for (int j = 0; j < 6; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        return color;
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        QMap<QString, QString> variables;
        QString qssTemplate;
        
        // Generate 2-5 variables
        int numVars = 2 + rng->bounded(4);
        QStringList varNames;
        for (int j = 0; j < numVars; ++j) {
            QString name = generateName();
            QString value = generateColor();
            variables[name] = value;
            varNames.append(name);
        }
        
        // Build a template using these variables
        qssTemplate = "QWidget {\n";
        for (int j = 0; j < varNames.size(); ++j) {
            qssTemplate += QString("    property%1: ${%2};\n").arg(j).arg(varNames[j]);
        }
        qssTemplate += "}\n";
        
        // Sometimes add duplicate references
        if (rng->bounded(2) == 0 && !varNames.isEmpty()) {
            qssTemplate += QString("/* duplicate: ${%1} */\n").arg(varNames[0]);
        }
        
        QTest::newRow(qPrintable(QString("substitution_%1").arg(i))) 
            << qssTemplate << variables;
    }
    
    // Edge cases
    StringMap emptyVars;
    StringMap singleVar;
    singleVar["color"] = "#FF0000";
    
    QTest::newRow("empty_template_empty_vars") << QString("") << emptyVars;
    QTest::newRow("no_refs_with_vars") << QString("QWidget { color: #fff; }") << singleVar;
    QTest::newRow("single_substitution") << QString("color: ${color};") << singleVar;
    
    StringMap multiVars;
    multiVars["a"] = "valueA";
    multiVars["b"] = "valueB";
    QTest::newRow("multiple_substitutions") << QString("${a} and ${b}") << multiVars;
    QTest::newRow("repeated_var") << QString("${a} ${a} ${a}") << singleVar;
}

void TestVariableManager::testVariableSubstitutionCorrectnessProperty()
{
    // Feature: qss-variables, Property 6: Variable Substitution Correctness
    
    QFETCH(QString, qssTemplate);
    QFETCH(StringMap, variables);
    
    VariableManager manager;
    
    // Set up variables
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        manager.setVariable(it.key(), it.value());
    }
    
    QString result = manager.substitute(qssTemplate);
    
    // Verify: For each defined variable, its reference should be replaced
    // The result should NOT contain ${name} for any variable that exists
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        QString reference = QString("${%1}").arg(it.key());
        
        QVERIFY2(!result.contains(reference),
                 qPrintable(QString("Result still contains reference '%1' which should have been substituted.\nTemplate: %2\nResult: %3")
                           .arg(reference)
                           .arg(qssTemplate.left(100))
                           .arg(result.left(100))));
        
        // If the template contained this reference, the value should appear in result
        if (qssTemplate.contains(reference)) {
            QVERIFY2(result.contains(it.value()),
                     qPrintable(QString("Result should contain value '%1' for variable '%2'.\nResult: %3")
                               .arg(it.value())
                               .arg(it.key())
                               .arg(result.left(100))));
        }
    }
}

// =============================================================================
// Property 7: Undefined Reference Preservation
// Feature: qss-variables, Property 7: Undefined Reference Preservation
// =============================================================================

void TestVariableManager::testUndefinedReferencePreservationProperty_data()
{
    QTest::addColumn<QString>("qssTemplate");
    QTest::addColumn<StringMap>("definedVariables");
    QTest::addColumn<QStringList>("undefinedRefs");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    const QString hexChars = "0123456789ABCDEF";
    
    // Helper lambda to generate a random valid variable name
    auto generateName = [&]() -> QString {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = rng->bounded(1, 8);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        return name;
    };
    
    // Helper lambda to generate a random color value
    auto generateColor = [&]() -> QString {
        QString color = "#";
        for (int j = 0; j < 6; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        return color;
    };
    
    // Generate 100 random test cases with mix of defined and undefined references
    for (int i = 0; i < 100; ++i) {
        QMap<QString, QString> definedVars;
        QStringList undefinedRefs;
        QString qssTemplate;
        
        // Generate 1-3 defined variables
        int numDefined = 1 + rng->bounded(3);
        QStringList definedNames;
        for (int j = 0; j < numDefined; ++j) {
            QString name = generateName();
            definedVars[name] = generateColor();
            definedNames.append(name);
        }
        
        // Generate 1-3 undefined variable names (verify they're different)
        int numUndefined = 1 + rng->bounded(3);
        for (int j = 0; j < numUndefined; ++j) {
            QString name;
            do {
                name = generateName();
            } while (definedVars.contains(name) || undefinedRefs.contains(name));
            undefinedRefs.append(name);
        }
        
        // Build template with both defined and undefined references
        qssTemplate = "QWidget {\n";
        for (const QString &name : definedNames) {
            qssTemplate += QString("    defined: ${%1};\n").arg(name);
        }
        for (const QString &name : undefinedRefs) {
            qssTemplate += QString("    undefined: ${%1};\n").arg(name);
        }
        qssTemplate += "}\n";
        
        QTest::newRow(qPrintable(QString("preservation_%1").arg(i))) 
            << qssTemplate << definedVars << undefinedRefs;
    }
    
    // Edge cases
    StringMap emptyVars;
    StringMap singleVar;
    singleVar["defined"] = "#FF0000";
    
    QTest::newRow("all_undefined") 
        << QString("${undefined1} ${undefined2}") 
        << emptyVars 
        << QStringList({"undefined1", "undefined2"});
    
    QTest::newRow("mixed_defined_undefined") 
        << QString("${defined} ${notdefined}") 
        << singleVar 
        << QStringList({"notdefined"});
    
    QTest::newRow("only_defined_no_undefined") 
        << QString("${defined}") 
        << singleVar 
        << QStringList();
}

void TestVariableManager::testUndefinedReferencePreservationProperty()
{
    // Feature: qss-variables, Property 7: Undefined Reference Preservation
    
    QFETCH(QString, qssTemplate);
    QFETCH(StringMap, definedVariables);
    QFETCH(QStringList, undefinedRefs);
    
    VariableManager manager;
    
    // Set up only the defined variables
    for (auto it = definedVariables.constBegin(); it != definedVariables.constEnd(); ++it) {
        manager.setVariable(it.key(), it.value());
    }
    
    QString result = manager.substitute(qssTemplate);
    
    // Verify: All undefined references should remain unchanged in the output
    for (const QString &undefinedName : undefinedRefs) {
        QString reference = QString("${%1}").arg(undefinedName);
        
        // Count occurrences in template
        int templateCount = 0;
        int pos = 0;
        while ((pos = qssTemplate.indexOf(reference, pos)) != -1) {
            ++templateCount;
            pos += reference.length();
        }
        
        // Count occurrences in result
        int resultCount = 0;
        pos = 0;
        while ((pos = result.indexOf(reference, pos)) != -1) {
            ++resultCount;
            pos += reference.length();
        }
        
        QVERIFY2(resultCount == templateCount,
                 qPrintable(QString("Undefined reference '%1' should be preserved.\n"
                                   "Expected %2 occurrences, found %3.\n"
                                   "Template: %4\nResult: %5")
                           .arg(reference)
                           .arg(templateCount)
                           .arg(resultCount)
                           .arg(qssTemplate.left(100))
                           .arg(result.left(100))));
    }
    
    // Also verify defined variables were substituted
    for (auto it = definedVariables.constBegin(); it != definedVariables.constEnd(); ++it) {
        QString reference = QString("${%1}").arg(it.key());
        QVERIFY2(!result.contains(reference),
                 qPrintable(QString("Defined reference '%1' should have been substituted")
                           .arg(reference)));
    }
}


// =============================================================================
// Property 8: Project File Round-Trip
// Feature: qss-variables, Property 8: Project File Round-Trip
// =============================================================================

void TestVariableManager::testProjectFileRoundTripProperty_data()
{
    QTest::addColumn<StringMap>("variables");
    QTest::addColumn<QString>("qssTemplate");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    const QString hexChars = "0123456789ABCDEF";
    
    // Helper lambda to generate a random valid variable name
    auto generateName = [&]() -> QString {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = rng->bounded(1, 10);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        return name;
    };
    
    // Helper lambda to generate a random color value
    auto generateColor = [&]() -> QString {
        QString color = "#";
        for (int j = 0; j < 6; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        return color;
    };
    
    // Helper lambda to generate a random string value
    auto generateValue = [&]() -> QString {
        if (rng->bounded(2) == 0) {
            return generateColor();
        }
        QString value;
        int length = rng->bounded(1, 20);
        for (int j = 0; j < length; ++j) {
            value += QChar('a' + rng->bounded(26));
        }
        return value;
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        StringMap variables;
        QString qssTemplate;
        
        // Generate 0-5 variables
        int numVars = rng->bounded(6);
        QStringList varNames;
        for (int j = 0; j < numVars; ++j) {
            QString name = generateName();
            variables[name] = generateValue();
            varNames.append(name);
        }
        
        // Build a QSS template
        qssTemplate = "QWidget {\n";
        for (const QString &name : varNames) {
            qssTemplate += QString("    color: ${%1};\n").arg(name);
        }
        qssTemplate += "    background: #ffffff;\n";
        qssTemplate += "}\n";
        
        // Add some random QSS content
        int extraLines = rng->bounded(5);
        for (int j = 0; j < extraLines; ++j) {
            qssTemplate += QString("QPushButton { margin: %1px; }\n").arg(rng->bounded(20));
        }
        
        QTest::newRow(qPrintable(QString("roundtrip_%1").arg(i))) 
            << variables << qssTemplate;
    }
    
    // Edge cases
    StringMap emptyVars;
    QTest::newRow("empty_vars_empty_template") << emptyVars << QString("");
    QTest::newRow("empty_vars_with_template") << emptyVars << QString("QWidget { color: #fff; }");
    
    StringMap singleVar;
    singleVar["primary"] = "#3498db";
    QTest::newRow("single_var") << singleVar << QString("color: ${primary};");
    
    StringMap multiVars;
    multiVars["bg"] = "#2c3e50";
    multiVars["fg"] = "#ecf0f1";
    multiVars["accent"] = "#3498db";
    QTest::newRow("multiple_vars") << multiVars << QString("QWidget { background: ${bg}; color: ${fg}; border-color: ${accent}; }");
    
    // Test with special characters in template (but not in variable names)
    StringMap specialVars;
    specialVars["color"] = "#FF0000";
    QTest::newRow("special_chars_template") << specialVars 
        << QString("/* Comment with special chars: @#$%^&*() */\nQWidget { color: ${color}; }");
    
    // Test with newlines and tabs
    QTest::newRow("whitespace_template") << singleVar 
        << QString("QWidget {\n\tcolor: ${primary};\n\tbackground: #000;\n}");
}

void TestVariableManager::testProjectFileRoundTripProperty()
{
    // Feature: qss-variables, Property 8: Project File Round-Trip
    
    QFETCH(StringMap, variables);
    QFETCH(QString, qssTemplate);
    
    VariableManager saveManager;
    
    // Set up variables
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        saveManager.setVariable(it.key(), it.value());
    }
    
    // Create a temporary file
    QTemporaryFile tempFile;
    tempFile.setFileTemplate(QDir::tempPath() + "/test_project_XXXXXX.qvp");
    QVERIFY(tempFile.open());
    QString filePath = tempFile.fileName();
    tempFile.close();
    
    // Save the project
    bool saveResult = saveManager.saveProject(filePath, qssTemplate);
    QVERIFY2(saveResult, "Failed to save project file");
    
    // Load into a new manager
    VariableManager loadManager;
    QString loadedTemplate;
    bool loadResult = loadManager.loadProject(filePath, loadedTemplate);
    QVERIFY2(loadResult, "Failed to load project file");
    
    // Verify template is identical
    QVERIFY2(loadedTemplate == qssTemplate,
             qPrintable(QString("Template mismatch.\nExpected: %1\nGot: %2")
                       .arg(qssTemplate.left(100))
                       .arg(loadedTemplate.left(100))));
    
    // Verify all variables are restored
    StringMap loadedVars = loadManager.allVariables();
    
    QVERIFY2(loadedVars.size() == variables.size(),
             qPrintable(QString("Variable count mismatch. Expected %1, got %2")
                       .arg(variables.size())
                       .arg(loadedVars.size())));
    
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        QVERIFY2(loadedVars.contains(it.key()),
                 qPrintable(QString("Missing variable: %1").arg(it.key())));
        QVERIFY2(loadedVars[it.key()] == it.value(),
                 qPrintable(QString("Value mismatch for '%1'. Expected: '%2', Got: '%3'")
                           .arg(it.key(), it.value(), loadedVars[it.key()])));
    }
    
    // Clean up
    QFile::remove(filePath);
}

// =============================================================================
// Property 9: Project File JSON Validity
// Feature: qss-variables, Property 9: Project File JSON Validity
// =============================================================================

void TestVariableManager::testProjectFileJsonValidityProperty_data()
{
    QTest::addColumn<StringMap>("variables");
    QTest::addColumn<QString>("qssTemplate");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    const QString hexChars = "0123456789ABCDEF";
    
    // Helper lambda to generate a random valid variable name
    auto generateName = [&]() -> QString {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = rng->bounded(1, 10);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        return name;
    };
    
    // Helper lambda to generate a random color value
    auto generateColor = [&]() -> QString {
        QString color = "#";
        for (int j = 0; j < 6; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        return color;
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        StringMap variables;
        QString qssTemplate;
        
        // Generate 0-5 variables
        int numVars = rng->bounded(6);
        for (int j = 0; j < numVars; ++j) {
            QString name = generateName();
            variables[name] = generateColor();
        }
        
        // Generate a simple template
        qssTemplate = QString("QWidget { color: #%1; }").arg(rng->bounded(0xFFFFFF), 6, 16, QChar('0'));
        
        QTest::newRow(qPrintable(QString("json_valid_%1").arg(i))) 
            << variables << qssTemplate;
    }
    
    // Edge cases with special characters that need JSON escaping
    StringMap specialVars;
    specialVars["color"] = "#FF0000";
    
    QTest::newRow("template_with_quotes") << specialVars 
        << QString("QWidget { font-family: \"Arial\"; }");
    QTest::newRow("template_with_backslash") << specialVars 
        << QString("/* path: C:\\Users\\test */");
    QTest::newRow("template_with_newlines") << specialVars 
        << QString("QWidget {\n    color: red;\n}");
    QTest::newRow("template_with_tabs") << specialVars 
        << QString("QWidget {\t\tcolor: red;\t}");
    QTest::newRow("template_with_unicode") << specialVars 
        << QString("/* Unicode: \u00e9\u00e8\u00ea */");
}

void TestVariableManager::testProjectFileJsonValidityProperty()
{
    // Feature: qss-variables, Property 9: Project File JSON Validity
    
    QFETCH(StringMap, variables);
    QFETCH(QString, qssTemplate);
    
    VariableManager manager;
    
    // Set up variables
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        manager.setVariable(it.key(), it.value());
    }
    
    // Create a temporary file
    QTemporaryFile tempFile;
    tempFile.setFileTemplate(QDir::tempPath() + "/test_json_XXXXXX.qvp");
    QVERIFY(tempFile.open());
    QString filePath = tempFile.fileName();
    tempFile.close();
    
    // Save the project
    bool saveResult = manager.saveProject(filePath, qssTemplate);
    QVERIFY2(saveResult, "Failed to save project file");
    
    // Read the file content
    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QByteArray fileContent = file.readAll();
    file.close();
    
    // Verify it's valid JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(fileContent, &parseError);
    
    QVERIFY2(parseError.error == QJsonParseError::NoError,
             qPrintable(QString("Invalid JSON: %1 at offset %2.\nContent: %3")
                       .arg(parseError.errorString())
                       .arg(parseError.offset)
                       .arg(QString::fromUtf8(fileContent.left(200)))));
    
    // Verify it's a JSON object (not array or other)
    QVERIFY2(doc.isObject(), "Project file should be a JSON object");
    
    // Verify required fields exist
    QJsonObject root = doc.object();
    QVERIFY2(root.contains("version"), "Missing 'version' field");
    QVERIFY2(root.contains("variables"), "Missing 'variables' field");
    QVERIFY2(root.contains("qssTemplate"), "Missing 'qssTemplate' field");
    
    // Verify version is a number
    QVERIFY2(root["version"].isDouble(), "'version' should be a number");
    
    // Verify variables is an object
    QVERIFY2(root["variables"].isObject(), "'variables' should be an object");
    
    // Verify qssTemplate is a string
    QVERIFY2(root["qssTemplate"].isString(), "'qssTemplate' should be a string");
    
    // Clean up
    QFile::remove(filePath);
}

// =============================================================================
// Property 10: QSS Export Correctness
// Feature: qss-variables, Property 10: QSS Export Correctness
// =============================================================================

void TestVariableManager::testQssExportCorrectnessProperty_data()
{
    QTest::addColumn<StringMap>("variables");
    QTest::addColumn<QString>("qssTemplate");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    const QString hexChars = "0123456789ABCDEF";
    
    // Helper lambda to generate a random valid variable name
    auto generateName = [&]() -> QString {
        QString name;
        name += validStartChars[rng->bounded(validStartChars.length())];
        int length = rng->bounded(1, 8);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        return name;
    };
    
    // Helper lambda to generate a random color value
    auto generateColor = [&]() -> QString {
        QString color = "#";
        for (int j = 0; j < 6; ++j) {
            color += hexChars[rng->bounded(hexChars.length())];
        }
        return color;
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        StringMap variables;
        QString qssTemplate;
        
        // Generate 1-5 variables
        int numVars = 1 + rng->bounded(5);
        QStringList varNames;
        for (int j = 0; j < numVars; ++j) {
            QString name = generateName();
            variables[name] = generateColor();
            varNames.append(name);
        }
        
        // Build a QSS template using the variables
        qssTemplate = "QWidget {\n";
        for (const QString &name : varNames) {
            qssTemplate += QString("    color: ${%1};\n").arg(name);
        }
        qssTemplate += "}\n";
        
        QTest::newRow(qPrintable(QString("export_%1").arg(i))) 
            << variables << qssTemplate;
    }
    
    // Edge cases
    StringMap emptyVars;
    QTest::newRow("no_vars_no_refs") << emptyVars << QString("QWidget { color: #fff; }");
    
    StringMap singleVar;
    singleVar["primary"] = "#3498db";
    QTest::newRow("single_var_single_ref") << singleVar << QString("color: ${primary};");
    QTest::newRow("single_var_multiple_refs") << singleVar << QString("${primary} ${primary} ${primary}");
    
    // Test with undefined references (should be preserved)
    QTest::newRow("with_undefined_ref") << singleVar << QString("${primary} ${undefined}");
    
    StringMap multiVars;
    multiVars["a"] = "valueA";
    multiVars["b"] = "valueB";
    multiVars["c"] = "valueC";
    QTest::newRow("multiple_vars_multiple_refs") << multiVars << QString("${a} ${b} ${c} ${a}");
}

void TestVariableManager::testQssExportCorrectnessProperty()
{
    // Feature: qss-variables, Property 10: QSS Export Correctness
    
    QFETCH(StringMap, variables);
    QFETCH(QString, qssTemplate);
    
    VariableManager manager;
    
    // Set up variables
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        manager.setVariable(it.key(), it.value());
    }
    
    // Get the expected result from substitute()
    QString expectedContent = manager.substitute(qssTemplate);
    
    // Create a temporary file
    QTemporaryFile tempFile;
    tempFile.setFileTemplate(QDir::tempPath() + "/test_export_XXXXXX.qss");
    QVERIFY(tempFile.open());
    QString filePath = tempFile.fileName();
    tempFile.close();
    
    // Export the resolved QSS
    bool exportResult = manager.exportResolvedQss(filePath, qssTemplate);
    QVERIFY2(exportResult, "Failed to export QSS file");
    
    // Read the exported file content
    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QString actualContent = QTextStream(&file).readAll();
    file.close();
    
    // Verify the exported content matches substitute() result
    QVERIFY2(actualContent == expectedContent,
             qPrintable(QString("Export content mismatch.\nExpected: %1\nGot: %2")
                       .arg(expectedContent.left(200))
                       .arg(actualContent.left(200))));
    
    // Clean up
    QFile::remove(filePath);
}


// =============================================================================
// Property 11: Variable Reference Formatting
// Feature: qss-variables, Property 11: Variable Reference Formatting
// =============================================================================

#include "VariablePanel.h"

void TestVariableManager::testVariableReferenceFormattingProperty_data()
{
    QTest::addColumn<QString>("variableName");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Valid variable name characters
    const QString validStartChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const QString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    
    // Generate 100 random valid variable names
    for (int i = 0; i < 100; ++i) {
        QString name;
        // Start with a valid start character
        name += validStartChars[rng->bounded(validStartChars.length())];
        
        // Add 0-15 more valid characters
        int length = rng->bounded(16);
        for (int j = 0; j < length; ++j) {
            name += validChars[rng->bounded(validChars.length())];
        }
        
        QTest::newRow(qPrintable(QString("name_%1").arg(i))) << name;
    }
    
    // Edge cases - specific valid names
    QTest::newRow("single_letter") << QString("a");
    QTest::newRow("single_underscore") << QString("_");
    QTest::newRow("underscore_start") << QString("_myVar");
    QTest::newRow("with_hyphen") << QString("my-variable");
    QTest::newRow("with_numbers") << QString("var123");
    QTest::newRow("all_caps") << QString("MY_VARIABLE");
    QTest::newRow("mixed_case") << QString("myVariable");
    QTest::newRow("complex_valid") << QString("_my-Var_123");
    QTest::newRow("long_name") << QString("thisIsAVeryLongVariableNameThatShouldStillWork");
    QTest::newRow("primary_color") << QString("primary-color");
    QTest::newRow("background") << QString("background");
    QTest::newRow("text_color") << QString("text_color");
}

void TestVariableManager::testVariableReferenceFormattingProperty()
{
    // Feature: qss-variables, Property 11: Variable Reference Formatting
    
    QFETCH(QString, variableName);
    
    // Format the variable name as a reference
    QString reference = VariablePanel::formatVariableReference(variableName);
    
    // Property 1: The formatted reference should be in the format ${name}
    QString expectedFormat = QString("${%1}").arg(variableName);
    QVERIFY2(reference == expectedFormat,
             qPrintable(QString("Reference format mismatch. Expected: '%1', Got: '%2'")
                       .arg(expectedFormat, reference)));
    
    // Property 2: The formatted reference should be recognizable by findVariableReferences
    VariableManager manager;
    QStringList foundRefs = manager.findVariableReferences(reference);
    
    QVERIFY2(foundRefs.size() == 1,
             qPrintable(QString("Expected 1 reference found, got %1 for reference '%2'")
                       .arg(foundRefs.size())
                       .arg(reference)));
    
    QVERIFY2(foundRefs.first() == variableName,
             qPrintable(QString("Found reference name mismatch. Expected: '%1', Got: '%2'")
                       .arg(variableName, foundRefs.first())));
    
    // Property 3: When the variable is defined, substitution should work correctly
    QString testValue = "#FF0000";
    manager.setVariable(variableName, testValue);
    
    QString substituted = manager.substitute(reference);
    QVERIFY2(substituted == testValue,
             qPrintable(QString("Substitution failed. Expected: '%1', Got: '%2' for reference '%3'")
                       .arg(testValue, substituted, reference)));
    
    // Property 4: The reference should start with ${ and end with }
    QVERIFY2(reference.startsWith("${"),
             qPrintable(QString("Reference should start with '${': '%1'").arg(reference)));
    QVERIFY2(reference.endsWith("}"),
             qPrintable(QString("Reference should end with '}': '%1'").arg(reference)));
    
    // Property 5: The name extracted from the reference should match the original
    QString extractedName = reference.mid(2, reference.length() - 3); // Remove ${ and }
    QVERIFY2(extractedName == variableName,
             qPrintable(QString("Extracted name mismatch. Expected: '%1', Got: '%2'")
                       .arg(variableName, extractedName)));
}
