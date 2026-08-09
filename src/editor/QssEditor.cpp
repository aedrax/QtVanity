#include "QssEditor.h"
#include "QssSyntaxHighlighter.h"
#include "ColorSwatchOverlay.h"
#include "FindReplaceBar.h"

#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QFontDatabase>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QShortcut>

QssEditor::QssEditor(QWidget *parent)
    : QWidget(parent)
    , m_textEdit(nullptr)
    , m_highlighter(nullptr)
    , m_colorSwatchOverlay(nullptr)
    , m_findReplaceBar(nullptr)
    , m_applyButton(nullptr)
    , m_toggleButton(nullptr)
    , m_autoApplyCheckbox(nullptr)
    , m_styleCombo(nullptr)
    , m_autoApplyTimer(nullptr)
    , m_hasUnsavedChanges(false)
    // The editor starts live: typing applies. Starting in Default mode makes the
    // most visible feature of the app inert until the user finds the toggle.
    , m_customStyleActive(true)
    , m_autoApplyDelay(DEFAULT_AUTO_APPLY_DELAY_MS)
    , m_isApplying(false)
{
    setupUi();
    setupConnections();
    setupFindReplaceShortcuts();
}

QssEditor::~QssEditor()
{
    // Qt handles child widget deletion
}

void QssEditor::setupUi()
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(4);

    // Create text edit for QSS code input
    m_textEdit = new QTextEdit(this);
    m_textEdit->setAcceptRichText(false);
    m_textEdit->setPlaceholderText(tr("Enter QSS code here..."));
    m_textEdit->setTabStopDistance(40); // 4 spaces equivalent
    
    // Use the platform's fixed-pitch font. "Monospace" is an X11 alias that
    // does not resolve on macOS or Windows, forcing a fallback lookup.
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setStyleHint(QFont::Monospace);
    m_textEdit->setFont(font);

    // Attach syntax highlighter
    m_highlighter = new QssSyntaxHighlighter(m_textEdit->document());

    // Create color swatch overlay for inline color picking
    m_colorSwatchOverlay = new ColorSwatchOverlay(m_textEdit, m_textEdit->viewport());
    m_colorSwatchOverlay->show();
    
    // Ensure overlay is excluded from global stylesheet effects
    m_colorSwatchOverlay->setStyleSheet(QString());

    // Create FindReplaceBar (initially hidden)
    m_findReplaceBar = new FindReplaceBar(m_textEdit, this);

    // Create button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);

    // Create Apply button
    m_applyButton = new QPushButton(tr("Apply"), this);
    m_applyButton->setToolTip(tr("Apply the stylesheet to the application"));

    // Create Auto-Apply checkbox
    m_autoApplyCheckbox = new QCheckBox(tr("Auto-Apply"), this);
    m_autoApplyCheckbox->setToolTip(
        tr("Automatically apply styles after a brief delay when text changes"));
    m_autoApplyCheckbox->setChecked(false);

    // Create Toggle button for switching between Custom and Default styles.
    // The label reflects the mode currently in effect.
    m_toggleButton = new QPushButton(m_customStyleActive ? tr("Custom") : tr("Default"), this);
    m_toggleButton->setToolTip(tr("Toggle between custom QSS and default Qt style (Ctrl+T)"));

    // Create Style selector combo box
    m_styleCombo = new QComboBox(this);
    m_styleCombo->setToolTip(tr("Select the base QStyle for the application"));

    // Add widgets to button layout
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_autoApplyCheckbox);
    buttonLayout->addWidget(m_toggleButton);
    buttonLayout->addWidget(m_styleCombo);
    buttonLayout->addStretch();

    // Add widgets to main layout
    mainLayout->addWidget(m_textEdit, 1); // Text edit takes all available space
    mainLayout->addWidget(m_findReplaceBar); // FindReplaceBar between text edit and buttons
    mainLayout->addLayout(buttonLayout);

    // Create auto-apply timer
    m_autoApplyTimer = new QTimer(this);
    m_autoApplyTimer->setSingleShot(true);
}

