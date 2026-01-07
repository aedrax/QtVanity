#include "MainWindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QCoreApplication::setApplicationName("QtVanity");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("QtVanity Project");
    
    // Display Qt version at runtime
    qDebug() << "QtVanity running with Qt version:" << qVersion();
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
