#ifndef TEST_DIALOGSPAGE_H
#define TEST_DIALOGSPAGE_H

#include <QObject>
#include <QtTest>

/**
 * @brief Unit tests for DialogsPage widget presence.
 * 
 * Tests verify that the new dialog trigger widgets are present:
 * - QWizard trigger button
 * - QInputDialog trigger buttons (text, integer, item selection)
 */
class TestDialogsPage : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testPageCreation();
    void testWizardButtonPresent();
    void testInputDialogButtonsPresent();
    void testSetWidgetsEnabled();
};

#endif // TEST_DIALOGSPAGE_H
