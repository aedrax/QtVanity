#ifndef VIEWSPAGE_H
#define VIEWSPAGE_H

#include "GalleryPage.h"

#include <QList>

class QWidget;

/**
 * @brief Gallery page displaying view widgets.
 * 
 * ViewsPage demonstrates all standard Qt view widgets with sample data:
 * - QListView/QListWidget with sample items
 * - QTreeView/QTreeWidget with sample hierarchy
 * - QTableView/QTableWidget with sample data
 */
class ViewsPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a ViewsPage.
     * @param parent The parent widget.
     */
    explicit ViewsPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~ViewsPage() override;

    /**
     * @brief Enables or disables all view widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

private:
    void setupWidgets();
    void setupListWidgets();
    void setupTreeWidgets();
    void setupTableWidgets();

    QList<QWidget*> m_widgets;
};

#endif // VIEWSPAGE_H
