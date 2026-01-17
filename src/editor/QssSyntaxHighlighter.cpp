#include "QssSyntaxHighlighter.h"

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
    HighlightingRule rule;

    // Sub-controls (must be before selectors to match :: properly)
    // Complete list from Qt documentation
    QString subControls = QStringLiteral(
        "::add-line|::add-page|::branch|::chunk|::close-button|::corner|"
        "::down-arrow|::down-button|::drop-down|::float-button|::groove|"
        "::indicator|::handle|::icon|::item|::left-arrow|::left-corner|"
        "::menu-arrow|::menu-button|::menu-indicator|::right-arrow|::pane|"
        "::right-corner|::scroller|::section|::separator|::sub-line|"
        "::sub-page|::tab|::tab-bar|::tear|::tearoff|::text|::title|"
        "::up-arrow|::up-button"
    );
    rule.pattern = QRegularExpression(subControls);
    rule.format = &m_subControlFormat;
    m_rules.append(rule);

    // Pseudo-states (must be before selectors)
    // Complete list from Qt documentation
    QString pseudoStates = QStringLiteral(
        ":active|:adjoins-item|:alternate|:bottom|:checked|:closable|"
        ":closed|:default|:disabled|:editable|:edit-focus|:enabled|"
        ":exclusive|:first|:flat|:floatable|:focus|:has-children|"
        ":has-siblings|:horizontal|:hover|:indeterminate|:last|:left|"
        ":maximized|:middle|:minimized|:movable|:no-frame|:non-exclusive|"
        ":off|:on|:only-one|:open|:next-selected|:pressed|"
        ":previous-selected|:read-only|:right|:selected|:top|"
        ":unchecked|:vertical|:window"
    );
    // Match pseudo-states - allow when followed by :: (sub-control) but not single : (another pseudo-state)
    // (?!:[^:]) means "not followed by a colon that isn't followed by another colon"
    rule.pattern = QRegularExpression(QStringLiteral("(?<!:)(") + pseudoStates + QStringLiteral(")(?!:[^:])"));
    rule.format = &m_pseudoStateFormat;
    m_rules.append(rule);

    // Selectors - Qt widget type names
    // Common Qt widgets that can be styled
    QString selectors = QStringLiteral(
        "\\bQAbstractScrollArea\\b|\\bQAbstractSpinBox\\b|\\bQAbstractItemView\\b|"
        "\\bQCheckBox\\b|\\bQColumnView\\b|\\bQComboBox\\b|"
        "\\bQDateEdit\\b|\\bQDateTimeEdit\\b|\\bQDialog\\b|\\bQDialogButtonBox\\b|"
        "\\bQDockWidget\\b|\\bQDoubleSpinBox\\b|"
        "\\bQFrame\\b|\\bQGroupBox\\b|\\bQHeaderView\\b|"
        "\\bQLabel\\b|\\bQLineEdit\\b|\\bQListView\\b|\\bQListWidget\\b|"
        "\\bQMainWindow\\b|\\bQMenu\\b|\\bQMenuBar\\b|\\bQMessageBox\\b|"
        "\\bQProgressBar\\b|\\bQPushButton\\b|\\bQRadioButton\\b|"
        "\\bQScrollArea\\b|\\bQScrollBar\\b|\\bQSizeGrip\\b|\\bQSlider\\b|"
        "\\bQSpinBox\\b|\\bQSplitter\\b|\\bQStatusBar\\b|"
        "\\bQTabBar\\b|\\bQTabWidget\\b|\\bQTableView\\b|\\bQTableWidget\\b|"
        "\\bQTextEdit\\b|\\bQTimeEdit\\b|\\bQToolBar\\b|\\bQToolBox\\b|"
        "\\bQToolButton\\b|\\bQToolTip\\b|\\bQTreeView\\b|\\bQTreeWidget\\b|"
        "\\bQWidget\\b|\\bQAbstractButton\\b|\\bQPlainTextEdit\\b|"
        "\\bQCalendarWidget\\b|\\bQFontComboBox\\b|\\bQMdiArea\\b|\\bQMdiSubWindow\\b"
    );
    rule.pattern = QRegularExpression(selectors);
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // Universal selector
    rule.pattern = QRegularExpression(QStringLiteral("(?<![\\w#.])\\*(?![\\w])"));
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // ID selector (#objectName)
    rule.pattern = QRegularExpression(QStringLiteral("#[a-zA-Z_][a-zA-Z0-9_]*"));
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // Class selector (.className)
    rule.pattern = QRegularExpression(QStringLiteral("\\.[A-Z][a-zA-Z0-9]*"));
    rule.format = &m_selectorFormat;
    m_rules.append(rule);

    // Property names (inside braces, before colon)
    // Common QSS properties from documentation
    QString properties = QStringLiteral(
        "\\b(background|background-color|background-image|background-repeat|"
        "background-position|background-attachment|background-clip|background-origin|"
        "alternate-background-color|accent-color|"
        "border|border-color|border-width|border-style|border-radius|border-image|"
        "border-top|border-right|border-bottom|border-left|"
        "border-top-color|border-right-color|border-bottom-color|border-left-color|"
        "border-top-width|border-right-width|border-bottom-width|border-left-width|"
        "border-top-style|border-right-style|border-bottom-style|border-left-style|"
        "border-top-left-radius|border-top-right-radius|"
        "border-bottom-left-radius|border-bottom-right-radius|"
        "margin|margin-top|margin-right|margin-bottom|margin-left|"
        "padding|padding-top|padding-right|padding-bottom|padding-left|"
        "spacing|min-width|min-height|max-width|max-height|width|height|"
        "font|font-family|font-size|font-weight|font-style|"
        "color|selection-color|selection-background-color|"
        "placeholder-text-color|gridline-color|"
        "position|top|left|right|bottom|"
        "subcontrol-origin|subcontrol-position|"
        "image|image-position|icon|icon-size|opacity|"
        "text-align|text-decoration|letter-spacing|word-spacing|"
        "outline|outline-color|outline-offset|outline-style|outline-radius|"
        "outline-bottom-left-radius|outline-bottom-right-radius|"
        "outline-top-left-radius|outline-top-right-radius|"
        "lineedit-password-character|lineedit-password-mask-delay|"
        "messagebox-text-interaction-flags|show-decoration-selected|"
        "button-layout|dialogbuttonbox-buttons-have-icons|"
        "titlebar-show-tooltips-on-buttons|widget-animation-duration|"
        "paint-alternating-row-colors-for-empty-area|"
        "-qt-background-role|-qt-style-features)\\s*:"
    );
    rule.pattern = QRegularExpression(properties);
    rule.format = &m_propertyFormat;
    m_rules.append(rule);

    // Color values - hex colors
    rule.pattern = QRegularExpression(QStringLiteral("#[0-9a-fA-F]{3,8}\\b"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Color values - rgb(), rgba(), hsv(), hsva(), hsl(), hsla()
    rule.pattern = QRegularExpression(QStringLiteral("\\b(rgb|rgba|hsv|hsva|hsl|hsla)\\s*\\([^)]*\\)"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Gradient values - qlineargradient, qradialgradient, qconicalgradient
    rule.pattern = QRegularExpression(QStringLiteral("\\b(qlineargradient|qradialgradient|qconicalgradient)\\s*\\([^)]*\\)"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // URL values
    rule.pattern = QRegularExpression(QStringLiteral("\\burl\\s*\\([^)]*\\)"));
    rule.format = &m_stringFormat;
    m_rules.append(rule);

    // Palette values
    rule.pattern = QRegularExpression(QStringLiteral("\\bpalette\\s*\\([^)]*\\)"));
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Named colors (common CSS color names)
    QString namedColors = QStringLiteral(
        "\\b(transparent|black|white|red|green|blue|yellow|cyan|magenta|"
        "gray|grey|darkgray|darkgrey|lightgray|lightgrey|"
        "darkred|darkgreen|darkblue|darkcyan|darkmagenta|darkyellow|"
        "orange|pink|purple|brown|navy|teal|olive|maroon|aqua|fuchsia|lime|silver)\\b"
    );
    rule.pattern = QRegularExpression(namedColors, QRegularExpression::CaseInsensitiveOption);
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Border style values
    QString borderStyles = QStringLiteral(
        "\\b(none|solid|dashed|dotted|double|groove|ridge|inset|outset|"
        "dot-dash|dot-dot-dash)\\b"
    );
    rule.pattern = QRegularExpression(borderStyles);
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Font weight/style values
    QString fontValues = QStringLiteral(
        "\\b(normal|bold|italic|oblique|underline|overline|line-through)\\b"
    );
    rule.pattern = QRegularExpression(fontValues);
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Position/alignment values
    QString positionValues = QStringLiteral(
        "\\b(relative|absolute|top|bottom|left|right|center|"
        "margin|border|padding|content|"
        "repeat|repeat-x|repeat-y|no-repeat|"
        "scroll|fixed)\\b"
    );
    rule.pattern = QRegularExpression(positionValues);
    rule.format = &m_valueFormat;
    m_rules.append(rule);

    // Numbers with units
    rule.pattern = QRegularExpression(QStringLiteral("-?\\d+(\\.\\d+)?\\s*(px|pt|em|ex|%)?\\b"));
    rule.format = &m_numberFormat;
    m_rules.append(rule);

    // Strings in quotes
    rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\"|'[^']*'"));
    rule.format = &m_stringFormat;
    m_rules.append(rule);

    // Variable references - ${variable_name}
    // Pattern: \$\{[a-zA-Z_][a-zA-Z0-9_-]*\}
    rule.pattern = QRegularExpression(QStringLiteral("\\$\\{[a-zA-Z_][a-zA-Z0-9_-]*\\}"));
    rule.format = &m_variableFormat;
    m_rules.append(rule);
}

void QssSyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply single-line rules
    for (const HighlightingRule &rule : qAsConst(m_rules)) {
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
