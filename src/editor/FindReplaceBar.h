#ifndef FINDREPLACEBAR_H
#define FINDREPLACEBAR_H

#include <QWidget>
#include <QString>
#include <QList>
#include <QTextCursor>
#include <QColor>

class QTextEdit;
class QLineEdit;
class QPushButton;
class QToolButton;
class QLabel;

/**
 * @brief A collapsible find/replace bar widget for text editors.
 * 
 * The FindReplaceBar provides:
 * - Search input with match highlighting
 * - Navigation between matches (next/previous)
 * - Case-sensitive search toggle
 * - Replace functionality (single and all)
 * - Match count display
 * - Theme-aware highlight colors
 */
class FindReplaceBar : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a FindReplaceBar widget.
     * @param editor The QTextEdit to search within.
     * @param parent The parent widget.
     */
    explicit FindReplaceBar(QTextEdit *editor, QWidget *parent = nullptr);
    
    /**
     * @brief Destructor.
     */
    ~FindReplaceBar();

    // Mode control
    /**
     * @brief Shows the bar in find-only mode (search row only).
     */
    void showFindMode();
    
    /**
     * @brief Shows the bar in replace mode (both search and replace rows).
     */
    void showReplaceMode();
    
    /**
     * @brief Hides the bar and clears highlights.
     */
    void hide();
    
    /**
     * @brief Returns whether the bar is currently visible.
     */
    bool isVisible() const;

    // Search state
    /**
     * @brief Returns the current search text.
     */
    QString searchText() const;
    
    /**
     * @brief Sets the search text and triggers a search.
     * @param text The text to search for.
     */
    void setSearchText(const QString &text);
    
    /**
     * @brief Returns whether case-sensitive search is enabled.
     */
    bool isCaseSensitive() const;
    
    /**
     * @brief Sets the case sensitivity mode.
     * @param sensitive true for case-sensitive search.
     */
    void setCaseSensitive(bool sensitive);

    // Match information
    /**
     * @brief Returns the total number of matches found.
     */
    int matchCount() const;
    
    /**
     * @brief Returns the index of the current match (0-based).
     * @return Current match index, or -1 if no matches.
     */
    int currentMatchIndex() const;

    // Theme support
    /**
     * @brief Sets the color theme for match highlighting.
     * @param dark true for dark theme colors, false for light.
     */
    void setDarkTheme(bool dark);

signals:
    /**
     * @brief Emitted when the bar visibility changes.
     * @param visible true if the bar is now visible.
     */
    void visibilityChanged(bool visible);
    
    /**
     * @brief Emitted when the search text changes.
     * @param text The new search text.
     */
    void searchTextChanged(const QString &text);
    
    /**
     * @brief Emitted when the match count changes.
     * @param count The new match count.
     */
    void matchCountChanged(int count);

public slots:
    /**
     * @brief Navigates to the next match.
     */
    void findNext();
    
    /**
     * @brief Navigates to the previous match.
     */
    void findPrevious();
    
    /**
     * @brief Replaces the current match with the replacement text.
     */
    void replaceCurrent();
    
    /**
     * @brief Replaces all matches with the replacement text.
     */
    void replaceAll();

private slots:
    void onSearchTextChanged(const QString &text);
    void onCaseSensitivityChanged(bool checked);
    void updateHighlights();
    void updateMatchCountLabel();

private:
    void setupUi();
    void setupConnections();
    void performSearch();
    void highlightMatches();
    void clearHighlights();
    void selectCurrentMatch();
    void scrollToCurrentMatch();

    // Editor reference
    QTextEdit *m_editor;
    
    // UI widgets - Find row
    QLineEdit *m_searchInput;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QToolButton *m_caseSensitiveButton;
    QToolButton *m_closeButton;
    QLabel *m_matchCountLabel;
    
    // UI widgets - Replace row
    QWidget *m_replaceRow;
    QLineEdit *m_replaceInput;
    QPushButton *m_replaceButton;
    QPushButton *m_replaceAllButton;

    // Search state
    QList<QTextCursor> m_matches;
    int m_currentMatchIndex;
    bool m_caseSensitive;
    bool m_darkTheme;

    // Highlight colors
    QColor m_matchHighlightColor;
    QColor m_currentMatchHighlightColor;
};

#endif // FINDREPLACEBAR_H
