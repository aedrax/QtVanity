#ifndef COLORSWATCHOVERLAY_H
#define COLORSWATCHOVERLAY_H

#include <QWidget>
#include <QVector>
#include <QRegularExpression>

class CodeEditor;
class QColorDialog;

/**
 * @brief Overlay widget that displays clickable color swatches for hex color codes.
 * 
 * This widget sits on top of the code editor and draws small colored squares
 * at the end of lines containing hex color codes. Clicking a swatch opens
 * a color picker to change the color.
 */
class ColorSwatchOverlay : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Information about a color code in the document.
     */
    struct ColorInfo {
        int line;           ///< Line number (0-based)
        int startPos;       ///< Start position in document
        int length;         ///< Length of color code string
        QString colorCode;  ///< The color code string (e.g., "#ff0000")
        QColor color;       ///< Parsed color
        QRect swatchRect;   ///< Rectangle for the swatch (in overlay coordinates)
    };

    explicit ColorSwatchOverlay(CodeEditor *editor, QWidget *parent = nullptr);
    ~ColorSwatchOverlay();

    /**
     * @brief Sets the size of color swatches.
     * @param size Size in pixels (default 12).
     */
    void setSwatchSize(int size);
    int swatchSize() const { return m_swatchSize; }

    /**
     * @brief Shows or hides the overlay.
     *
     * Deliberately not named setEnabled(): that would shadow the non-virtual
     * QWidget::setEnabled(), so a call through a QWidget* would silently take
     * the base version and leave the overlay in an inconsistent state.
     */
    void setOverlayEnabled(bool enabled);
    bool isOverlayEnabled() const { return m_enabled; }

public slots:
    /**
     * @brief Updates the color positions based on current document content.
     */
    void updateColors();

signals:
    /**
     * @brief Emitted when a color is changed via the color picker.
     * @param oldColor The original color code.
     * @param newColor The new color code.
     */
    void colorChanged(const QString &oldColor, const QString &newColor);

protected:
    /**
     * @brief Watches the editor viewport so the overlay tracks its size.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onTextChanged();
    void onScrolled();
    void onColorSelected(const QColor &color);

private:
    void parseColors();
    void updateSwatchPositions();

    /**
     * @brief Returns the index into m_colors of the swatch under @p pos, or -1.
     */
    int swatchIndexAt(const QPoint &pos) const;

    /**
     * @brief Returns the index of the swatch starting at @p startPos, or -1.
     *
     * Used to re-resolve a swatch after the document has been reparsed.
     */
    int swatchIndexForStartPos(int startPos) const;

    QString colorToHex(const QColor &color) const;

    CodeEditor *m_editor;
    QVector<ColorInfo> m_colors;
    QRegularExpression m_colorRegex;
    int m_swatchSize;
    bool m_enabled;

    // Swatches are identified by index and by document offset, never by
    // address. parseColors() clears and refills m_colors on every keystroke,
    // so a ColorInfo* held across an edit points into freed storage - and
    // paintEvent() and onColorSelected() both dereference these.
    int m_hoveredIndex;
    int m_editingStartPos;

    QColorDialog *m_colorDialog;
};

#endif // COLORSWATCHOVERLAY_H
