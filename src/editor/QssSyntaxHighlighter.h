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
 * Supports both light and dark color schemes.
 * 
 * Reference: qss-docs.md for complete QSS syntax specification.
 */
class QssSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    /**
     * @brief Color scheme for syntax highlighting.
     */
    enum ColorScheme {
        LightScheme,  ///< Colors optimized for light backgrounds
        DarkScheme    ///< Colors optimized for dark backgrounds
    };

    /**
     * @brief Constructs a QssSyntaxHighlighter.
     * @param parent The QTextDocument to highlight.
     */
    explicit QssSyntaxHighlighter(QTextDocument *parent = nullptr);

    /**
     * @brief Sets the color scheme for syntax highlighting.
     * @param scheme The color scheme to use.
     */
    void setColorScheme(ColorScheme scheme);

    /**
     * @brief Returns the current color scheme.
     */
    ColorScheme colorScheme() const { return m_colorScheme; }

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

    /**
     * @brief Returns the format used for variable references.
     */
    QTextCharFormat variableFormat() const { return m_variableFormat; }

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
        QTextCharFormat *format;  // Pointer to allow format updates
    };

    void setupFormats();
    void setupDarkFormats();
    void setupLightFormats();
    void setupRules();
    void highlightMultilineComments(const QString &text);

    ColorScheme m_colorScheme;
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
    QTextCharFormat m_variableFormat;

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
