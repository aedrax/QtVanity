#include "StyleManager.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

StyleManager::StyleManager(QObject *parent)
    : QObject(parent)
{
    // Default templates path: look in standard locations
    // First try the application directory, then standard data locations
    QString appDir = QCoreApplication::applicationDirPath();
    
    // Check for styles directory relative to executable
    QDir stylesDir(appDir + "/styles");
    if (stylesDir.exists()) {
        m_templatesPath = stylesDir.absolutePath();
    } else {
        // Try parent directory (for development builds)
        stylesDir.setPath(appDir + "/../styles");
        if (stylesDir.exists()) {
            m_templatesPath = stylesDir.absolutePath();
        } else {
            // Try standard data location
            QStringList dataPaths = QStandardPaths::standardLocations(
                QStandardPaths::AppDataLocation);
            for (const QString &path : dataPaths) {
                QDir dir(path + "/styles");
                if (dir.exists()) {
                    m_templatesPath = dir.absolutePath();
                    break;
                }
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
    qApp->setStyleSheet(qss);
    emit styleApplied();
}

QString StyleManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit loadError(tr("Cannot open file: %1").arg(file.errorString()));
        return QString();
    }
    
    QTextStream in(&file);
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
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit saveError(tr("Cannot save file: %1").arg(file.errorString()));
        return false;
    }
    
    QTextStream out(&file);
    out << qss;
    
    if (file.error() != QFile::NoError) {
        emit saveError(tr("Error writing file: %1").arg(file.errorString()));
        file.close();
        return false;
    }
    
    file.close();
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
    filters << "*.qss";
    
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Name);
    for (const QFileInfo &fileInfo : files) {
        templates << fileInfo.baseName();
    }
    
    return templates;
}

QString StyleManager::loadTemplate(const QString &templateName)
{
    QString templatePath = m_templatesPath + "/" + templateName + ".qss";
    
    if (!QFile::exists(templatePath)) {
        emit loadError(tr("Template not found: %1").arg(templateName));
        return QString();
    }
    
    return loadFromFile(templatePath);
}

QString StyleManager::currentStyleSheet() const
{
    return m_currentStyleSheet;
}

void StyleManager::setTemplatesPath(const QString &path)
{
    m_templatesPath = path;
}

QString StyleManager::templatesPath() const
{
    return m_templatesPath;
}
