#include "CodeEditor.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextBlock>
#include <QWheelEvent>

namespace {

/// Gutter padding either side of the digits.
const int LineNumberMargin = 8;

/// Spaces inserted for one indent level.
const QString IndentUnit = QStringLiteral("    ");

/**
 * @brief The gutter widget.
 *
 * A bare child that forwards its paint event back to the editor, which is the
 * only thing that knows the block geometry.
 */
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor)
        : QWidget(editor)
        , m_editor(editor)
    {
    }

    QSize sizeHint() const override
    {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_editor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *m_editor;
};

} // namespace

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(new LineNumberArea(this))
    , m_completer(nullptr)
    , m_highlightCurrentLine(true)
{
    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditor::refreshExtraSelections);

    updateLineNumberAreaWidth();
    refreshExtraSelections();
}

CodeEditor::~CodeEditor() = default;

// =============================================================================
// Line number gutter
// =============================================================================

int CodeEditor::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    return 2 * LineNumberMargin + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy != 0) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth();
    }
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);

    // Take both colours from the palette so the gutter follows the theme.
    const QColor background = palette().color(QPalette::Window);
    const QColor idle = palette().color(QPalette::Disabled, QPalette::WindowText);
    const QColor active = palette().color(QPalette::WindowText);

    painter.fillRect(event->rect(), background);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    const int currentLine = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            painter.setPen(blockNumber == currentLine ? active : idle);
            painter.drawText(0, top,
                             m_lineNumberArea->width() - LineNumberMargin,
                             fontMetrics().height(),
                             Qt::AlignRight, QString::number(blockNumber + 1));
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                        lineNumberAreaWidth(), cr.height()));
}

// =============================================================================
// Extra selections
// =============================================================================

void CodeEditor::setSearchSelections(const QList<QTextEdit::ExtraSelection> &selections)
{
    m_searchSelections = selections;
    refreshExtraSelections();
}

void CodeEditor::setCurrentLineHighlightEnabled(bool enabled)
{
    if (m_highlightCurrentLine != enabled) {
        m_highlightCurrentLine = enabled;
        refreshExtraSelections();
    }
}

void CodeEditor::refreshExtraSelections()
{
    QList<QTextEdit::ExtraSelection> selections;
    selections.append(currentLineSelections());
    selections.append(bracketSelections());
    // Search highlighting last so it wins where the ranges overlap.
    selections.append(m_searchSelections);
    setExtraSelections(selections);

    // The active line number is drawn emphasised, so the gutter has to repaint
    // when the cursor moves between lines.
    m_lineNumberArea->update();
}

QList<QTextEdit::ExtraSelection> CodeEditor::currentLineSelections() const
{
    QList<QTextEdit::ExtraSelection> selections;
    if (!m_highlightCurrentLine || isReadOnly()) {
        return selections;
    }

    QTextEdit::ExtraSelection selection;
    // A tint of the window colour: visible on either theme without guessing
    // at a literal that only works on one.
    QColor lineColor = palette().color(QPalette::Highlight);
    lineColor.setAlpha(28);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    selections.append(selection);

    return selections;
}

int CodeEditor::matchingBracketPosition(int position) const
{
    static const QString openers = QStringLiteral("{([");
    static const QString closers = QStringLiteral("})]");

    const QString text = document()->toPlainText();
    if (position < 0 || position >= text.size()) {
        return -1;
    }

    const QChar ch = text.at(position);
    const int openIndex = openers.indexOf(ch);
    const int closeIndex = closers.indexOf(ch);

    const int direction = openIndex >= 0 ? 1 : (closeIndex >= 0 ? -1 : 0);
    if (direction == 0) {
        return -1;
    }

    const QChar partner = direction > 0 ? closers.at(openIndex) : openers.at(closeIndex);

    int depth = 0;
    for (int i = position; i >= 0 && i < text.size(); i += direction) {
        const QChar c = text.at(i);
        if (c == ch) {
            ++depth;
        } else if (c == partner) {
            if (--depth == 0) {
                return i;
            }
        }
    }
    return -1;
}

QList<QTextEdit::ExtraSelection> CodeEditor::bracketSelections() const
{
    QList<QTextEdit::ExtraSelection> selections;

    const QTextCursor cursor = textCursor();
    const int position = cursor.position();

    // Consider the character on either side of the caret, as editors do.
    int bracketPos = -1;
    int matchPos = matchingBracketPosition(position);
    if (matchPos >= 0) {
        bracketPos = position;
    } else if (position > 0) {
        matchPos = matchingBracketPosition(position - 1);
        if (matchPos >= 0) {
            bracketPos = position - 1;
        }
    }

    if (bracketPos < 0) {
        return selections;
    }

    QColor matchColor = palette().color(QPalette::Highlight);
    matchColor.setAlpha(90);

    for (int pos : {bracketPos, matchPos}) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(matchColor);
        selection.cursor = QTextCursor(document());
        selection.cursor.setPosition(pos);
        selection.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
        selections.append(selection);
    }

    return selections;
}

