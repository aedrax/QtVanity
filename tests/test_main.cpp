#include <QApplication>
#include <QtTest>

// Include all test classes
#include "test_stylemanager.h"
#include "test_qsssyntaxhighlighter.h"
#include "test_qsseditor.h"
#include "test_widgetgallery.h"
#include "test_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    int status = 0;
    
    // Run StyleManager tests
    {
        TestStyleManager test;
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
    
    // Run MainWindow tests
    {
        TestMainWindow test;
        status |= QTest::qExec(&test, argc, argv);
    }
    
    return status;
}
