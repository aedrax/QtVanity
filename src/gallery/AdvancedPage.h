#ifndef ADVANCEDPAGE_H
#define ADVANCEDPAGE_H

#include "GalleryPage.h"

#include <QList>

class QWidget;

/**
 * @brief Gallery page displaying MDI and advanced view widgets.
 * 
 * AdvancedPage demonstrates Qt widgets for complex document and data display:
 * - QMdiArea with QMdiSubWindow children
 * - QListView, QTreeView, QTableView with QStandardItemModel
 * - QColumnView for hierarchical navigation
 * - QGraphicsView with QGraphicsScene
 */
class AdvancedPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an AdvancedPage.
     * @param parent The parent widget.
     */
    explicit AdvancedPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~AdvancedPage() override;

    /**
     * @brief Enables or disables all advanced widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

private:
    void setupWidgets();
    void setupMdiArea();
    void setupModelViews();
    void setupGraphicsView();

    QList<QWidget*> m_widgets;
};

#endif // ADVANCEDPAGE_H
