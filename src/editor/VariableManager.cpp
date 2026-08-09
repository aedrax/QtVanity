#include "VariableManager.h"

#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

VariableManager::VariableManager(QObject *parent)
    : QObject(parent)
{
}

// =============================================================================
// Variable Operations
// =============================================================================

void VariableManager::setVariable(const QString &name, const QString &value)
{
    m_variables[name] = value;
    emit variableChanged(name, value);
}

void VariableManager::removeVariable(const QString &name)
{
    if (m_variables.remove(name) > 0) {
        emit variableRemoved(name);
    }
}

QString VariableManager::variable(const QString &name) const
{
    return m_variables.value(name);
}

QStringList VariableManager::variableNames() const
{
    return m_variables.keys();
}

QMap<QString, QString> VariableManager::allVariables() const
{
    return m_variables;
}

bool VariableManager::hasVariable(const QString &name) const
{
    return m_variables.contains(name);
}

void VariableManager::clearVariables()
{
    m_variables.clear();
    emit variablesCleared();
}

// =============================================================================
// Substitution
// =============================================================================

QString VariableManager::substitute(const QString &qssTemplate) const
{
    QString result = qssTemplate;
    
    // Pattern: ${variable_name} where name starts with letter/underscore
    // and contains letters, numbers, underscores, or hyphens
    static const QRegularExpression varPattern(
        QStringLiteral("\\$\\{([a-zA-Z_][a-zA-Z0-9_-]*)\\}")
    );
    
    // Find all matches and replace from end to start to preserve positions
    QRegularExpressionMatchIterator it = varPattern.globalMatch(qssTemplate);
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext()) {
        matches.append(it.next());
    }
    
    // Process matches in reverse order
    for (int i = matches.size() - 1; i >= 0; --i) {
        const QRegularExpressionMatch &match = matches[i];
        QString varName = match.captured(1);
        
        // Only substitute if variable exists
        if (m_variables.contains(varName)) {
            result.replace(match.capturedStart(), match.capturedLength(),
                          m_variables.value(varName));
        }
    }
    
    return result;
}

QStringList VariableManager::findVariableReferences(const QString &qssTemplate) const
{
    QStringList references;
    
    static const QRegularExpression varPattern(
        QStringLiteral("\\$\\{([a-zA-Z_][a-zA-Z0-9_-]*)\\}")
    );
    
    QRegularExpressionMatchIterator it = varPattern.globalMatch(qssTemplate);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString varName = match.captured(1);
        if (!references.contains(varName)) {
            references.append(varName);
        }
    }
    
    return references;
}

QStringList VariableManager::findUndefinedReferences(const QString &qssTemplate) const
{
    QStringList undefined;
    QStringList allRefs = findVariableReferences(qssTemplate);
    
    for (const QString &ref : allRefs) {
        if (!m_variables.contains(ref)) {
            undefined.append(ref);
        }
    }
    
    return undefined;
}

// =============================================================================
// Project File I/O
// =============================================================================

bool VariableManager::saveProject(const QString &filePath, const QString &qssTemplate)
{
    // QSaveFile writes to a temporary and renames on commit, so a failure part
    // way through leaves the previous project intact rather than truncated.
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit saveError(tr("Cannot save file: %1").arg(file.errorString()));
        return false;
    }

    QJsonObject root;
    root[QStringLiteral("version")] = CurrentProjectVersion;
    
    // Save variables
    QJsonObject varsObj;
    for (auto it = m_variables.constBegin(); it != m_variables.constEnd(); ++it) {
        varsObj[it.key()] = it.value();
    }
    root[QStringLiteral("variables")] = varsObj;
    
    // Save template
    root[QStringLiteral("qssTemplate")] = qssTemplate;
    
    const QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));

    // commit() flushes and renames; only then is the error state meaningful.
    // The previous code checked file.error() while the data was still sitting
    // in a QTextStream buffer, so a failed write could report success.
    if (!file.commit()) {
        emit saveError(tr("Error writing file: %1").arg(file.errorString()));
        return false;
    }

    emit projectSaved();
    return true;
}

bool VariableManager::loadProject(const QString &filePath, QString &qssTemplate)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit loadError(tr("Cannot open file: %1").arg(file.errorString()));
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit loadError(tr("Invalid JSON: %1").arg(parseError.errorString()));
        return false;
    }
    
    if (!doc.isObject()) {
        emit loadError(tr("Invalid project file format"));
        return false;
    }
    
    QJsonObject root = doc.object();

    // Check version
    if (!root.contains(QStringLiteral("version"))) {
        emit loadError(tr("Missing required field: version"));
        return false;
    }

    const int version = root[QStringLiteral("version")].toInt(-1);
    if (version < 1 || version > CurrentProjectVersion) {
        emit loadError(tr("Unsupported project version: %1 (this build understands up to %2)")
                       .arg(root[QStringLiteral("version")].toVariant().toString())
                       .arg(CurrentProjectVersion));
        return false;
    }

    // A project without a template is almost certainly a wrong or damaged file;
    // silently loading it presented the user with a blank editor and no reason.
    if (!root.contains(QStringLiteral("qssTemplate"))) {
        emit loadError(tr("Missing required field: qssTemplate"));
        return false;
    }

    // Load variables, skipping any name the panel could not represent.
    m_variables.clear();
    QStringList rejected;
    if (root.contains(QStringLiteral("variables"))) {
        QJsonObject varsObj = root[QStringLiteral("variables")].toObject();
        for (auto it = varsObj.constBegin(); it != varsObj.constEnd(); ++it) {
            if (!isValidVariableName(it.key())) {
                rejected.append(it.key());
                continue;
            }
            m_variables[it.key()] = it.value().toString();
        }
    }

    // Load template
    qssTemplate = root[QStringLiteral("qssTemplate")].toString();

    emit projectLoaded();

    if (!rejected.isEmpty()) {
        emit loadError(tr("Ignored %n variable(s) with invalid names: %1", "", rejected.size())
                       .arg(rejected.join(QStringLiteral(", "))));
    }

    return true;
}

bool VariableManager::exportResolvedQss(const QString &filePath, const QString &qssTemplate)
{
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit saveError(tr("Cannot save file: %1").arg(file.errorString()));
        return false;
    }

    // Write UTF-8 explicitly. Under Qt 5 QTextStream defaults to the locale
    // codec, which mangles non-ASCII content in a stylesheet.
    file.write(substitute(qssTemplate).toUtf8());

    if (!file.commit()) {
        emit saveError(tr("Error writing file: %1").arg(file.errorString()));
        return false;
    }

    return true;
}

// =============================================================================
// Validation
// =============================================================================

bool VariableManager::isValidVariableName(const QString &name)
{
    if (name.isEmpty()) {
        return false;
    }
    
    // Pattern: starts with letter or underscore, followed by letters, numbers,
    // underscores, or hyphens
    static const QRegularExpression validNamePattern(
        QStringLiteral("^[a-zA-Z_][a-zA-Z0-9_-]*$")
    );
    
    return validNamePattern.match(name).hasMatch();
}

bool VariableManager::isColorValue(const QString &value)
{
    if (value.isEmpty() || !value.startsWith(QLatin1Char('#'))) {
        return false;
    }
    
    // Check for valid hex color formats: #RGB, #RRGGBB, #AARRGGBB
    static const QRegularExpression colorPattern(
        QStringLiteral("^#([0-9A-Fa-f]{3}|[0-9A-Fa-f]{6}|[0-9A-Fa-f]{8})$")
    );
    
    return colorPattern.match(value).hasMatch();
}
