#ifndef TEST_VARIABLEMANAGER_H
#define TEST_VARIABLEMANAGER_H

#include <QObject>
#include <QtTest>

class TestVariableManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Property 1: Variable Name Validation
    // For any string input as a variable name, the validation function SHALL
    // accept it if and only if it is non-empty, starts with a letter or underscore,
    // and contains only letters, numbers, underscores, or hyphens.
    void testVariableNameValidationProperty();
    void testVariableNameValidationProperty_data();
    
    // Property 4: Color Value Detection
    // For any string value, the isColorValue function SHALL return true if and only
    // if the string matches a valid hex color format (#RGB, #RRGGBB, or #AARRGGBB).
    void testColorValueDetectionProperty();
    void testColorValueDetectionProperty_data();
    
    // Property 3: Variable CRUD Consistency
    // For any sequence of variable create, update, and delete operations,
    // the variable manager SHALL maintain a consistent state.
    void testVariableCRUDConsistencyProperty();
    void testVariableCRUDConsistencyProperty_data();
    
    // Property 5: Variable Reference Pattern Recognition
    // For any QSS template string, the findVariableReferences function SHALL
    // return exactly the set of variable names referenced using the ${name} syntax.
    void testVariableReferencePatternRecognitionProperty();
    void testVariableReferencePatternRecognitionProperty_data();
    
    // Property 6: Variable Substitution Correctness
    // For any QSS template and variable map, the substitute function SHALL
    // replace every ${name} reference with the corresponding value from the map.
    void testVariableSubstitutionCorrectnessProperty();
    void testVariableSubstitutionCorrectnessProperty_data();
    
    // Property 7: Undefined Reference Preservation
    // For any QSS template containing variable references where some references
    // have no corresponding variable defined, the substitute function SHALL
    // leave those undefined references unchanged in the output.
    void testUndefinedReferencePreservationProperty();
    void testUndefinedReferencePreservationProperty_data();
    
    // Property 8: Project File Round-Trip
    // For any valid variable map and QSS template, saving to a project file
    // and then loading from that file SHALL restore the exact same variables
    // and template content.
    void testProjectFileRoundTripProperty();
    void testProjectFileRoundTripProperty_data();
    
    // Property 9: Project File JSON Validity
    // For any saved project file, the file content SHALL be valid JSON
    // that can be parsed without errors.
    void testProjectFileJsonValidityProperty();
    void testProjectFileJsonValidityProperty_data();
    
    // Property 10: QSS Export Correctness
    // For any QSS template and variable map, exporting to a .qss file SHALL
    // produce content identical to the result of substitute(template, variables).
    void testQssExportCorrectnessProperty();
    void testQssExportCorrectnessProperty_data();
    
    // Property 11: Variable Reference Formatting
    // For any valid variable name, the reference formatting function SHALL
    // produce a string in the format ${name} that can be recognized by the
    // variable reference pattern.
    void testVariableReferenceFormattingProperty();
    void testVariableReferenceFormattingProperty_data();
};

#endif // TEST_VARIABLEMANAGER_H
