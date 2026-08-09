#ifndef QSSDIAGNOSTICS_H
#define QSSDIAGNOSTICS_H

#include <QString>
#include <QStringList>

/**
 * @brief Captures the parse errors Qt reports while a stylesheet is applied.
 *
 * Qt does not return an error when a stylesheet fails to parse. It writes
 * "Could not parse stylesheet of object ..." to the warning log and applies
 * nothing. In an editor whose whole purpose is writing stylesheets, that is
 * the single most important piece of feedback, and it was going to a terminal
 * the user may not even have.
 *
 * Construct one around the call that applies a stylesheet; it installs a
 * message handler for its lifetime, collects the matching warnings and
 * forwards everything else to the previous handler.
 *
 * Not thread-safe, and not re-entrant: one at a time, on the GUI thread.
 */
class QssDiagnostics
{
public:
    QssDiagnostics();
    ~QssDiagnostics();

    QssDiagnostics(const QssDiagnostics &) = delete;
    QssDiagnostics &operator=(const QssDiagnostics &) = delete;

    /**
     * @brief Returns the stylesheet parse errors captured so far.
     */
    QStringList messages() const;

    /**
     * @brief Returns true if @p message is a stylesheet parse complaint.
     */
    static bool isStyleSheetError(const QString &message);
};

#endif // QSSDIAGNOSTICS_H
