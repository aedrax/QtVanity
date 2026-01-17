#include "ColorSwatchOverlay.h"

#include <QTextEdit>
#include <QTextBlock>
#include <QTextCursor>
#include <QScrollBar>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QToolTip>

ColorSwatchOverlay::ColorSwatchOverlay(QTextEdit *editor, QWidget *parent)
    : QWidget(parent ? parent : editor->viewport())
    , m_editor(editor)
    , m_swatchSize(12)
    , m_enabled(true)
    , m_hoveredSwatch(nullptr)
    , m_editingSwatch(nullptr)
    , m_colorDialog(nullptr)
{
    // Match hex colors: #RGB, #RRGGBB, #AARRGGBB
    m_colorRegex = QRegularExpression(QStringLiteral("#([0-9a-fA-F]{3}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})\\b"));
    
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);
    
    // Position overlay on top of the viewport
    if (m_editor && m_editor->viewport()) {
        setGeometry(m_editor->viewport()->rect());
    }
    
    // Connect to editor signals
    connect(m_editor->document(), &QTextDocument::contentsChanged,
            this, &ColorSwatchOverlay::onTextChanged);
    connect(m_editor->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &ColorSwatchOverlay::onScrolled);
    connect(m_editor->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &ColorSwatchOverlay::onScrolled);
    
    // Initial parse
    updateColors();
}

ColorSwatchOverlay::~ColorSwatchOverlay()
{
    delete m_colorDialog;
}

void ColorSwatchOverlay::setSwatchSize(int size)
{
    if (m_swatchSize != size) {
        m_swatchSize = qMax(8, qMin(24, size));
        updateSwatchPositions();
        update();
    }
}

void ColorSwatchOverlay::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        setVisible(enabled);
        if (enabled) {
            updateColors();
        }
    }
}

void ColorSwatchOverlay::updateColors()
{
    if (!m_enabled) return;
    
    parseColors();
    updateSwatchPositions();
    update();
}

void ColorSwatchOverlay::parseColors()
{
    m_colors.clear();
    
    if (!m_editor || !m_editor->document()) return;
    
    QTextDocument *doc = m_editor->document();
    QTextBlock block = doc->begin();
    
    while (block.isValid()) {
        QString text = block.text();
        QRegularExpressionMatchIterator it = m_colorRegex.globalMatch(text);
        
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            ColorInfo info;
            info.line = block.blockNumber();
            info.startPos = block.position() + match.capturedStart();
            info.length = match.capturedLength();
            info.colorCode = match.captured();
            info.color = QColor(info.colorCode);
            
            if (info.color.isValid()) {
                m_colors.append(info);
            }
        }
        
        block = block.next();
    }
}

void ColorSwatchOverlay::updateSwatchPositions()
{
    if (!m_editor || !m_editor->viewport()) return;
    
    // Update geometry to match viewport
    setGeometry(m_editor->viewport()->rect());
    
    QFontMetrics fm(m_editor->font());
    int lineHeight = fm.height();
    int margin = 4;
    
    for (int i = 0; i < m_colors.size(); ++i) {
        ColorInfo &info = m_colors[i];
        
        // Get the cursor position at the end of the color code
        QTextCursor cursor(m_editor->document());
        cursor.setPosition(info.startPos + info.length);
        QRect cursorRect = m_editor->cursorRect(cursor);
        
        // Position swatch after the color code
        int x = cursorRect.right() + margin;
        int y = cursorRect.top() + (lineHeight - m_swatchSize) / 2;
        
        info.swatchRect = QRect(x, y, m_swatchSize, m_swatchSize);
    }
}

void ColorSwatchOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    if (!m_enabled || m_colors.isEmpty()) return;
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect visibleRect = rect();
    
    for (const ColorInfo &info : qAsConst(m_colors)) {
        // Only draw visible swatches
        if (!visibleRect.intersects(info.swatchRect)) continue;
        
        // Draw border
        painter.setPen(QPen(Qt::gray, 1));
        
        // Highlight hovered swatch
        if (m_hoveredSwatch && m_hoveredSwatch->startPos == info.startPos) {
            painter.setPen(QPen(Qt::white, 2));
        }
        
        // Fill with color
        painter.setBrush(info.color);
        painter.drawRect(info.swatchRect);
        
        // Draw a checkerboard pattern behind transparent colors
        if (info.color.alpha() < 255) {
            QRect innerRect = info.swatchRect.adjusted(1, 1, -1, -1);
            int halfW = innerRect.width() / 2;
            int halfH = innerRect.height() / 2;
            
            painter.setBrush(Qt::white);
            painter.setPen(Qt::NoPen);
            painter.drawRect(innerRect.x(), innerRect.y(), halfW, halfH);
            painter.drawRect(innerRect.x() + halfW, innerRect.y() + halfH, halfW, halfH);
            
            painter.setBrush(Qt::lightGray);
            painter.drawRect(innerRect.x() + halfW, innerRect.y(), halfW, halfH);
            painter.drawRect(innerRect.x(), innerRect.y() + halfH, halfW, halfH);
            
            // Draw color on top
            painter.setBrush(info.color);
            painter.drawRect(innerRect);
        }
    }
}

