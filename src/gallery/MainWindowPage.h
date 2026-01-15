#ifndef MAINWINDOWPAGE_H
#define MAINWINDOWPAGE_H

#include "GalleryPage.h"

#include <QList>

class QWidget;

/**
 * @brief Gallery page displaying main window components.
 * 
 * MainWindowPage demonstrates Qt widgets typically used in QMainWindow:
 * - QToolBar (with actions, separators, and tool button styles)
 * - QStatusBar (permanent widgets and temporary messages)
 * - QMenuBar (menus with actions, submenus, separators)
 * - QDockWidget (dockable panel demonstration)
 * - QSplitter (horizontal and vertical splitters)
 */
class MainWindowPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a MainWindowPage.
     * @param parent The parent widget.
     */
    explicit MainWindowPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~MainWindowPage() override;

    /**
     * @brief Enables or disables all main window component widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

private:
    void setupWidgets();
    void setupToolBar();
    void setupStatusBar();
    void setupMenuBar();
    void setupDockWidget();
    void setupSplitter();

    QList<QWidget*> m_widgets;
};

#endif // MAINWINDOWPAGE_H
