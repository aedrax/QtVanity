#include <QApplication>
#include <QtTest>

#include "test_settingsmanager.h"

int main(int argc, char *argv[])
{
    // Use offscreen platform by default for tests to avoid display requirements
    qputenv("QT_QPA_PLATFORM", "offscreen");
    
    QApplication app(argc, argv);
    
    TestSettingsManager test;
    return QTest::qExec(&test, argc, argv);
}
