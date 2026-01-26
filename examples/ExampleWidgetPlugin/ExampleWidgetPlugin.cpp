/**
 * @file ExampleWidgetPlugin.cpp
 * @brief Implementation of the ExampleWidgetPlugin
 *
 * This file contains the implementation of the ExampleWidgetPlugin class
 * and the GradientButton custom widget. The GradientButton demonstrates
 * how to create a visually interesting widget that can be styled with QSS.
 */

#include "ExampleWidgetPlugin.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPainter>
#include <QLinearGradient>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

/**
 * @brief A custom button widget with gradient background
 *
 * GradientButton is a visually styled button that demonstrates how custom
 * widgets can be created for QtVanity's plugin system. It features:
 * - A gradient background that can be customized via QSS
 * - Hover effects with color transitions
 * - A modern, rounded appearance
 *
 * The widget is designed to showcase how QSS styling affects custom widgets.
 */
class GradientButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor gradientStart READ gradientStart WRITE setGradientStart)
    Q_PROPERTY(QColor gradientEnd READ gradientEnd WRITE setGradientEnd)

public:
    explicit GradientButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
        , m_gradientStart(QColor(64, 158, 255))   // Blue
        , m_gradientEnd(QColor(103, 194, 58))     // Green
        , m_hovered(false)
    {
        setMinimumSize(150, 50);
        setCursor(Qt::PointingHandCursor);
        setFlat(true);
        
        // Add subtle shadow effect
        auto *shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(10);
        shadow->setColor(QColor(0, 0, 0, 80));
        shadow->setOffset(0, 2);
        setGraphicsEffect(shadow);
    }

    QColor gradientStart() const { return m_gradientStart; }
    void setGradientStart(const QColor &color) { 
        m_gradientStart = color; 
        update();
    }

    QColor gradientEnd() const { return m_gradientEnd; }
    void setGradientEnd(const QColor &color) { 
        m_gradientEnd = color; 
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Create gradient
        QLinearGradient gradient(0, 0, width(), height());
        
        QColor startColor = m_gradientStart;
        QColor endColor = m_gradientEnd;
        
        // Lighten colors on hover
        if (m_hovered) {
            startColor = startColor.lighter(115);
            endColor = endColor.lighter(115);
        }
        
        // Darken colors when pressed
        if (isDown()) {
            startColor = startColor.darker(110);
            endColor = endColor.darker(110);
        }
        
        // Desaturate when disabled
        if (!isEnabled()) {
            startColor = QColor(180, 180, 180);
            endColor = QColor(150, 150, 150);
        }
        
        gradient.setColorAt(0, startColor);
        gradient.setColorAt(1, endColor);
        
        // Draw rounded rectangle background
        QRectF rect = this->rect().adjusted(1, 1, -1, -1);
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect, 8, 8);
        
        // Draw text
        painter.setPen(isEnabled() ? Qt::white : QColor(120, 120, 120));
        QFont font = this->font();
        font.setBold(true);
        font.setPointSize(11);
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignCenter, text());
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override
#else
    void enterEvent(QEvent *event) override
#endif
    {
        m_hovered = true;
        update();
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        m_hovered = false;
        update();
        QPushButton::leaveEvent(event);
    }

private:
    QColor m_gradientStart;
    QColor m_gradientEnd;
    bool m_hovered;
};

/**
 * @brief A color swatch display widget
 *
 * ColorSwatch displays a colored rectangle with a label, demonstrating
 * how simple custom widgets can be created for the plugin system.
 */
class ColorSwatch : public QFrame
{
    Q_OBJECT

public:
    explicit ColorSwatch(const QString &name, const QColor &color, QWidget *parent = nullptr)
        : QFrame(parent)
        , m_color(color)
        , m_name(name)
    {
        setMinimumSize(80, 80);
        setMaximumSize(100, 100);
        setFrameStyle(QFrame::Box | QFrame::Plain);
        setLineWidth(1);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QFrame::paintEvent(event);
        
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw color fill
        QRect colorRect = rect().adjusted(2, 2, -2, -20);
        painter.fillRect(colorRect, m_color);
        
        // Draw color name
        painter.setPen(palette().text().color());
        QFont font = this->font();
        font.setPointSize(8);
        painter.setFont(font);
        QRect textRect = rect().adjusted(2, rect().height() - 18, -2, -2);
        painter.drawText(textRect, Qt::AlignCenter, m_name);
    }

private:
    QColor m_color;
    QString m_name;
};

// =============================================================================
// ExampleWidgetPlugin Implementation
// =============================================================================

ExampleWidgetPlugin::ExampleWidgetPlugin(QObject *parent)
    : QObject(parent)
{
}

QWidget* ExampleWidgetPlugin::createWidget(QWidget *parent)
{
    // Create a container widget with multiple example widgets
    QWidget *container = new QWidget(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Add gradient buttons section
    QLabel *buttonLabel = new QLabel(QObject::tr("Gradient Buttons:"), container);
    buttonLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    mainLayout->addWidget(buttonLabel);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    // Create gradient buttons with different color schemes
    GradientButton *primaryBtn = new GradientButton(QObject::tr("Primary"), container);
    primaryBtn->setGradientStart(QColor(64, 158, 255));
    primaryBtn->setGradientEnd(QColor(36, 112, 229));
    buttonLayout->addWidget(primaryBtn);
    
    GradientButton *successBtn = new GradientButton(QObject::tr("Success"), container);
    successBtn->setGradientStart(QColor(103, 194, 58));
    successBtn->setGradientEnd(QColor(72, 168, 36));
    buttonLayout->addWidget(successBtn);
    
    GradientButton *warningBtn = new GradientButton(QObject::tr("Warning"), container);
    warningBtn->setGradientStart(QColor(230, 162, 60));
    warningBtn->setGradientEnd(QColor(207, 130, 30));
    buttonLayout->addWidget(warningBtn);
    
    mainLayout->addLayout(buttonLayout);
    
    // Add color swatches section
    QLabel *swatchLabel = new QLabel(QObject::tr("Color Swatches:"), container);
    swatchLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    mainLayout->addWidget(swatchLabel);
    
    QHBoxLayout *swatchLayout = new QHBoxLayout();
    swatchLayout->setSpacing(10);
    
    swatchLayout->addWidget(new ColorSwatch(QObject::tr("Primary"), QColor(64, 158, 255), container));
    swatchLayout->addWidget(new ColorSwatch(QObject::tr("Success"), QColor(103, 194, 58), container));
    swatchLayout->addWidget(new ColorSwatch(QObject::tr("Warning"), QColor(230, 162, 60), container));
    swatchLayout->addWidget(new ColorSwatch(QObject::tr("Danger"), QColor(245, 108, 108), container));
    swatchLayout->addStretch();
    
    mainLayout->addLayout(swatchLayout);
    mainLayout->addStretch();
    
    return container;
}

QString ExampleWidgetPlugin::widgetName() const
{
    return QStringLiteral("Example Widget");
}

QString ExampleWidgetPlugin::widgetDescription() const
{
    return QObject::tr("A demonstration widget plugin featuring gradient buttons and color swatches. "
                       "This example shows how to create custom widgets for QtVanity's plugin system.");
}

QIcon ExampleWidgetPlugin::widgetIcon() const
{
    // Return empty icon - no custom icon provided
    return QIcon();
}

QString ExampleWidgetPlugin::widgetCategory() const
{
    return QStringLiteral("Examples");
}

// Include the moc file for the internal classes
#include "ExampleWidgetPlugin.moc"
