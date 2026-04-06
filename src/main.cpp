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

    // Add a bunch of example variables that are Qt types that normally look bad in gdb
    auto exampleString = QString("Hello, QtVanity!");
    // add sample qmaps, qhash, qmultihash, and qvariant variables here with content in them
    QMap<QString, int> exampleMap;
    exampleMap["key1"] = 1;
    exampleMap["key2"] = 2;
    QHash<QString, int> exampleHash;
    exampleHash["key1"] = 1;
    exampleHash["key2"] = 2;
    QMultiHash<QString, int> exampleMultiHash;
    exampleMultiHash.insert("key1", 1);
    exampleMultiHash.insert("key1", 2);
    QVariant exampleVariant = QVariant::fromValue(42);
    

    auto exampleColor = QColor(Qt::red);
    auto exampleFont = QFont("Arial", 12);
    auto exampleSize = QSize(100, 50);
    auto exampleRect = QRect(10, 10, 200, 100);
    auto examplePoint = QPoint(50, 50);
    auto examplePalette = QPalette(Qt::blue);
    auto examplePixmap = QPixmap(100, 100);
    auto exampleIcon = QIcon(":/icon.png");
    auto exampleWidget = new QWidget();
    auto exampleMainWindow = new QMainWindow();
    auto exampleDockWidget = new QDockWidget();

    qDebug() << "Example variables created:" << exampleColor << exampleFont << exampleSize
             << exampleRect << examplePoint << examplePalette << examplePixmap
             << exampleIcon << exampleWidget << exampleMainWindow
             << exampleDockWidget;
    

    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
