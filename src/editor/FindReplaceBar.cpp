#include "FindReplaceBar.h"
#include "CodeEditor.h"

#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextDocument>
#include <QTextBlock>
#include <QApplication>

FindReplaceBar::FindReplaceBar(CodeEditor *editor, QWidget *parent)
    : QWidget(parent)
    , m_editor(editor)
    , m_searchInput(nullptr)
    , m_prevButton(nullptr)
    , m_nextButton(nullptr)
    , m_caseSensitiveButton(nullptr)
    , m_closeButton(nullptr)
    , m_matchCountLabel(nullptr)
    , m_replaceRow(nullptr)
    , m_replaceInput(nullptr)
    , m_replaceButton(nullptr)
    , m_replaceAllButton(nullptr)
    , m_currentMatchIndex(-1)
    , m_caseSensitive(false)
    , m_darkTheme(false)
    , m_replacing(false)
    , m_matchHighlightColor(QColor("#FFFF00"))        // Yellow for light theme
    , m_currentMatchHighlightColor(QColor("#FF9632")) // Orange for light theme
{
    setupUi();
    setupConnections();
    
    // Start hidden
    QWidget::hide();
}

FindReplaceBar::~FindReplaceBar()
{
    // Qt handles child widget deletion
}

void FindReplaceBar::setupUi()
{
    // Main vertical layout for find row and replace row
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // === Find Row ===
    QHBoxLayout *findLayout = new QHBoxLayout();
    findLayout->setContentsMargins(0, 0, 0, 0);
    findLayout->setSpacing(4);

    // Search input field
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText(tr("Find..."));
    m_searchInput->setClearButtonEnabled(true);
    m_searchInput->setMinimumWidth(200);

    // Previous button
    m_prevButton = new QPushButton(tr("◀"), this);
    m_prevButton->setToolTip(tr("Previous match (Shift+Enter)"));
    m_prevButton->setAccessibleName(tr("Previous match"));
    m_prevButton->setFixedWidth(30);
    m_prevButton->setEnabled(false);

    // Next button
    m_nextButton = new QPushButton(tr("▶"), this);
    m_nextButton->setToolTip(tr("Next match (Enter)"));
    m_nextButton->setAccessibleName(tr("Next match"));
    m_nextButton->setFixedWidth(30);
    m_nextButton->setEnabled(false);

    // Case sensitivity toggle button
    m_caseSensitiveButton = new QToolButton(this);
    m_caseSensitiveButton->setText(tr("Aa"));
    m_caseSensitiveButton->setToolTip(tr("Match Case"));
    m_caseSensitiveButton->setAccessibleName(tr("Match case"));
    m_caseSensitiveButton->setCheckable(true);
    m_caseSensitiveButton->setChecked(false);

    // Match count label
    m_matchCountLabel = new QLabel(this);
    m_matchCountLabel->setMinimumWidth(60);
    m_matchCountLabel->setAlignment(Qt::AlignCenter);

    // Close button
    m_closeButton = new QToolButton(this);
    m_closeButton->setText(tr("✕"));
    m_closeButton->setToolTip(tr("Close (Escape)"));
    m_closeButton->setAccessibleName(tr("Close find bar"));

    // Add widgets to find layout
    findLayout->addWidget(m_searchInput);
    findLayout->addWidget(m_prevButton);
    findLayout->addWidget(m_nextButton);
    findLayout->addWidget(m_caseSensitiveButton);
    findLayout->addWidget(m_matchCountLabel);
    findLayout->addStretch();
    findLayout->addWidget(m_closeButton);

    // === Replace Row ===
    m_replaceRow = new QWidget(this);
    QHBoxLayout *replaceLayout = new QHBoxLayout(m_replaceRow);
    replaceLayout->setContentsMargins(0, 0, 0, 0);
    replaceLayout->setSpacing(4);

    // Replace input field
    m_replaceInput = new QLineEdit(m_replaceRow);
    m_replaceInput->setPlaceholderText(tr("Replace with..."));
    m_replaceInput->setClearButtonEnabled(true);
    m_replaceInput->setMinimumWidth(200);

    // Replace button
    m_replaceButton = new QPushButton(tr("Replace"), m_replaceRow);
    m_replaceButton->setToolTip(tr("Replace current match"));
    m_replaceButton->setEnabled(false);

    // Replace All button
    m_replaceAllButton = new QPushButton(tr("Replace All"), m_replaceRow);
    m_replaceAllButton->setToolTip(tr("Replace all matches"));
    m_replaceAllButton->setEnabled(false);

    // Add widgets to replace layout
    replaceLayout->addWidget(m_replaceInput);
    replaceLayout->addWidget(m_replaceButton);
    replaceLayout->addWidget(m_replaceAllButton);
    replaceLayout->addStretch();

    // Add rows to main layout
    mainLayout->addLayout(findLayout);
    mainLayout->addWidget(m_replaceRow);

    // Initially hide replace row
    m_replaceRow->hide();
}

