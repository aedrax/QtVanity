#ifndef TEST_WIDGETGALLERY_H
#define TEST_WIDGETGALLERY_H

#include <QObject>
#include <QtTest>

/**
 * @brief Test class for WidgetGallery functionality.
 * 
 * Tests include:
 * - Property 3: Widget Enabled State Toggle
 * - Property 4: Input Read-Only State Toggle
 */
class TestWidgetGallery : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Unit tests
    void testInitialState();
    void testSetWidgetsEnabled();
    void testSetInputsReadOnly();
    void testSignalEmission();

    // Property-based tests
    void testWidgetEnabledStateToggle_data();
    void testWidgetEnabledStateToggle();
    
    void testInputReadOnlyStateToggle_data();
    void testInputReadOnlyStateToggle();
    
    // Property 1: Enabled State Propagation for new pages
    void testNewPagesEnabledStatePropagation_data();
    void testNewPagesEnabledStatePropagation();
};

#endif // TEST_WIDGETGALLERY_H
