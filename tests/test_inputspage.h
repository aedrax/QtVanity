#ifndef TEST_INPUTSPAGE_H
#define TEST_INPUTSPAGE_H

#include <QObject>
#include <QtTest>

/**
 * @brief Unit tests for InputsPage widget presence.
 * 
 * Tests verify that the new advanced input widgets are present:
 * - QFontComboBox
 * - QKeySequenceEdit
 * - QPlainTextEdit
 * - QScrollBar
 */
class TestInputsPage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testPageCreation();
    void testFontComboBoxPresent();
    void testKeySequenceEditPresent();
    void testPlainTextEditPresent();
    void testScrollBarsPresent();
    void testSetWidgetsEnabled();
    void testSetReadOnly();
};

#endif // TEST_INPUTSPAGE_H
