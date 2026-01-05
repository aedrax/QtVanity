#ifndef BUTTONSPAGE_H
#define BUTTONSPAGE_H

#include "GalleryPage.h"

#include <QList>

class QAbstractButton;

/**
 * @brief Gallery page displaying button widgets.
 * 
 * ButtonsPage demonstrates all standard Qt button widgets in various states:
 * - QPushButton (normal, flat, default, checkable)
 * - QToolButton (normal, with menu, checkable)
 * - QRadioButton (checked, unchecked)
 * - QCheckBox (checked, unchecked, tristate)
 */
class ButtonsPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a ButtonsPage.
     * @param parent The parent widget.
     */
    explicit ButtonsPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~ButtonsPage() override;

    /**
     * @brief Enables or disables all button widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

private:
    void setupWidgets();
    void setupPushButtons();
    void setupToolButtons();
    void setupRadioButtons();
    void setupCheckBoxes();

    QList<QAbstractButton*> m_buttons;
};

#endif // BUTTONSPAGE_H
