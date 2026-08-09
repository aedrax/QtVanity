#include "QssSyntaxHighlighter.h"
#include "QssVocabulary.h"

#include <algorithm>

namespace {

/**
 * @brief Joins words into a regex alternation, longest first.
 *
 * Ordering matters: with "border|border-color" the engine would match the
 * "border" prefix of "border-color" and stop.
 */
QString alternation(const QStringList &words)
{
    QStringList sorted = words;
    std::sort(sorted.begin(), sorted.end(),
              [](const QString &a, const QString &b) {
                  return a.size() != b.size() ? a.size() > b.size() : a < b;
              });
    QStringList escaped;
    escaped.reserve(sorted.size());
    for (const QString &word : sorted) {
        escaped.append(QRegularExpression::escape(word));
    }
    return escaped.join(QLatin1Char('|'));
}

} // namespace

QssSyntaxHighlighter::QssSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
    , m_colorScheme(DarkScheme)
{
    setupFormats();
    setupRules();

    // Multi-line comment patterns
    m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    m_commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void QssSyntaxHighlighter::setColorScheme(ColorScheme scheme)
{
    if (m_colorScheme != scheme) {
        m_colorScheme = scheme;
        setupFormats();
        rehighlight();
    }
}

void QssSyntaxHighlighter::setupFormats()
{
    if (m_colorScheme == DarkScheme) {
        setupDarkFormats();
    } else {
        setupLightFormats();
    }
}

void QssSyntaxHighlighter::setupDarkFormats()
{
    // Selector format - widget type names (bright sky blue, bold)
    m_selectorFormat.setForeground(QColor(86, 156, 214));  // VS Code blue
    m_selectorFormat.setFontWeight(QFont::Bold);

    // Property format - CSS-like properties (light cyan)
    m_propertyFormat.setForeground(QColor(156, 220, 254));

    // Value format - property values (soft green)
    m_valueFormat.setForeground(QColor(181, 206, 168));  // Soft sage green

    // Pseudo-state format - :hover, :pressed, etc. (soft purple/lavender)
    m_pseudoStateFormat.setForeground(QColor(197, 134, 192));  // Soft magenta
    m_pseudoStateFormat.setFontWeight(QFont::Bold);

    // Sub-control format - ::indicator, ::handle, etc. (teal/cyan)
    m_subControlFormat.setForeground(QColor(78, 201, 176));  // Teal
    m_subControlFormat.setFontWeight(QFont::Bold);

    // Comment format - /* ... */ (muted gray, italic)
    m_commentFormat.setForeground(QColor(106, 153, 85));  // Soft olive green
    m_commentFormat.setFontItalic(true);

    // String format - "strings" and url() (warm orange)
    m_stringFormat.setForeground(QColor(206, 145, 120));  // Soft peach/coral

    // Number format - numbers with units (light green)
    m_numberFormat.setForeground(QColor(184, 215, 163));  // Pale lime

    // Variable reference format - ${variable_name} (bright pink/magenta)
    m_variableFormat.setForeground(QColor(220, 130, 180));  // Soft pink
    m_variableFormat.setFontWeight(QFont::Bold);
}

void QssSyntaxHighlighter::setupLightFormats()
{
    // Selector format - widget type names (deep blue, bold)
    m_selectorFormat.setForeground(QColor(0, 0, 180));
    m_selectorFormat.setFontWeight(QFont::Bold);

    // Property format - CSS-like properties (dark magenta/maroon)
    m_propertyFormat.setForeground(QColor(127, 0, 85));

    // Value format - property values (dark green)
    m_valueFormat.setForeground(QColor(0, 128, 0));

    // Pseudo-state format - :hover, :pressed, etc. (purple)
    m_pseudoStateFormat.setForeground(QColor(128, 0, 128));
    m_pseudoStateFormat.setFontWeight(QFont::Bold);

    // Sub-control format - ::indicator, ::handle, etc. (dark cyan/teal)
    m_subControlFormat.setForeground(QColor(0, 128, 128));
    m_subControlFormat.setFontWeight(QFont::Bold);

    // Comment format - /* ... */ (gray, italic)
    m_commentFormat.setForeground(QColor(96, 96, 96));
    m_commentFormat.setFontItalic(true);

    // String format - "strings" and url() (dark orange/brown)
    m_stringFormat.setForeground(QColor(163, 21, 21));

    // Number format - numbers with units (dark blue)
    m_numberFormat.setForeground(QColor(9, 134, 88));

    // Variable reference format - ${variable_name} (magenta)
    m_variableFormat.setForeground(QColor(199, 21, 133));
    m_variableFormat.setFontWeight(QFont::Bold);
}

void QssSyntaxHighlighter::setupRules()
{
    m_rules.clear();
    HighlightingRule rule;

    // Rule order is significant: setFormat() overwrites, so a later rule wins
    // over an earlier one on the same characters. Several words appear in more
    // than one list - "border", "padding" and "top" are both properties and
    // value keywords - so the value rules run first and the property rule runs
    // last, letting the property spelling (which requires a trailing colon)
    // reclaim its own tokens.

    // --- Structure -------------------------------------------------------

    // Sub-controls, before selectors so the "::" form matches first
    rule.pattern = QRegularExpression(QStringLiteral("::(") +
                                      alternation(QssVocabulary::subControls()) +
                                      QStringLiteral(")\\b"));
    rule.format = &m_subControlFormat;
    m_rules.append(rule);

    // Pseudo-states: a single colon not part of a "::" sub-control, optionally
    // negated. Chained states (":hover:pressed") and a state followed by a
    // sub-control (":enabled::groove") both highlight.
    rule.pattern = QRegularExpression(QStringLiteral("(?<!:):!?(") +
                                      alternation(QssVocabulary::pseudoStates()) +
                                      QStringLiteral(")\\b"));
    rule.format = &m_pseudoStateFormat;
    m_rules.append(rule);

    // Qt widget type names
    rule.pattern = QRegularExpression(QStringLiteral("\\b(") +
                                      alternation(QssVocabulary::selectors()) +
                                      QStringLiteral(")\\b"));
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // Universal selector
    rule.pattern = QRegularExpression(QStringLiteral("(?<![\\w#.])\\*(?![\\w])"));
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // ID selector (#objectName). Restricted to names starting with a letter or
    // underscore so it cannot swallow a hex colour such as #1e1e1e.
    rule.pattern = QRegularExpression(QStringLiteral("#[a-zA-Z_][a-zA-Z0-9_-]*"));
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // Class selector (.className)
    rule.pattern = QRegularExpression(QStringLiteral("\\.[A-Z][a-zA-Z0-9]*"));
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // --- Values ----------------------------------------------------------

    // Numbers with optional units. The lookbehind keeps this off the digits of
    // a hex colour, which would otherwise be recoloured as a number.
    rule.pattern = QRegularExpression(
        QStringLiteral("(?<![#\\w-])-?\\d+(\\.\\d+)?\\s*(px|pt|em|ex|%)?\\b"));
    rule.format = &m_numberFormat;
    m_rules.append(rule);

    // Value keywords: named colours, border styles, font and position values
    rule.pattern = QRegularExpression(QStringLiteral("\\b(") +
                                      alternation(QssVocabulary::valueKeywords()) +
                                      QStringLiteral(")\\b"),
                                      QRegularExpression::CaseInsensitiveOption);
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Colour functions
    rule.pattern = QRegularExpression(
        QStringLiteral("\\b(rgb|rgba|hsv|hsva|hsl|hsla)\\s*\\([^)]*\\)"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Gradients
    rule.pattern = QRegularExpression(
        QStringLiteral("\\b(qlineargradient|qradialgradient|qconicalgradient)\\s*\\([^)]*\\)"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Palette references
    rule.pattern = QRegularExpression(QStringLiteral("\\bpalette\\s*\\([^)]*\\)"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Hex colours, after the number rule so they keep the value colour
    rule.pattern = QRegularExpression(QStringLiteral("#[0-9a-fA-F]{3,8}\\b"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Strings and url()
    rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\"|'[^']*'"));
    rule.format = &m_stringFormat;
    m_rules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\burl\\s*\\([^)]*\\)"));
    rule.format = &m_stringFormat;
    m_rules.append(rule);

    // --- Properties, last so they win their own tokens -------------------

    rule.pattern = QRegularExpression(QStringLiteral("\\b(") +
                                      alternation(QssVocabulary::properties()) +
                                      QStringLiteral(")\\s*(?=:)"));
    rule.format = &m_propertyFormat;
    m_rules.append(rule);

    // Variable references, last of all: ${name} is never anything else
    rule.pattern = QRegularExpression(QStringLiteral("\\$\\{[a-zA-Z_][a-zA-Z0-9_-]*\\}"));
    rule.format = &m_variableFormat;
    m_rules.append(rule);
}

void QssSyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply single-line rules
    for (const HighlightingRule &rule : std::as_const(m_rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), *rule.format);
        }
    }

    // Handle multi-line comments
    highlightMultilineComments(text);
}

void QssSyntaxHighlighter::highlightMultilineComments(const QString &text)
{
    setCurrentBlockState(Normal);

    int startIndex = 0;
    if (previousBlockState() != InComment) {
        // Not in a comment from previous block, look for comment start
        QRegularExpressionMatch startMatch = m_commentStartExpression.match(text);
        startIndex = startMatch.hasMatch() ? startMatch.capturedStart() : -1;
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        int searchStart = (previousBlockState() == InComment && startIndex == 0) 
                          ? 0 
                          : startIndex + 2;
        
        endMatch = m_commentEndExpression.match(text, searchStart);
        
        int endIndex = endMatch.hasMatch() ? endMatch.capturedStart() : -1;
        int commentLength;

        if (endIndex == -1) {
            // Comment extends to end of block
            setCurrentBlockState(InComment);
            commentLength = text.length() - startIndex;
        } else {
            // Comment ends in this block
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }

        setFormat(startIndex, commentLength, m_commentFormat);

        // Look for next comment start
        QRegularExpressionMatch nextStartMatch = m_commentStartExpression.match(text, startIndex + commentLength);
        startIndex = nextStartMatch.hasMatch() ? nextStartMatch.capturedStart() : -1;
    }

    // If we started in a comment and didn't find an end, we're still in a comment
    if (previousBlockState() == InComment && currentBlockState() != InComment) {
        // Check if we actually found an end
        QRegularExpressionMatch endMatch = m_commentEndExpression.match(text);
        if (!endMatch.hasMatch()) {
            setCurrentBlockState(InComment);
            setFormat(0, text.length(), m_commentFormat);
        }
    }
}