void ColorSwatchOverlay::mousePressEvent(QMouseEvent *event)
{
    if (!m_enabled) {
        event->ignore();
        return;
    }
    
    ColorInfo *swatch = findSwatchAt(event->pos());
    
    if (swatch && event->button() == Qt::LeftButton) {
        m_editingSwatch = swatch;
        
        // Create color dialog if needed
        if (!m_colorDialog) {
            m_colorDialog = new QColorDialog(this);
            m_colorDialog->setOption(QColorDialog::ShowAlphaChannel, true);
            connect(m_colorDialog, &QColorDialog::colorSelected,
                    this, &ColorSwatchOverlay::onColorSelected);
        }
        
        m_colorDialog->setCurrentColor(swatch->color);
        m_colorDialog->show();
        
        event->accept();
    } else {
        // Pass through to editor
        event->ignore();
    }
}

void ColorSwatchOverlay::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_enabled) {
        event->ignore();
        return;
    }
    
    ColorInfo *swatch = findSwatchAt(event->pos());
    
    if (swatch != m_hoveredSwatch) {
        m_hoveredSwatch = swatch;
        
        if (swatch) {
            setCursor(Qt::PointingHandCursor);
            QToolTip::showText(event->globalPos(), 
                tr("Click to change color: %1").arg(swatch->colorCode));
        } else {
            setCursor(Qt::IBeamCursor);
            QToolTip::hideText();
        }
        
        update();
    }
    
    // Pass through if not on a swatch
    if (!swatch) {
        event->ignore();
    }
}

void ColorSwatchOverlay::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    
    if (m_hoveredSwatch) {
        m_hoveredSwatch = nullptr;
        update();
    }
}

ColorSwatchOverlay::ColorInfo* ColorSwatchOverlay::findSwatchAt(const QPoint &pos)
{
    for (int i = 0; i < m_colors.size(); ++i) {
        if (m_colors[i].swatchRect.contains(pos)) {
            return &m_colors[i];
        }
    }
    return nullptr;
}

void ColorSwatchOverlay::onTextChanged()
{
    // Debounce updates - use a small delay
    QMetaObject::invokeMethod(this, "updateColors", Qt::QueuedConnection);
}

void ColorSwatchOverlay::onScrolled()
{
    updateSwatchPositions();
    update();
}

void ColorSwatchOverlay::onColorSelected(const QColor &color)
{
    if (!m_editingSwatch) return;
    
    QString oldCode = m_editingSwatch->colorCode;
    QString newCode = colorToHex(color);
    
    if (oldCode != newCode) {
        // Replace the color in the document
        QTextCursor cursor(m_editor->document());
        cursor.setPosition(m_editingSwatch->startPos);
        cursor.setPosition(m_editingSwatch->startPos + m_editingSwatch->length, 
                          QTextCursor::KeepAnchor);
        cursor.insertText(newCode);
        
        emit colorChanged(oldCode, newCode);
    }
    
    m_editingSwatch = nullptr;
}

QString ColorSwatchOverlay::colorToHex(const QColor &color) const
{
    if (color.alpha() < 255) {
        // Include alpha: #AARRGGBB
        return QString("#%1%2%3%4")
            .arg(color.alpha(), 2, 16, QChar('0'))
            .arg(color.red(), 2, 16, QChar('0'))
            .arg(color.green(), 2, 16, QChar('0'))
            .arg(color.blue(), 2, 16, QChar('0'));
    } else {
        // Standard: #RRGGBB
        return QString("#%1%2%3")
            .arg(color.red(), 2, 16, QChar('0'))
            .arg(color.green(), 2, 16, QChar('0'))
            .arg(color.blue(), 2, 16, QChar('0'));
    }
}
