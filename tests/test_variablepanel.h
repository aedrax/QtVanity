#ifndef TEST_VARIABLEPANEL_H
#define TEST_VARIABLEPANEL_H

#include <QObject>
#include <QtTest>

class TestVariablePanel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Property 1: Color Format Correctness
    // Feature: color-cell-picker, Property 1: Color Format Correctness
    // For any QColor, if the alpha value is less than 255, the formatted hex string
    // SHALL be in #AARRGGBB format (9 characters); otherwise, it SHALL be in
    // #RRGGBB format (7 characters).
    void testColorFormatCorrectnessProperty();
    void testColorFormatCorrectnessProperty_data();
    
    // Unit tests for color cell click functionality
    void testClickingColorColumnTriggersColorPickerLogic();
    void testClickingNonColorColumnsDoesNotTriggerPicker();
    
    void testDefaultColorWhenValueIsNotValidColor();
};

#endif // TEST_VARIABLEPANEL_H