void QssEditor::setupConnections()
{
    // Connect text changes
    connect(m_textEdit, &QTextEdit::textChanged,
            this, &QssEditor::onTextChanged);

    // Connect Apply button
    connect(m_applyButton, &QPushButton::clicked,
            this, &QssEditor::onApplyClicked);

    // Connect Auto-Apply checkbox
    connect(m_autoApplyCheckbox, &QCheckBox::toggled,
            this, &QssEditor::onAutoApplyToggled);

    // Connect auto-apply timer
    connect(m_autoApplyTimer, &QTimer::timeout,
            this, &QssEditor::onAutoApplyTimeout);

    // Connect Toggle button
    connect(m_toggleButton, &QPushButton::clicked,
            this, &QssEditor::toggleStyleMode);

    // Connect Style selector combo box
    connect(m_styleCombo, &QComboBox::currentTextChanged,
            this, [this](const QString &text) {
                // Strip "(Default)" suffix if present
                QString styleName = text;
                const QString defaultSuffix = QStringLiteral(" (Default)");
                if (styleName.endsWith(defaultSuffix)) {
                    styleName.chop(defaultSuffix.length());
                }
                emit styleChangeRequested(styleName);
            });
}

QString QssEditor::qssText() const
{
    return m_textEdit->toPlainText();
}

void QssEditor::setQssText(const QString &qss)
{
    // Block signals to avoid triggering unsaved changes
    m_textEdit->blockSignals(true);
    m_textEdit->setPlainText(qss);
    m_textEdit->blockSignals(false);
    
    // Reset unsaved changes state
    m_hasUnsavedChanges = false;
    emit unsavedChangesChanged(false);
}

bool QssEditor::hasUnsavedChanges() const
{
    return m_hasUnsavedChanges;
}

void QssEditor::markAsSaved()
{
    if (m_hasUnsavedChanges) {
        m_hasUnsavedChanges = false;
        emit unsavedChangesChanged(false);
    }
}

bool QssEditor::isAutoApplyEnabled() const
{
    return m_autoApplyCheckbox->isChecked();
}

void QssEditor::setAutoApplyEnabled(bool enabled)
{
    m_autoApplyCheckbox->setChecked(enabled);
}

int QssEditor::autoApplyDelay() const
{
    return m_autoApplyDelay;
}

void QssEditor::setAutoApplyDelay(int ms)
{
    m_autoApplyDelay = ms;
}

QTextEdit* QssEditor::textEdit() const
{
    return m_textEdit;
}

bool QssEditor::isCustomStyleActive() const
{
    return m_customStyleActive;
}

void QssEditor::setCustomStyleActive(bool customActive)
{
    if (m_customStyleActive == customActive) {
        return;
    }
    
    m_customStyleActive = customActive;
    
    // Update toggle button text
    if (m_toggleButton) {
        m_toggleButton->setText(m_customStyleActive ? tr("Custom") : tr("Default"));
    }
    
    emit styleModeChanged(m_customStyleActive);
    
    if (m_customStyleActive) {
        // Switching to custom mode - apply the editor content
        emit applyRequested(m_textEdit->toPlainText());
    } else {
        // Switching to default mode - request default style
        emit defaultStyleRequested();
    }
}

void QssEditor::toggleStyleMode()
{
    setCustomStyleActive(!m_customStyleActive);
}

void QssEditor::cancelPendingApply()
{
    if (m_autoApplyTimer) {
        m_autoApplyTimer->stop();
    }
}

