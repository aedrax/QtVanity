#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Display Qt version at runtime
    qDebug() << "QtVanity running with Qt version:" << qVersion();
    
    // Create main window
    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QString("QtVanity - Qt %1").arg(qVersion()));
    mainWindow.resize(800, 600);
    
    // Temporary central widget showing Qt version
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    QLabel *versionLabel = new QLabel(
        QString("QtVanity\nRunning with Qt %1").arg(qVersion()),
        centralWidget
    );
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);
    
    mainWindow.setCentralWidget(centralWidget);
    mainWindow.show();
    
    return app.exec();
}
