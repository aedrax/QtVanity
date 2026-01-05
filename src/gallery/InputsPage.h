#ifndef INPUTSPAGE_H
#define INPUTSPAGE_H

#include "GalleryPage.h"

#include <QList>

class QWidget;
class QLineEdit;
class QTextEdit;

/**
 * @brief Gallery page displaying input widgets.
 * 
 * InputsPage demonstrates all standard Qt input widgets in various states:
 * - QLineEdit (normal, password, read-only)
 * - QTextEdit (normal, read-only)
 * - QSpinBox, QDoubleSpinBox
 * - QComboBox (editable, non-editable)
 * - QSlider (horizontal, vertical)
 * - QDial
 * - QDateEdit, QTimeEdit, QDateTimeEdit
 */
class InputsPage : public GalleryPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an InputsPage.
     * @param parent The parent widget.
     */
    explicit InputsPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~InputsPage() override;

    /**
     * @brief Enables or disables all input widgets.
     * @param enabled true to enable, false to disable.
     */
    void setWidgetsEnabled(bool enabled) override;

    /**
     * @brief Sets read-only state for applicable input widgets.
     * @param readOnly true to set read-only, false for editable.
     */
    void setReadOnly(bool readOnly);

private:
    void setupWidgets();
    void setupLineEdits();
    void setupTextEdits();
    void setupSpinBoxes();
    void setupComboBoxes();
    void setupSliders();
    void setupDateTimeEdits();

    QList<QWidget*> m_widgets;
    QList<QLineEdit*> m_lineEdits;
    QList<QTextEdit*> m_textEdits;
};

#endif // INPUTSPAGE_H
