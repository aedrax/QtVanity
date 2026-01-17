#include "test_qsssyntaxhighlighter.h"
#include "QssSyntaxHighlighter.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QRandomGenerator>
#include <QCoreApplication>

/**
 * Test helper class that intercepts setFormat calls to track what formats are applied.
 */
class TestableHighlighter : public QssSyntaxHighlighter
{
public:
    explicit TestableHighlighter(QTextDocument *parent = nullptr)
        : QssSyntaxHighlighter(parent), m_tracking(false) {}
    
    struct FormatRecord {
        int start;
        int length;
        QColor color;
    };
    
    QVector<FormatRecord> appliedFormats;
    
    void startTracking() { 
        m_tracking = true; 
        appliedFormats.clear(); 
    }
    
    void stopTracking() { m_tracking = false; }
    
    bool hasFormatWithColor(const QColor &color) const {
        for (const auto &record : appliedFormats) {
            if (record.color == color) return true;
        }
        return false;
    }
    
    // Force rehighlight and track
    void rehighlightAndTrack() {
        startTracking();
        rehighlight();
        stopTracking();
    }
    
protected:
    void highlightBlock(const QString &text) override {
        // Call parent to do the actual highlighting
        QssSyntaxHighlighter::highlightBlock(text);
        
        // If tracking, record what formats were applied
        if (m_tracking) {
            for (int i = 0; i < text.length(); ++i) {
                QTextCharFormat fmt = format(i);
                if (fmt.foreground().style() != Qt::NoBrush) {
                    FormatRecord record;
                    record.start = currentBlock().position() + i;
                    record.length = 1;
                    record.color = fmt.foreground().color();
                    appliedFormats.append(record);
                }
            }
        }
    }
    
private:
    bool m_tracking;
};

void TestQssSyntaxHighlighter::initTestCase()
{
}

void TestQssSyntaxHighlighter::cleanupTestCase()
{
}

void TestQssSyntaxHighlighter::testSelectorHighlighting()
{
    QTextDocument doc;
    TestableHighlighter highlighter(&doc);
    doc.setPlainText("QPushButton { color: red; }");
    highlighter.rehighlightAndTrack();
    
    QColor selectorColor = highlighter.selectorFormat().foreground().color();
    bool foundSelector = highlighter.hasFormatWithColor(selectorColor);
    QVERIFY2(foundSelector, "Selector 'QPushButton' should be highlighted");
}

void TestQssSyntaxHighlighter::testPropertyHighlighting()
{
    QTextDocument doc;
    TestableHighlighter highlighter(&doc);
    doc.setPlainText("QLabel { background-color: blue; }");
    highlighter.rehighlightAndTrack();
    
    QColor propertyColor = highlighter.propertyFormat().foreground().color();
    bool foundProperty = highlighter.hasFormatWithColor(propertyColor);
    QVERIFY2(foundProperty, "Property 'background-color' should be highlighted");
}

void TestQssSyntaxHighlighter::testPseudoStateHighlighting()
{
    QTextDocument doc;
    TestableHighlighter highlighter(&doc);
    doc.setPlainText("QPushButton:hover { color: red; }");
    highlighter.rehighlightAndTrack();
    
    QColor pseudoStateColor = highlighter.pseudoStateFormat().foreground().color();
    bool foundPseudoState = highlighter.hasFormatWithColor(pseudoStateColor);
    QVERIFY2(foundPseudoState, "Pseudo-state ':hover' should be highlighted");
}

void TestQssSyntaxHighlighter::testSubControlHighlighting()
{
    QTextDocument doc;
    TestableHighlighter highlighter(&doc);
    doc.setPlainText("QComboBox::drop-down { border: none; }");
    highlighter.rehighlightAndTrack();
    
    QColor subControlColor = highlighter.subControlFormat().foreground().color();
    bool foundSubControl = highlighter.hasFormatWithColor(subControlColor);
    QVERIFY2(foundSubControl, "Sub-control '::drop-down' should be highlighted");
}

void TestQssSyntaxHighlighter::testCommentHighlighting()
{
    QTextDocument doc;
    TestableHighlighter highlighter(&doc);
    doc.setPlainText("/* This is a comment */ QPushButton { }");
    highlighter.rehighlightAndTrack();
    
    QColor commentColor = highlighter.commentFormat().foreground().color();
    bool foundComment = highlighter.hasFormatWithColor(commentColor);
    QVERIFY2(foundComment, "Comment should be highlighted");
}

