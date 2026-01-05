#ifndef GALLERYPAGE_H
#define GALLERYPAGE_H

#include <QScrollArea>
#include <QWidget>

class QVBoxLayout;

/**
 * @brief Base class for gallery pages in the Widget Gallery.
 * 
 * GalleryPage provides a scrollable container for widget demonstrations.
 * Each derived class implements a specific category of widgets (Buttons,
 * Inputs, Views, Containers, Dialogs).
 * 
 * Features:
 * - Scrollable content area
 * - Common layout setup
 * - Virtual method for enabling/disabling all widgets
 */
class GalleryPage : public QScrollArea
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a GalleryPage.
     * @param parent The parent widget.
     */
    explicit GalleryPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    virtual ~GalleryPage();

    /**
     * @brief Enables or disables all widgets on this page.
     * @param enabled true to enable widgets, false to disable.
     * 
     * Derived classes must implement this to toggle their specific widgets.
     */
    virtual void setWidgetsEnabled(bool enabled) = 0;

protected:
    /**
     * @brief Returns the content widget for adding child widgets.
     * @return Pointer to the content widget.
     */
    QWidget* contentWidget() const;

    /**
     * @brief Returns the main layout for adding widgets.
     * @return Pointer to the main vertical layout.
     */
    QVBoxLayout* mainLayout() const;

    /**
     * @brief Creates a group box with a title and vertical layout.
     * @param title The group box title.
     * @return Pointer to the created group box.
     * 
     * Helper method for derived classes to create consistent widget groups.
     */
    QWidget* createGroup(const QString &title);

private:
    void setupUi();

    QWidget *m_contentWidget;
    QVBoxLayout *m_mainLayout;
};

#endif // GALLERYPAGE_H