void FindReplaceBar::setupConnections()
{
    // Search input text changes
    connect(m_searchInput, &QLineEdit::textChanged,
            this, &FindReplaceBar::onSearchTextChanged);
    
    // Search input return pressed (Enter = next, Shift+Enter handled via event filter)
    connect(m_searchInput, &QLineEdit::returnPressed,
            this, &FindReplaceBar::findNext);

    // Navigation buttons
    connect(m_prevButton, &QPushButton::clicked,
            this, &FindReplaceBar::findPrevious);
    connect(m_nextButton, &QPushButton::clicked,
            this, &FindReplaceBar::findNext);

    // Case sensitivity toggle
    connect(m_caseSensitiveButton, &QToolButton::toggled,
            this, &FindReplaceBar::onCaseSensitivityChanged);

    // Close button
    connect(m_closeButton, &QToolButton::clicked,
            this, &FindReplaceBar::hideBar);

    // Replace buttons
    connect(m_replaceButton, &QPushButton::clicked,
            this, &FindReplaceBar::replaceCurrent);
    connect(m_replaceAllButton, &QPushButton::clicked,
            this, &FindReplaceBar::replaceAll);

    // Keep results current while the bar is open. Stored cursors follow edits,
    // but text typed after the search ran was never matched at all.
    if (m_editor && m_editor->document()) {
        connect(m_editor->document(), &QTextDocument::contentsChanged,
                this, &FindReplaceBar::onDocumentChanged);
    }
}

void FindReplaceBar::onDocumentChanged()
{
    // Ignore edits we are making ourselves, and any while the bar is hidden.
    if (m_replacing || !QWidget::isVisible() || m_searchInput->text().isEmpty()) {
        return;
    }

    const int caret = m_editor->textCursor().position();

    m_matches.clear();
    QString searchStr = m_searchInput->text();
    QTextDocument *doc = m_editor->document();
    QTextDocument::FindFlags flags;
    if (m_caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor cursor(doc);
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(searchStr, cursor, flags);
        if (!cursor.isNull()) {
            m_matches.append(cursor);
        }
    }

    // Re-anchor to where the user is rather than yanking them to match one.
    m_currentMatchIndex = m_matches.isEmpty() ? -1 : indexOfFirstMatchAtOrAfter(caret);

    updateMatchCountLabel();
    highlightMatches();
}

// === Mode Control ===

void FindReplaceBar::showFindMode()
{
    m_replaceRow->hide();
    QWidget::show();
    m_searchInput->setFocus();
    m_searchInput->selectAll();
    emit visibilityChanged(true);
}

void FindReplaceBar::showReplaceMode()
{
    m_replaceRow->show();
    QWidget::show();
    m_searchInput->setFocus();
    m_searchInput->selectAll();
    emit visibilityChanged(true);
}

void FindReplaceBar::hideBar()
{
    clearHighlights();
    QWidget::hide();
    
    // Return focus to editor
    if (m_editor) {
        m_editor->setFocus();
    }
    
    emit visibilityChanged(false);
}

