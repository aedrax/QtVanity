#ifndef TEST_QSSSYNTAXHIGHLIGHTER_H
#define TEST_QSSSYNTAXHIGHLIGHTER_H

#include <QObject>
#include <QtTest>

class TestQssSyntaxHighlighter : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Unit tests
    void testSelectorHighlighting();
    void testPropertyHighlighting();
    void testPseudoStateHighlighting();
    void testSubControlHighlighting();
    void testCommentHighlighting();
    void testMultilineCommentHighlighting();
    void testColorSchemeSwitch();
    
    // Property-based tests
    void testSyntaxHighlightingCoverage();
    void testSyntaxHighlightingCoverage_data();
};

#endif // TEST_QSSSYNTAXHIGHLIGHTER_H