void QssEditor::apply()
{
    // If in Default mode, switch to Custom mode first
    if (!m_customStyleActive) {
        m_customStyleActive = true;
        
        // Update toggle button text
        if (m_toggleButton) {
            m_toggleButton->setText(tr("Custom"));
        }
        
        emit styleModeChanged(true);
    }
    
    // Store the whole selection and the scroll offset, not just the caret.
    // Restoring position alone collapsed any selection the user had made,
    // which with Auto-Apply on happened while they were still working.
    const QTextCursor before = m_textEdit->textCursor();
    const int anchor = before.anchor();
    const int position = before.position();
    const int scrollValue = m_textEdit->verticalScrollBar()->value();

    // Set flag to indicate we're applying (for cursor preservation)
    m_isApplying = true;

    // Emit the apply request
    emit applyRequested(m_textEdit->toPlainText());

    // Restore selection and scroll position
    const int maxPos = m_textEdit->toPlainText().length();
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.setPosition(qMin(anchor, maxPos));
    cursor.setPosition(qMin(position, maxPos), QTextCursor::KeepAnchor);
    m_textEdit->setTextCursor(cursor);
    m_textEdit->verticalScrollBar()->setValue(scrollValue);

    m_isApplying = false;
}

void QssEditor::onTextChanged()
{
    // Update unsaved changes state
    if (!m_hasUnsavedChanges) {
        m_hasUnsavedChanges = true;
        emit unsavedChangesChanged(true);
    }
    
    // Emit contents changed signal
    emit contentsChanged();
    
    // Handle auto-apply
    if (isAutoApplyEnabled()) {
        // Reset and start the timer
        m_autoApplyTimer->stop();
        m_autoApplyTimer->start(m_autoApplyDelay);
    }
}

void QssEditor::onAutoApplyTimeout()
{
    // Auto-apply triggered - apply the stylesheet
    apply();
}

void QssEditor::onApplyClicked()
{
    apply();
}

void QssEditor::onAutoApplyToggled(bool checked)
{
    if (checked) {
        // Immediately apply the current QSS when auto-apply is enabled
        apply();
    } else {
        // Stop any pending auto-apply
        m_autoApplyTimer->stop();
    }
}

void QssEditor::setAvailableStyles(const QStringList &styles)
{
    // Block signals to avoid emitting styleChangeRequested during population
    m_styleCombo->blockSignals(true);
    
    m_styleCombo->clear();
    
    for (const QString &style : styles) {
        // Add with "(Default)" suffix if this is the default style
        if (!m_defaultStyleName.isEmpty() && 
            style.compare(m_defaultStyleName, Qt::CaseInsensitive) == 0) {
            m_styleCombo->addItem(style + QStringLiteral(" (Default)"));
        } else {
            m_styleCombo->addItem(style);
        }
    }
    
    m_styleCombo->blockSignals(false);
}

void QssEditor::setCurrentStyle(const QString &styleName)
{
    // Block signals to avoid emitting styleChangeRequested
    m_styleCombo->blockSignals(true);
    
    // Try to find the style with or without "(Default)" suffix
    int index = -1;
    for (int i = 0; i < m_styleCombo->count(); ++i) {
        QString itemText = m_styleCombo->itemText(i);
        QString itemStyle = itemText;
        const QString defaultSuffix = QStringLiteral(" (Default)");
        if (itemStyle.endsWith(defaultSuffix)) {
            itemStyle.chop(defaultSuffix.length());
        }
        
        if (itemStyle.compare(styleName, Qt::CaseInsensitive) == 0) {
            index = i;
            break;
        }
    }
    
    if (index >= 0) {
        m_styleCombo->setCurrentIndex(index);
    }
    
    m_styleCombo->blockSignals(false);
}

QString QssEditor::currentStyle() const
{
    QString text = m_styleCombo->currentText();
    const QString defaultSuffix = QStringLiteral(" (Default)");
    if (text.endsWith(defaultSuffix)) {
        text.chop(defaultSuffix.length());
    }
    return text;
}

void QssEditor::setDefaultStyleMarker(const QString &styleName)
{
    m_defaultStyleName = styleName;
    
    // Update the combo box items to reflect the new default marker
    // We need to re-populate if items already exist
    if (m_styleCombo->count() > 0) {
        QStringList currentStyles;
        for (int i = 0; i < m_styleCombo->count(); ++i) {
            QString itemText = m_styleCombo->itemText(i);
            const QString defaultSuffix = QStringLiteral(" (Default)");
            if (itemText.endsWith(defaultSuffix)) {
                itemText.chop(defaultSuffix.length());
            }
            currentStyles.append(itemText);
        }
        
        // Remember current selection
        QString currentSelection = currentStyle();
        
        // Re-populate with updated default marker
        setAvailableStyles(currentStyles);
        
        // Restore selection
        setCurrentStyle(currentSelection);
    }
}

