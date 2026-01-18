#include <QApplication>
#include <QtTest>

// Include all test classes
#include "test_stylemanager.h"
#include "test_thememanager.h"
#include "test_qsssyntaxhighlighter.h"
#include "test_qsseditor.h"
#include "test_widgetgallery.h"
#include "test_displaypage.h"
#include "test_mainwindowpage.h"
#include "test_advancedpage.h"
#include "test_inputspage.h"
#include "test_containerspage.h"
#include "test_dialogspage.h"
#include "test_mainwindow.h"
#include "test_variablemanager.h"
#include "test_settingsmanager.h"
#include "test_variablepanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    int status = 0;
    
    // Run StyleManager tests
    {
        TestStyleManager test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run ThemeManager tests
    {
        TestThemeManager test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run QssSyntaxHighlighter tests
    {
        TestQssSyntaxHighlighter test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run QssEditor tests
    {
        TestQssEditor test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run WidgetGallery tests
    {
        TestWidgetGallery test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run DisplayPage tests
    {
        TestDisplayPage test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run MainWindowPage tests
    {
        TestMainWindowPage test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run AdvancedPage tests
    {
        TestAdvancedPage test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run InputsPage tests
    {
        TestInputsPage test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run ContainersPage tests
    {
        TestContainersPage test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run DialogsPage tests
    {
        TestDialogsPage test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run MainWindow tests
    {
        TestMainWindow test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run VariableManager tests
    {
        TestVariableManager test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run SettingsManager tests
    {
        TestSettingsManager test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    // Run VariablePanel tests
    {
        TestVariablePanel test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    return status;
}
