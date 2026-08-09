#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QStringList>

class QCompleter;

/**
 * @brief A plain text editor with the affordances expected of a code editor.
 *
 * Provides a line number gutter, current-line highlighting, bracket matching,
 * auto-indentation, Ctrl+wheel zoom and word completion.
 *
 * Built on QPlainTextEdit rather than QTextEdit: it is designed for large
 * plain documents, and the templates edited here run to tens of thousands of
 * characters.
 *
 * Extra selections are composed here rather than set directly by callers.
 * Current-line highlighting, bracket matching and search highlighting all want
 * QPlainTextEdit::setExtraSelections(), and whichever called it last would
 * erase the others; setSearchSelections() lets a search UI contribute its
 * share without clobbering the rest.
 */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor() override;

    /**
     * @brief Paints the gutter. Called by the line number area widget.
     */
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    /**
     * @brief Width in pixels the gutter needs for the current line count.
     */
    int lineNumberAreaWidth() const;

    /**
     * @brief Sets the words offered by the completer.
     * @param words Completion candidates, e.g. selectors and property names.
     */
    void setCompletionWords(const QStringList &words);

    /**
     * @brief Contributes search-result highlighting to the extra selections.
     * @param selections The ranges to highlight.
     *
     * Merged with the editor's own current-line and bracket highlighting.
     */
    void setSearchSelections(const QList<QTextEdit::ExtraSelection> &selections);

    /**
     * @brief Returns just the search-result highlighting.
     *
     * extraSelections() returns the composed set, which also carries the
     * current-line and bracket highlighting.
     */
    QList<QTextEdit::ExtraSelection> searchSelections() const { return m_searchSelections; }

    /**
     * @brief Sets whether the current line is highlighted.
     */
    void setCurrentLineHighlightEnabled(bool enabled);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);
    void refreshExtraSelections();
    void insertCompletion(const QString &completion);

private:
    /**
     * @brief Returns the identifier fragment immediately before the cursor.
     */
    QString completionPrefix() const;

    /**
     * @brief Inserts a newline carrying the current line's indentation.
     */
    void insertIndentedNewline();

    /**
     * @brief Returns the position of the brace matching the one at @p position.
     * @return Document position of the match, or -1 if there is none.
     */
    int matchingBracketPosition(int position) const;

    QList<QTextEdit::ExtraSelection> currentLineSelections() const;
    QList<QTextEdit::ExtraSelection> bracketSelections() const;

    QWidget *m_lineNumberArea;
    QCompleter *m_completer;
    QList<QTextEdit::ExtraSelection> m_searchSelections;
    bool m_highlightCurrentLine;
};

#endif // CODEEDITOR_H
