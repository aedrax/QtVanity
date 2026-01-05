#include "QssEditor.h"
#include "QssSyntaxHighlighter.h"

#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>

QssEditor::QssEditor(QWidget *parent)
    : QWidget(parent)
    , m_textEdit(nullptr)
    , m_highlighter(nullptr)
    , m_applyButton(nullptr)
    , m_autoApplyCheckbox(nullptr)
    , m_autoApplyTimer(nullptr)
    , m_hasUnsavedChanges(false)
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

    // Add widgets to button layout
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_autoApplyCheckbox);
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

void QssEditor::apply()
{
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
    if (!checked) {
        // Stop any pending auto-apply
        m_autoApplyTimer->stop();
    }
}
