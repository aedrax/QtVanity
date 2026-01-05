#ifndef CONTAINERSPAGE_H
#define CONTAINERSPAGE_H

#include "GalleryPage.h"

#include <QList>

class QWidget;

/**
 * @brief Gallery page displaying container widgets.
 * 
 * ContainersPage demonstrates all standard Qt container widgets:
 * - QGroupBox (normal, checkable)
 * - QTabWidget with sample tabs
 * - QStackedWidget with navigation
 * - QScrollArea with content
 * - QFrame with various frame styles
 */
class ContainersPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a ContainersPage.
     * @param parent The parent widget.
     */
    explicit ContainersPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~ContainersPage() override;

    /**
     * @brief Enables or disables all container widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

private:
    void setupWidgets();
    void setupGroupBoxes();
    void setupTabWidget();
    void setupStackedWidget();
    void setupScrollArea();
    void setupFrames();

    QList<QWidget*> m_widgets;
};

#endif // CONTAINERSPAGE_H
