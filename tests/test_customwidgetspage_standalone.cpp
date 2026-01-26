#include <QApplication>
#include <QtTest>

#include "test_customwidgetspage.h"

/**
 * @brief Standalone test runner for CustomWidgetsPage tests.
 * 
 * This executable allows running CustomWidgetsPage tests independently
 * for faster iteration during development.
 */
int main(int argc, char *argv[])
{
    // Use offscreen platform by default for tests to avoid display requirements
    qputenv("QT_QPA_PLATFORM", "offscreen");
    
    QApplication app(argc, argv);
    
    TestCustomWidgetsPage test;
    return QTest::qExec(&test, argc, argv);
}
