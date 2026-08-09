#ifndef GALLERYPAGE_H
#define GALLERYPAGE_H

#include <QScrollArea>
#include <QWidget>
#include <QList>
#include <QPointer>

class QVBoxLayout;
class QGroupBox;

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

    /**
     * @brief Hides groups whose title and contents do not match @p text.
     * @param text Filter text; empty shows everything.
     * @return The number of groups still visible.
     *
     * Matching considers the group title and the text of the labels, buttons
     * and check boxes inside it, so searching for "slider" or for "Read-only"
     * both find something useful.
     */
    int applyFilter(const QString &text);

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

    /// Groups created through createGroup(), for filtering.
    QList<QPointer<QGroupBox>> m_groups;
};

#endif // GALLERYPAGE_H
