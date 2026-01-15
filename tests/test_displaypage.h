#ifndef TEST_DISPLAYPAGE_H
#define TEST_DISPLAYPAGE_H

#include <QObject>
#include <QtTest>

/**
 * @brief Test class for DisplayPage functionality.
 * 
 * Tests include:
 * - Widget presence verification (QLabel, QLCDNumber, QCalendarWidget)
 * - Enabled state propagation
 */
class TestDisplayPage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Unit tests
    void testPageCreation();
    void testLabelWidgetsPresent();
    void testLCDNumberWidgetsPresent();
    void testCalendarWidgetPresent();
    void testSetWidgetsEnabled();
};

#endif // TEST_DISPLAYPAGE_H
