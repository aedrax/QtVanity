#ifndef QSSVOCABULARY_H
#define QSSVOCABULARY_H

#include <QStringList>

/**
 * @brief The vocabulary of Qt Style Sheets: selectors, properties, states.
 *
 * A single source of truth shared by the syntax highlighter and the editor's
 * completer. Previously these word lists existed only as alternations baked
 * into regular expression literals, so nothing else could use them.
 *
 * Lists are taken from the Qt Style Sheets Reference.
 */
namespace QssVocabulary {

/**
 * @brief Qt widget class names usable as selectors, e.g. "QPushButton".
 */
const QStringList &selectors();

/**
 * @brief Style sheet property names, e.g. "background-color".
 */
const QStringList &properties();

/**
 * @brief Pseudo-states without their leading colon, e.g. "hover".
 */
const QStringList &pseudoStates();

/**
 * @brief Sub-controls without their leading double colon, e.g. "indicator".
 */
const QStringList &subControls();

/**
 * @brief Keywords that appear on the value side of a declaration.
 *
 * Covers named colours, border styles, font weights and positions. Several of
 * these words are also property names ("border", "padding", "top"); the
 * highlighter resolves that overlap by ordering its rules, not by omitting
 * words from either list.
 */
const QStringList &valueKeywords();

} // namespace QssVocabulary

#endif // QSSVOCABULARY_H
