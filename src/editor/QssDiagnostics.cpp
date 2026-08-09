#include "QssDiagnostics.h"

#include <QtGlobal>

namespace {

/// The instance currently collecting messages, if any.
QssDiagnostics *g_active = nullptr;

/// The handler that was installed before ours.
QtMessageHandler g_previousHandler = nullptr;

/// Messages captured by the active instance.
QStringList g_captured;

void collectingHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    if (g_active && QssDiagnostics::isStyleSheetError(message)) {
        g_captured.append(message);
        return;
    }

    if (g_previousHandler) {
        g_previousHandler(type, context, message);
    }
}

} // namespace

QssDiagnostics::QssDiagnostics()
{
    g_captured.clear();
    g_active = this;
    g_previousHandler = qInstallMessageHandler(collectingHandler);
}

QssDiagnostics::~QssDiagnostics()
{
    qInstallMessageHandler(g_previousHandler);
    g_previousHandler = nullptr;
    g_active = nullptr;
}

QStringList QssDiagnostics::messages() const
{
    return g_captured;
}

bool QssDiagnostics::isStyleSheetError(const QString &message)
{
    // Qt's wording, from QStyleSheetStyle: "Could not parse stylesheet of
    // object 0x...". Match on the stable part of the phrase.
    return message.contains(QLatin1String("Could not parse stylesheet"),
                            Qt::CaseInsensitive)
        || message.contains(QLatin1String("Could not parse application stylesheet"),
                            Qt::CaseInsensitive);
}
