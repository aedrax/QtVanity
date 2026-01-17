#ifndef COLORSWATCHOVERLAY_H
#define COLORSWATCHOVERLAY_H

#include <QWidget>
#include <QVector>
#include <QRegularExpression>

class QTextEdit;
class QColorDialog;

/**
 * @brief Overlay widget that displays clickable color swatches for hex color codes.
 * 
 * This widget sits on top of a QTextEdit and draws small colored squares
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

    explicit ColorSwatchOverlay(QTextEdit *editor, QWidget *parent = nullptr);
    ~ColorSwatchOverlay();

    /**
     * @brief Sets the size of color swatches.
     * @param size Size in pixels (default 12).
     */
    void setSwatchSize(int size);
    int swatchSize() const { return m_swatchSize; }

    /**
     * @brief Enables or disables the overlay.
     */
    void setEnabled(bool enabled);
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
    ColorInfo* findSwatchAt(const QPoint &pos);
    QString colorToHex(const QColor &color) const;

    QTextEdit *m_editor;
    QVector<ColorInfo> m_colors;
    QRegularExpression m_colorRegex;
    int m_swatchSize;
    bool m_enabled;
    ColorInfo *m_hoveredSwatch;
    ColorInfo *m_editingSwatch;
    QColorDialog *m_colorDialog;
};

#endif // COLORSWATCHOVERLAY_H