bool FindReplaceBar::isBarVisible() const
{
    return QWidget::isVisible();
}

// === Search State ===

QString FindReplaceBar::searchText() const
{
    return m_searchInput->text();
}

void FindReplaceBar::setSearchText(const QString &text)
{
    m_searchInput->setText(text);
    m_searchInput->selectAll();
}

bool FindReplaceBar::isCaseSensitive() const
{
    return m_caseSensitive;
}

void FindReplaceBar::setCaseSensitive(bool sensitive)
{
    m_caseSensitive = sensitive;
    m_caseSensitiveButton->setChecked(sensitive);
}

// === Match Information ===

int FindReplaceBar::matchCount() const
{
    return m_matches.count();
}

int FindReplaceBar::currentMatchIndex() const
{
    return m_currentMatchIndex;
}

// === Theme Support ===

void FindReplaceBar::setDarkTheme(bool dark)
{
    m_darkTheme = dark;
    
    if (dark) {
        m_matchHighlightColor = QColor("#5C5C00");        // Dark yellow
        m_currentMatchHighlightColor = QColor("#8B5A00"); // Dark orange
    } else {
        m_matchHighlightColor = QColor("#FFFF00");        // Yellow
        m_currentMatchHighlightColor = QColor("#FF9632"); // Orange
    }
    
    // Re-apply highlights with new colors
    if (!m_matches.isEmpty()) {
        highlightMatches();
    }
}

// === Navigation Slots ===

void FindReplaceBar::findNext()
{
    if (m_matches.isEmpty()) {
        return;
    }
    
    // Move to next match with wrap-around
    m_currentMatchIndex = (m_currentMatchIndex + 1) % m_matches.count();
    
    selectCurrentMatch();
    scrollToCurrentMatch();
    updateMatchCountLabel();
    highlightMatches();
}

void FindReplaceBar::findPrevious()
{
    if (m_matches.isEmpty()) {
        return;
    }
    
    // Move to previous match with wrap-around
    m_currentMatchIndex = (m_currentMatchIndex - 1 + m_matches.count()) % m_matches.count();
    
    selectCurrentMatch();
    scrollToCurrentMatch();
    updateMatchCountLabel();
    highlightMatches();
}

// === Replace Slots ===

void FindReplaceBar::replaceCurrent()
{
    if (m_matches.isEmpty() || m_currentMatchIndex < 0 || m_currentMatchIndex >= m_matches.count()) {
        return;
    }

    m_replacing = true;
    QTextCursor cursor = m_matches[m_currentMatchIndex];
    cursor.beginEditBlock();
    cursor.removeSelectedText();
    cursor.insertText(m_replaceInput->text());
    cursor.endEditBlock();

    m_replacing = false;

    // Where the replacement left off. performSearch() resets the index to the
    // first match, so without this the caret jumps back to the top of the
    // document after every replacement and the user walks backwards.
    const int resumeFrom = cursor.position();

    // Re-search to update matches
    performSearch();

    if (!m_matches.isEmpty()) {
        m_currentMatchIndex = indexOfFirstMatchAtOrAfter(resumeFrom);
        selectCurrentMatch();
        scrollToCurrentMatch();
    }

    updateMatchCountLabel();
    highlightMatches();
}

int FindReplaceBar::indexOfFirstMatchAtOrAfter(int position) const
{
    for (int i = 0; i < m_matches.count(); ++i) {
        if (m_matches[i].selectionStart() >= position) {
            return i;
        }
    }
    // Past the last match: wrap to the beginning.
    return 0;
}

void FindReplaceBar::replaceAll()
{
    if (m_matches.isEmpty()) {
        return;
    }
    
    int replacementCount = m_matches.count();
    QString replacementText = m_replaceInput->text();
    
    // Use a single edit block for atomic undo
    m_replacing = true;
    QTextCursor cursor(m_editor->document());
    cursor.beginEditBlock();
    
    // Replace in reverse order to preserve positions
    for (int i = m_matches.count() - 1; i >= 0; --i) {
        QTextCursor matchCursor = m_matches[i];
        matchCursor.removeSelectedText();
        matchCursor.insertText(replacementText);
    }
    
    cursor.endEditBlock();
    m_replacing = false;

    // Re-search to update matches (should be empty now)
    performSearch();
    
    // Update label to show replacement count
    m_matchCountLabel->setText(tr("%1 replaced").arg(replacementCount));
}

