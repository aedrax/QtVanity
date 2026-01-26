/**
 * Standalone test runner for PluginManager tests only.
 * This allows running the PluginManager property tests without
 * waiting for all other tests to complete.
 */

#include <QApplication>
#include <QtTest>

#include "test_pluginmanager.h"

int main(int argc, char *argv[])
{
    // Use offscreen platform by default for tests to avoid display requirements
    qputenv("QT_QPA_PLATFORM", "offscreen");
    
    QApplication app(argc, argv);
    
    TestPluginManager test;
    return QTest::qExec(&test, argc, argv);
}
