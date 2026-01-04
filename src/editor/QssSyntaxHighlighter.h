#ifndef QSSSYNTAXHIGHLIGHTER_H
#define QSSSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

/**
 * @brief Syntax highlighter for QSS (Qt Style Sheets) code.
 * 
 * Provides visual highlighting for:
 * - Selectors (widget names, class names, object names)
 * - Properties (CSS-like property names)
 * - Values (property values including colors, numbers, urls)
 * - Pseudo-states (:hover, :pressed, :disabled, etc.)
 * - Sub-controls (::indicator, ::handle, ::drop-down, etc.)
 * - Comments (block comments)
 * 
 * Reference: qss-docs.md for complete QSS syntax specification.
 */
class QssSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a QssSyntaxHighlighter.
     * @param parent The QTextDocument to highlight.
     */
    explicit QssSyntaxHighlighter(QTextDocument *parent = nullptr);

    /**
     * @brief Returns the format used for selectors.
     */
    QTextCharFormat selectorFormat() const { return m_selectorFormat; }

    /**
     * @brief Returns the format used for properties.
     */
    QTextCharFormat propertyFormat() const { return m_propertyFormat; }

    /**
     * @brief Returns the format used for values.
     */
    QTextCharFormat valueFormat() const { return m_valueFormat; }

    /**
     * @brief Returns the format used for pseudo-states.
     */
    QTextCharFormat pseudoStateFormat() const { return m_pseudoStateFormat; }

    /**
     * @brief Returns the format used for sub-controls.
     */
    QTextCharFormat subControlFormat() const { return m_subControlFormat; }

    /**
     * @brief Returns the format used for comments.
     */
    QTextCharFormat commentFormat() const { return m_commentFormat; }

protected:
    /**
     * @brief Highlights a single block of text.
     * @param text The text to highlight.
     */
    void highlightBlock(const QString &text) override;

private:
    /**
     * @brief Highlighting rule structure.
     */
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void setupFormats();
    void setupRules();
    void highlightMultilineComments(const QString &text);

    QVector<HighlightingRule> m_rules;

    // Text formats for different syntax elements
    QTextCharFormat m_selectorFormat;
    QTextCharFormat m_propertyFormat;
    QTextCharFormat m_valueFormat;
    QTextCharFormat m_pseudoStateFormat;
    QTextCharFormat m_subControlFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_numberFormat;

    // Multi-line comment handling
    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;

    // Block state for multi-line comments
    enum BlockState {
        Normal = 0,
        InComment = 1
    };
};

#endif // QSSSYNTAXHIGHLIGHTER_H