void TestQssSyntaxHighlighter::testMultilineCommentHighlighting()
{
    QTextDocument doc;
    TestableHighlighter highlighter(&doc);
    doc.setPlainText("/* Line 1\nLine 2\nLine 3 */ QPushButton { }");
    highlighter.rehighlightAndTrack();
    
    QColor commentColor = highlighter.commentFormat().foreground().color();
    bool foundComment = highlighter.hasFormatWithColor(commentColor);
    QVERIFY2(foundComment, "Multi-line comment should be highlighted");
}


/**
 * Property 2: Syntax Highlighting Coverage
 * 
 * For any QSS text containing selectors, properties, values, pseudo-states,
 * sub-controls, and comments, the QssSyntaxHighlighter should apply distinct
 * non-default formatting to each syntax element type, and different element
 * types should have different formats.
 */
void TestQssSyntaxHighlighter::testSyntaxHighlightingCoverage_data()
{
    QTest::addColumn<QString>("qssText");
    QTest::addColumn<bool>("hasSelector");
    QTest::addColumn<bool>("hasProperty");
    QTest::addColumn<bool>("hasPseudoState");
    QTest::addColumn<bool>("hasSubControl");
    QTest::addColumn<bool>("hasComment");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    QStringList selectors = {
        "QPushButton", "QLabel", "QLineEdit", "QComboBox",
        "QCheckBox", "QRadioButton", "QSlider", "QSpinBox",
        "QTextEdit", "QListView", "QTreeView", "QTableView",
        "QWidget", "QFrame", "QGroupBox", "QTabWidget"
    };
    
    QStringList properties = {
        "color", "background-color", "background", "border",
        "border-radius", "padding", "margin", "font-size",
        "font-weight", "min-width", "min-height"
    };
    
    QStringList values = {
        "red", "blue", "#ffffff", "#000000",
        "rgb(255, 128, 0)", "1px solid black",
        "10px", "bold", "none"
    };
    
    QStringList pseudoStates = {
        ":hover", ":pressed", ":disabled", ":checked",
        ":focus", ":selected", ":enabled", ":active"
    };
    
    QStringList subControls = {
        "::indicator", "::drop-down", "::handle", "::groove",
        "::chunk", "::branch", "::tab", "::pane"
    };
    
    for (int i = 0; i < 100; ++i) {
        QString qss;
        bool hasSelector = true;
        bool hasProperty = (rng->generate() % 2) == 0;
        bool hasPseudoState = (rng->generate() % 3) == 0;
        bool hasSubControl = (rng->generate() % 3) == 0;
        bool hasComment = (rng->generate() % 4) == 0;
        
        if (hasComment) {
            qss += "/* Comment */ ";
        }
        
        qss += selectors[rng->generate() % selectors.size()];
        
        if (hasPseudoState) {
            qss += pseudoStates[rng->generate() % pseudoStates.size()];
        }
        
        if (hasSubControl) {
            qss += subControls[rng->generate() % subControls.size()];
        }
        
        qss += " { ";
        
        if (hasProperty) {
            QString prop = properties[rng->generate() % properties.size()];
            QString val = values[rng->generate() % values.size()];
            qss += prop + ": " + val + "; ";
        }
        
        qss += "}";
        
        QTest::newRow(qPrintable(QString("random_qss_%1").arg(i))) 
            << qss << hasSelector << hasProperty << hasPseudoState << hasSubControl << hasComment;
    }
    
    QTest::newRow("selector_only") 
        << "QPushButton { }" << true << false << false << false << false;
    
    QTest::newRow("selector_with_property") 
        << "QLabel { color: red; }" << true << true << false << false << false;
    
    QTest::newRow("selector_with_pseudo_state") 
        << "QPushButton:hover { }" << true << false << true << false << false;
    
    QTest::newRow("selector_with_sub_control") 
        << "QComboBox::drop-down { }" << true << false << false << true << false;
    
    QTest::newRow("comment_only") 
        << "/* This is a comment */" << false << false << false << false << true;
    
    QTest::newRow("all_elements") 
        << "/* Comment */ QPushButton:hover::indicator { color: red; }" 
        << true << true << true << true << true;
    
    QTest::newRow("multiple_pseudo_states") 
        << "QPushButton:hover:pressed { background: blue; }" 
        << true << true << true << false << false;
    
    QTest::newRow("complex_selector") 
        << "QTabWidget::pane { border: 1px solid gray; }" 
        << true << true << false << true << false;
}