// =============================================================================
// Completion
// =============================================================================

void CodeEditor::setCompletionWords(const QStringList &words)
{
    if (!m_completer) {
        m_completer = new QCompleter(this);
        m_completer->setWidget(this);
        m_completer->setCompletionMode(QCompleter::PopupCompletion);
        m_completer->setCaseSensitivity(Qt::CaseInsensitive);
        m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
        connect(m_completer, QOverload<const QString &>::of(&QCompleter::activated),
                this, &CodeEditor::insertCompletion);
    }

    QStringList sorted = words;
    sorted.removeDuplicates();
    std::sort(sorted.begin(), sorted.end(),
              [](const QString &a, const QString &b) {
                  return a.compare(b, Qt::CaseInsensitive) < 0;
              });
    m_completer->setModel(new QStringListModel(sorted, m_completer));
}

QString CodeEditor::completionPrefix() const
{
    QTextCursor cursor = textCursor();
    const QString line = cursor.block().text().left(cursor.positionInBlock());

    // Walk back over the characters that can appear in a selector, a property
    // name or a ${variable} reference.
    int start = line.size();
    while (start > 0) {
        const QChar c = line.at(start - 1);
        if (c.isLetterOrNumber() || c == QLatin1Char('-') || c == QLatin1Char('_') ||
            c == QLatin1Char('$') || c == QLatin1Char('{')) {
            --start;
        } else {
            break;
        }
    }
    return line.mid(start);
}

void CodeEditor::insertCompletion(const QString &completion)
{
    QTextCursor cursor = textCursor();
    const int prefixLength = m_completer->completionPrefix().length();
    cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, prefixLength);
    cursor.insertText(completion);
    setTextCursor(cursor);
}

void CodeEditor::focusInEvent(QFocusEvent *event)
{
    if (m_completer) {
        m_completer->setWidget(this);
    }
    QPlainTextEdit::focusInEvent(event);
}

// =============================================================================
// Editing behaviour
// =============================================================================

void CodeEditor::insertIndentedNewline()
{
    QTextCursor cursor = textCursor();
    const QString line = cursor.block().text();

    // Carry the current line's leading whitespace onto the new line.
    int indentEnd = 0;
    while (indentEnd < line.size() && line.at(indentEnd).isSpace()) {
        ++indentEnd;
    }
    QString indent = line.left(indentEnd);

    // Opening a block indents one level further.
    const QString beforeCursor = line.left(cursor.positionInBlock()).trimmed();
    if (beforeCursor.endsWith(QLatin1Char('{'))) {
        indent += IndentUnit;
    }

    cursor.insertText(QLatin1String("\n") + indent);
    setTextCursor(cursor);
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    // While the completion popup is up it owns the navigation keys.
    if (m_completer && m_completer->popup()->isVisible()) {
        switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            event->ignore();
            return;
        default:
            break;
        }
    }

    const bool isNewline = event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter;
    if (isNewline && !(event->modifiers() & Qt::ShiftModifier)) {
        insertIndentedNewline();
        return;
    }

    // Typing the closing brace of a block pulls that line back one level.
    if (event->text() == QLatin1String("}")) {
        QTextCursor cursor = textCursor();
        const QString before = cursor.block().text().left(cursor.positionInBlock());
        if (before.trimmed().isEmpty() && before.endsWith(IndentUnit)) {
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, IndentUnit.size());
            cursor.removeSelectedText();
            setTextCursor(cursor);
        }
    }

    QPlainTextEdit::keyPressEvent(event);

    if (!m_completer) {
        return;
    }

    // Offer completions once there is enough of a word to be useful.
    const QString prefix = completionPrefix();
    const bool wordCharTyped = !event->text().isEmpty() &&
                               (event->text().at(0).isLetterOrNumber() ||
                                event->text().at(0) == QLatin1Char('-') ||
                                event->text().at(0) == QLatin1Char('$'));

    if (!wordCharTyped || prefix.length() < 2) {
        m_completer->popup()->hide();
        return;
    }

    if (prefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(prefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    if (m_completer->completionCount() == 0) {
        m_completer->popup()->hide();
        return;
    }

    QRect rect = cursorRect();
    rect.setWidth(m_completer->popup()->sizeHintForColumn(0) +
                  m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(rect);
}

void CodeEditor::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        const int delta = event->angleDelta().y();
        if (delta > 0) {
            zoomIn(1);
        } else if (delta < 0) {
            zoomOut(1);
        }
        updateLineNumberAreaWidth();
        event->accept();
        return;
    }

    QPlainTextEdit::wheelEvent(event);
}
