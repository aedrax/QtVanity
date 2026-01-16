#include "VariableManager.h"

#include <QFile>
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
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit saveError(tr("Cannot save file: %1").arg(file.errorString()));
        return false;
    }
    
    QJsonObject root;
    root[QStringLiteral("version")] = 1;
    
    // Save variables
    QJsonObject varsObj;
    for (auto it = m_variables.constBegin(); it != m_variables.constEnd(); ++it) {
        varsObj[it.key()] = it.value();
    }
    root[QStringLiteral("variables")] = varsObj;
    
    // Save template
    root[QStringLiteral("qssTemplate")] = qssTemplate;
    
    QJsonDocument doc(root);
    QTextStream out(&file);
    out << doc.toJson(QJsonDocument::Indented);
    
    if (file.error() != QFile::NoError) {
        emit saveError(tr("Error writing file: %1").arg(file.errorString()));
        file.close();
        return false;
    }
    
    file.close();
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
    
    // Load variables
    m_variables.clear();
    if (root.contains(QStringLiteral("variables"))) {
        QJsonObject varsObj = root[QStringLiteral("variables")].toObject();
        for (auto it = varsObj.constBegin(); it != varsObj.constEnd(); ++it) {
            m_variables[it.key()] = it.value().toString();
        }
    }
    
    // Load template
    qssTemplate = root[QStringLiteral("qssTemplate")].toString();
    
    emit projectLoaded();
    return true;
}

bool VariableManager::exportResolvedQss(const QString &filePath, const QString &qssTemplate)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit saveError(tr("Cannot save file: %1").arg(file.errorString()));
        return false;
    }
    
    QString resolved = substitute(qssTemplate);
    
    QTextStream out(&file);
    out << resolved;
    
    if (file.error() != QFile::NoError) {
        emit saveError(tr("Error writing file: %1").arg(file.errorString()));
        file.close();
        return false;
    }
    
    file.close();
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