void TestQssSyntaxHighlighter::testSyntaxHighlightingCoverage()
{
    // Feature: qtvanity, Property 2: Syntax Highlighting Coverage
    
    QFETCH(QString, qssText);
    QFETCH(bool, hasSelector);
    QFETCH(bool, hasProperty);
    QFETCH(bool, hasPseudoState);
    QFETCH(bool, hasSubControl);
    QFETCH(bool, hasComment);
    
    QTextDocument doc;
    TestableHighlighter highlighter(&doc);
    doc.setPlainText(qssText);
    highlighter.rehighlightAndTrack();
    
    QColor selectorColor = highlighter.selectorFormat().foreground().color();
    QColor propertyColor = highlighter.propertyFormat().foreground().color();
    QColor pseudoStateColor = highlighter.pseudoStateFormat().foreground().color();
    QColor subControlColor = highlighter.subControlFormat().foreground().color();
    QColor commentColor = highlighter.commentFormat().foreground().color();
    
    bool foundSelectorFormat = highlighter.hasFormatWithColor(selectorColor);
    bool foundPropertyFormat = highlighter.hasFormatWithColor(propertyColor);
    bool foundPseudoStateFormat = highlighter.hasFormatWithColor(pseudoStateColor);
    bool foundSubControlFormat = highlighter.hasFormatWithColor(subControlColor);
    bool foundCommentFormat = highlighter.hasFormatWithColor(commentColor);
    
    if (hasSelector) {
        QVERIFY2(foundSelectorFormat, 
            qPrintable(QString("Selector should be highlighted in: %1").arg(qssText)));
    }
    
    if (hasProperty) {
        QVERIFY2(foundPropertyFormat, 
            qPrintable(QString("Property should be highlighted in: %1").arg(qssText)));
    }
    
    if (hasPseudoState) {
        QVERIFY2(foundPseudoStateFormat, 
            qPrintable(QString("Pseudo-state should be highlighted in: %1").arg(qssText)));
    }
    
    if (hasSubControl) {
        QVERIFY2(foundSubControlFormat, 
            qPrintable(QString("Sub-control should be highlighted in: %1").arg(qssText)));
    }
    
    if (hasComment) {
        QVERIFY2(foundCommentFormat, 
            qPrintable(QString("Comment should be highlighted in: %1").arg(qssText)));
    }
    
    // Verify that different element types have different formats
    QVERIFY2(selectorColor != propertyColor, "Selector and property colors should be different");
    QVERIFY2(selectorColor != pseudoStateColor, "Selector and pseudo-state colors should be different");
    QVERIFY2(selectorColor != subControlColor, "Selector and sub-control colors should be different");
    QVERIFY2(selectorColor != commentColor, "Selector and comment colors should be different");
    QVERIFY2(propertyColor != pseudoStateColor, "Property and pseudo-state colors should be different");
    QVERIFY2(propertyColor != subControlColor, "Property and sub-control colors should be different");
    QVERIFY2(propertyColor != commentColor, "Property and comment colors should be different");
    QVERIFY2(pseudoStateColor != subControlColor, "Pseudo-state and sub-control colors should be different");
    QVERIFY2(pseudoStateColor != commentColor, "Pseudo-state and comment colors should be different");
    QVERIFY2(subControlColor != commentColor, "Sub-control and comment colors should be different");
}

void TestQssSyntaxHighlighter::testColorSchemeSwitch()
{
    QTextDocument doc;
    QssSyntaxHighlighter highlighter(&doc);
    doc.setPlainText("QPushButton { color: red; }");
    
    // Default is dark scheme
    QCOMPARE(highlighter.colorScheme(), QssSyntaxHighlighter::DarkScheme);
    QColor darkSelectorColor = highlighter.selectorFormat().foreground().color();
    
    // Switch to light scheme
    highlighter.setColorScheme(QssSyntaxHighlighter::LightScheme);
    QCOMPARE(highlighter.colorScheme(), QssSyntaxHighlighter::LightScheme);
    QColor lightSelectorColor = highlighter.selectorFormat().foreground().color();
    
    // Colors should be different between schemes
    QVERIFY2(darkSelectorColor != lightSelectorColor, 
        "Dark and light scheme selector colors should be different");
    
    // Switch back to dark scheme
    highlighter.setColorScheme(QssSyntaxHighlighter::DarkScheme);
    QCOMPARE(highlighter.colorScheme(), QssSyntaxHighlighter::DarkScheme);
    QColor darkSelectorColorAgain = highlighter.selectorFormat().foreground().color();
    
    // Should be same as original dark color
    QCOMPARE(darkSelectorColor, darkSelectorColorAgain);
}