void QssEditor::setDarkColorScheme(bool dark)
{
    if (m_highlighter) {
        m_highlighter->setColorScheme(dark ? QssSyntaxHighlighter::DarkScheme 
                                           : QssSyntaxHighlighter::LightScheme);
    }
    if (m_findReplaceBar) {
        m_findReplaceBar->setDarkTheme(dark);
    }
}

void QssEditor::setColorSwatchesEnabled(bool enabled)
{
    if (m_colorSwatchOverlay) {
        m_colorSwatchOverlay->setOverlayEnabled(enabled);
    }
}

bool QssEditor::colorSwatchesEnabled() const
{
    return m_colorSwatchOverlay ? m_colorSwatchOverlay->isOverlayEnabled() : false;
}

void QssEditor::showFindBar()
{
    if (!m_findReplaceBar) {
        return;
    }
    
    // Get selected text to initialize search
    QString selectedText = m_textEdit->textCursor().selectedText();
    
    if (!selectedText.isEmpty()) {
        // Use selected text as search query
        m_findReplaceBar->setSearchText(selectedText);
    }
    // If no selection, preserve previous query (setSearchText not called)
    
    m_findReplaceBar->showFindMode();
}

void QssEditor::showReplaceBar()
{
    if (!m_findReplaceBar) {
        return;
    }
    
    // Get selected text to initialize search
    QString selectedText = m_textEdit->textCursor().selectedText();
    
    if (!selectedText.isEmpty()) {
        // Use selected text as search query
        m_findReplaceBar->setSearchText(selectedText);
    }
    // If no selection, preserve previous query (setSearchText not called)
    
    m_findReplaceBar->showReplaceMode();
}

void QssEditor::hideFindReplaceBar()
{
    if (m_findReplaceBar) {
        m_findReplaceBar->hideBar();
    }
}

FindReplaceBar* QssEditor::findReplaceBar() const
{
    return m_findReplaceBar;
}

void QssEditor::refreshColorSwatches()
{
    if (m_colorSwatchOverlay) {
        // Ensure overlay geometry matches viewport after style changes
        m_colorSwatchOverlay->setGeometry(m_textEdit->viewport()->rect());
        // Force update of color positions and repaint
        m_colorSwatchOverlay->updateColors();
        m_colorSwatchOverlay->update();
    }
}

void QssEditor::setupFindReplaceShortcuts()
{
    // Ctrl+F - Show find bar
    QShortcut *findShortcut = new QShortcut(QKeySequence::Find, this);
    connect(findShortcut, &QShortcut::activated, this, &QssEditor::showFindBar);
    
    // Ctrl+H - Show replace bar
    QShortcut *replaceShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_H), this);
    connect(replaceShortcut, &QShortcut::activated, this, &QssEditor::showReplaceBar);
    
    // F3 - Find next
    QShortcut *findNextShortcut = new QShortcut(QKeySequence::FindNext, this);
    connect(findNextShortcut, &QShortcut::activated, this, [this]() {
        if (m_findReplaceBar && m_findReplaceBar->isBarVisible()) {
            m_findReplaceBar->findNext();
        }
    });
    
    // Shift+F3 - Find previous
    QShortcut *findPrevShortcut = new QShortcut(QKeySequence::FindPrevious, this);
    connect(findPrevShortcut, &QShortcut::activated, this, [this]() {
        if (m_findReplaceBar && m_findReplaceBar->isBarVisible()) {
            m_findReplaceBar->findPrevious();
        }
    });
    
    // Escape - Hide find/replace bar. Scoped to this widget and its children:
    // at window scope it swallowed Escape everywhere in the main window, even
    // with the bar hidden.
    QShortcut *escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    escapeShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(escapeShortcut, &QShortcut::activated, this, &QssEditor::hideFindReplaceBar);
}
