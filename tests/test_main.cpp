#include <QApplication>
#include <QtTest>

// Include all test classes
#include "test_stylemanager.h"
#include "test_qsssyntaxhighlighter.h"

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
    
    return status;
}
