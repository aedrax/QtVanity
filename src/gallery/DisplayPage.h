#ifndef DISPLAYPAGE_H
#define DISPLAYPAGE_H

#include "GalleryPage.h"

#include <QList>

class QWidget;

/**
 * @brief Gallery page displaying display-oriented widgets.
 * 
 * DisplayPage demonstrates Qt widgets primarily used to show information:
 * - QLabel (plain text, rich text, with pixmap, with buddy)
 * - QLCDNumber (decimal, hexadecimal, binary display modes)
 * - QCalendarWidget (date selection)
 */
class DisplayPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a DisplayPage.
     * @param parent The parent widget.
     */
    explicit DisplayPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~DisplayPage() override;

    /**
     * @brief Enables or disables all display widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

private:
    void setupWidgets();
    void setupLabels();
    void setupLCDNumbers();
    void setupCalendar();

    QList<QWidget*> m_widgets;
};

#endif // DISPLAYPAGE_H
