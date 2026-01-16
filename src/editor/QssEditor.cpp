#include "QssEditor.h"
#include "QssSyntaxHighlighter.h"

#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>

QssEditor::QssEditor(QWidget *parent)
    : QWidget(parent)
    , m_textEdit(nullptr)
    , m_highlighter(nullptr)
    , m_applyButton(nullptr)
    , m_toggleButton(nullptr)
    , m_autoApplyCheckbox(nullptr)
    , m_styleCombo(nullptr)
    , m_autoApplyTimer(nullptr)
    , m_hasUnsavedChanges(false)
    , m_customStyleActive(false)
    , m_autoApplyDelay(DEFAULT_AUTO_APPLY_DELAY_MS)
    , m_isApplying(false)
{
    setupUi();
    setupConnections();
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
    
    // Set a monospace font for code editing
    QFont font("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(10);
    m_textEdit->setFont(font);

    // Attach syntax highlighter
    m_highlighter = new QssSyntaxHighlighter(m_textEdit->document());

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

    // Create Toggle button for switching between Custom and Default styles
    m_toggleButton = new QPushButton(tr("Default"), this);
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

QString QssEditor::styleSheet() const
{
    return m_textEdit->toPlainText();
}

void QssEditor::setStyleSheet(const QString &qss)
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
    
    // Store cursor position before applying
    int cursorPosition = m_textEdit->textCursor().position();
    
    // Set flag to indicate we're applying (for cursor preservation)
    m_isApplying = true;
    
    // Emit the apply request
    emit applyRequested(m_textEdit->toPlainText());
    
    // Restore cursor position
    QTextCursor cursor = m_textEdit->textCursor();
    int maxPos = m_textEdit->toPlainText().length();
    cursor.setPosition(qMin(cursorPosition, maxPos));
    m_textEdit->setTextCursor(cursor);
    
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

void QssEditor::insertVariableReference(const QString &name)
{
    if (name.isEmpty()) {
        return;
    }
    
    // Format the variable reference as ${name}
    QString reference = QStringLiteral("${%1}").arg(name);
    
    // Insert at current cursor position
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.insertText(reference);
    
    // Verify the text edit has focus after insertion
    m_textEdit->setFocus();
}