// === Private Slots ===

void FindReplaceBar::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text)
    performSearch();
    updateMatchCountLabel();
    highlightMatches();
    emit searchTextChanged(m_searchInput->text());
}

void FindReplaceBar::onCaseSensitivityChanged(bool checked)
{
    m_caseSensitive = checked;
    performSearch();
    updateMatchCountLabel();
    highlightMatches();
}

void FindReplaceBar::updateHighlights()
{
    highlightMatches();
}

void FindReplaceBar::updateMatchCountLabel()
{
    if (m_searchInput->text().isEmpty()) {
        m_matchCountLabel->clear();
        m_prevButton->setEnabled(false);
        m_nextButton->setEnabled(false);
        m_replaceButton->setEnabled(false);
        m_replaceAllButton->setEnabled(false);
    } else if (m_matches.isEmpty()) {
        m_matchCountLabel->setText(tr("No results"));
        m_prevButton->setEnabled(false);
        m_nextButton->setEnabled(false);
        m_replaceButton->setEnabled(false);
        m_replaceAllButton->setEnabled(false);
    } else {
        m_matchCountLabel->setText(tr("%1 of %2").arg(m_currentMatchIndex + 1).arg(m_matches.count()));
        m_prevButton->setEnabled(true);
        m_nextButton->setEnabled(true);
        m_replaceButton->setEnabled(true);
        m_replaceAllButton->setEnabled(true);
    }
    
    emit matchCountChanged(m_matches.count());
}

// === Private Methods ===

void FindReplaceBar::performSearch()
{
    m_matches.clear();
    m_currentMatchIndex = -1;
    
    QString searchStr = m_searchInput->text();
    if (searchStr.isEmpty() || !m_editor) {
        return;
    }
    
    QTextDocument *doc = m_editor->document();
    QTextDocument::FindFlags flags;
    
    if (m_caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    
    // Find all matches
    QTextCursor cursor(doc);
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(searchStr, cursor, flags);
        if (!cursor.isNull()) {
            m_matches.append(cursor);
        }
    }
    
    // Set current match to first one if matches exist
    if (!m_matches.isEmpty()) {
        m_currentMatchIndex = 0;
        selectCurrentMatch();
        scrollToCurrentMatch();
    }
}

void FindReplaceBar::highlightMatches()
{
    if (!m_editor) {
        return;
    }
    
    QList<QTextEdit::ExtraSelection> extraSelections;
    
    // Highlight all matches
    for (int i = 0; i < m_matches.count(); ++i) {
        QTextEdit::ExtraSelection selection;
        
        if (i == m_currentMatchIndex) {
            // Current match - more prominent color
            selection.format.setBackground(m_currentMatchHighlightColor);
        } else {
            // Other matches - standard highlight color
            selection.format.setBackground(m_matchHighlightColor);
        }
        
        selection.cursor = m_matches[i];
        extraSelections.append(selection);
    }
    
    m_editor->setSearchSelections(extraSelections);
}

void FindReplaceBar::clearHighlights()
{
    if (m_editor) {
        m_editor->setSearchSelections(QList<QTextEdit::ExtraSelection>());
    }
    m_matches.clear();
    m_currentMatchIndex = -1;
}

void FindReplaceBar::selectCurrentMatch()
{
    if (m_currentMatchIndex >= 0 && m_currentMatchIndex < m_matches.count() && m_editor) {
        m_editor->setTextCursor(m_matches[m_currentMatchIndex]);
    }
}

void FindReplaceBar::scrollToCurrentMatch()
{
    if (m_currentMatchIndex >= 0 && m_currentMatchIndex < m_matches.count() && m_editor) {
        m_editor->ensureCursorVisible();
    }
}
