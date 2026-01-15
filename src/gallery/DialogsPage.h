#ifndef DIALOGSPAGE_H
#define DIALOGSPAGE_H

#include "GalleryPage.h"

#include <QList>

class QWidget;

/**
 * @brief Gallery page displaying dialog-related widgets.
 * 
 * DialogsPage demonstrates dialog widgets and triggers:
 * - Buttons to trigger QMessageBox variants
 * - Buttons to trigger QFileDialog, QColorDialog, QFontDialog
 * - QProgressBar (determinate, indeterminate)
 * - Button to trigger QWizard dialog
 * - Buttons to trigger QInputDialog variants (text, integer, item selection)
 */
class DialogsPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a DialogsPage.
     * @param parent The parent widget.
     */
    explicit DialogsPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~DialogsPage() override;

    /**
     * @brief Enables or disables all widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

private:
    void setupWidgets();
    void setupMessageBoxes();
    void setupDialogs();
    void setupProgressBars();
    void setupWizard();
    void setupInputDialogs();

    QList<QWidget*> m_widgets;
};

#endif // DIALOGSPAGE_H
