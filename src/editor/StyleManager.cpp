#include "StyleManager.h"
#include "QssDiagnostics.h"
#include "VariableManager.h"

#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QStyle>

StyleManager::StyleManager(QObject *parent)
    : QObject(parent)
    , m_previewTarget(nullptr)
    // Defaults to the whole application so a StyleManager used on its own
    // behaves as before; MainWindow narrows it to the gallery.
    , m_applyToApplication(true)
{
    // Detect the platform default style at startup
    QStyle *appStyle = QApplication::style();
    if (appStyle) {
        m_defaultStyle = appStyle->objectName();
        m_currentStyle = m_defaultStyle;
    } else {
        // Fallback to Fusion as cross-platform default
        m_defaultStyle = QStringLiteral("Fusion");
        m_currentStyle = m_defaultStyle;
    }

    // Default templates path: look in standard locations
    // First try the application directory, then standard data locations
    QString appDir = QCoreApplication::applicationDirPath();
    
    // Search, in order: alongside the executable, inside a macOS bundle's
    // Resources (where CMake installs them), the parent directory (development
    // builds run from build/), then the standard data locations.
    const QStringList candidates = {
        appDir + "/styles",
        appDir + "/../Resources/styles",
        appDir + "/../styles"
    };

    for (const QString &candidate : candidates) {
        QDir dir(candidate);
        if (dir.exists()) {
            m_templatesPath = dir.absolutePath();
            break;
        }
    }

    if (m_templatesPath.isEmpty()) {
        const QStringList dataPaths = QStandardPaths::standardLocations(
            QStandardPaths::AppDataLocation);
        for (const QString &path : dataPaths) {
            QDir dir(path + "/styles");
            if (dir.exists()) {
                m_templatesPath = dir.absolutePath();
                break;
            }
        }
    }

    // If still not found, use a reasonable default
    if (m_templatesPath.isEmpty()) {
        m_templatesPath = appDir + "/styles";
    }
}

void StyleManager::applyStyleSheet(const QString &qss)
{
    m_currentStyleSheet = qss;

    QStringList parseErrors;
    {
        // Qt logs a warning and applies nothing when a stylesheet is
        // malformed; capture that for the duration of the call.
        QssDiagnostics diagnostics;
        routeStyleSheet(qss);
        parseErrors = diagnostics.messages();
    }

    emit diagnosticsChanged(parseErrors);
    emit styleApplied();
}

void StyleManager::routeStyleSheet(const QString &qss)
{
    const bool wholeApplication = m_applyToApplication || !m_previewTarget;

    if (wholeApplication) {
        if (m_previewTarget) {
            m_previewTarget->setStyleSheet(QString());
        }
        qApp->setStyleSheet(qss);
    } else {
        // Clear the application sheet first, or the previous global preview
        // would keep showing through underneath the scoped one.
        qApp->setStyleSheet(QString());
        m_previewTarget->setStyleSheet(qss);
    }
}

void StyleManager::setPreviewTarget(QWidget *target)
{
    if (m_previewTarget == target) {
        return;
    }

    if (m_previewTarget) {
        m_previewTarget->setStyleSheet(QString());
    }
    m_previewTarget = target;
    routeStyleSheet(m_currentStyleSheet);
}

QWidget* StyleManager::previewTarget() const
{
    return m_previewTarget;
}

void StyleManager::setApplyToApplication(bool enabled)
{
    if (m_applyToApplication == enabled) {
        return;
    }
    m_applyToApplication = enabled;
    routeStyleSheet(m_currentStyleSheet);
}

bool StyleManager::applyToApplication() const
{
    return m_applyToApplication;
}

QString StyleManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit loadError(tr("Cannot open file: %1").arg(file.errorString()));
        return QString();
    }
    
    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif
    QString content = in.readAll();

    if (file.error() != QFile::NoError) {
        emit loadError(tr("Error reading file: %1").arg(file.errorString()));
        return QString();
    }
    
    file.close();
    return content;
}

bool StyleManager::saveToFile(const QString &filePath, const QString &qss)
{
    // QSaveFile so a failed write leaves any existing file untouched, and an
    // explicit UTF-8 encoding because Qt 5's QTextStream defaults to the
    // locale codec.
    QSaveFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit saveError(tr("Cannot save file: %1").arg(file.errorString()));
        return false;
    }

    file.write(qss.toUtf8());

    if (!file.commit()) {
        emit saveError(tr("Error writing file: %1").arg(file.errorString()));
        return false;
    }

    return true;
}

QStringList StyleManager::availableTemplates() const
{
    QStringList templates;
    
    QDir dir(m_templatesPath);
    if (!dir.exists()) {
        return templates;
    }
    
    QStringList filters;
    filters << "*.qvp";  // Changed from "*.qss" to load project files
    
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Name);
    for (const QFileInfo &fileInfo : files) {
        templates << fileInfo.baseName();
    }
    
    return templates;
}

QString StyleManager::loadTemplate(const QString &templateName)
{
    // Templates are .qvp project files (JSON: variables + qssTemplate), matching
    // what availableTemplates() lists. Reading ".qss" here is what made every
    // template load fail after the project-file migration.
    const QString templatePath = m_templatesPath + "/" + templateName + ".qvp";

    if (!QFile::exists(templatePath)) {
        emit loadError(tr("Template not found: %1").arg(templateName));
        return QString();
    }

    VariableManager variables;
    QString qssTemplate;
    if (!variables.loadProject(templatePath, qssTemplate)) {
        emit loadError(tr("Template '%1' could not be read").arg(templateName));
        return QString();
    }

    return variables.substitute(qssTemplate);
}

QString StyleManager::currentStyleSheet() const
{
    return m_currentStyleSheet;
}

void StyleManager::clearStyleSheet()
{
    m_currentStyleSheet = QString();
    if (m_previewTarget) {
        m_previewTarget->setStyleSheet(QString());
    }
    qApp->setStyleSheet(QString());
    emit styleCleared();
}

bool StyleManager::hasCustomStyleSheet() const
{
    return !m_currentStyleSheet.isEmpty();
}

void StyleManager::setTemplatesPath(const QString &path)
{
    m_templatesPath = path;
}

QString StyleManager::templatesPath() const
{
    return m_templatesPath;
}

QStringList StyleManager::availableStyles() const
{
    return QStyleFactory::keys();
}

QString StyleManager::currentStyle() const
{
    return m_currentStyle;
}

QString StyleManager::defaultStyle() const
{
    return m_defaultStyle;
}

void StyleManager::setStyle(const QString &styleName)
{
    // Check if the style is available
    QStringList available = QStyleFactory::keys();
    bool found = false;
    QString normalizedName;
    
    // QStyleFactory::keys() returns style names, but create() is case-insensitive
    // Find the exact name from the available list for consistency
    for (const QString &name : available) {
        if (name.compare(styleName, Qt::CaseInsensitive) == 0) {
            found = true;
            normalizedName = name;
            break;
        }
    }
    
    if (!found) {
        emit styleChangeError(tr("Style '%1' is not available").arg(styleName));
        return;
    }
    
    // Create the new style
    QStyle *newStyle = QStyleFactory::create(normalizedName);
    if (!newStyle) {
        emit styleChangeError(tr("Failed to create style '%1'").arg(normalizedName));
        return;
    }
    
    // Apply the new style to the application
    QApplication::setStyle(newStyle);
    m_currentStyle = normalizedName;
    
    // Reapply the current QSS to make sure it works with the new base style
    if (!m_currentStyleSheet.isEmpty()) {
        routeStyleSheet(m_currentStyleSheet);
    }
    
    emit styleChanged(m_currentStyle);
}
