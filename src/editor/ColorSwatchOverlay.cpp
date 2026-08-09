#include "ColorSwatchOverlay.h"
#include "CodeEditor.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QScrollBar>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QColorDialog>
#include <QToolTip>

ColorSwatchOverlay::ColorSwatchOverlay(CodeEditor *editor, QWidget *parent)
    : QWidget(parent ? parent : editor->viewport())
    , m_editor(editor)
    , m_swatchSize(12)
    , m_enabled(true)
    , m_hoveredIndex(-1)
    , m_editingStartPos(-1)
    , m_colorDialog(nullptr)
{
    // Match hex colors: #RGB, #RRGGBB, #AARRGGBB
    m_colorRegex = QRegularExpression(QStringLiteral("#([0-9a-fA-F]{3}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})\\b"));
    
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);
    
    // Position overlay on top of the viewport, and follow it when it resizes.
    // Geometry was previously only refreshed on scroll or edit, so resizing
    // the window left the overlay at its old size until the user typed.
    if (m_editor && m_editor->viewport()) {
        setGeometry(m_editor->viewport()->rect());
        m_editor->viewport()->installEventFilter(this);
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

void ColorSwatchOverlay::setOverlayEnabled(bool enabled)
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
    // The vector the hover index refers to is gone; anything drawn from it
    // would read stale entries. The editing swatch survives as a document
    // offset and is re-resolved when the dialog reports a color.
    m_hoveredIndex = -1;

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

bool ColorSwatchOverlay::eventFilter(QObject *watched, QEvent *event)
{
    if (m_editor && watched == m_editor->viewport() && event->type() == QEvent::Resize) {
        setGeometry(m_editor->viewport()->rect());
        updateSwatchPositions();
        update();
    }
    return QWidget::eventFilter(watched, event);
}

void ColorSwatchOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    if (!m_enabled || m_colors.isEmpty()) return;
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect visibleRect = rect();

    for (int i = 0; i < m_colors.size(); ++i) {
        const ColorInfo &info = m_colors.at(i);

        // Only draw visible swatches
        if (!visibleRect.intersects(info.swatchRect)) continue;

        // Draw border
        painter.setPen(QPen(Qt::gray, 1));

        // Highlight hovered swatch
        if (i == m_hoveredIndex) {
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
    
    const int index = swatchIndexAt(event->pos());

    if (index >= 0 && event->button() == Qt::LeftButton) {
        // Remember the swatch by document offset. The dialog is modeless, so
        // the user can keep editing while it is open and m_colors will have
        // been rebuilt by the time a color comes back.
        m_editingStartPos = m_colors.at(index).startPos;

        // Create color dialog if needed
        if (!m_colorDialog) {
            m_colorDialog = new QColorDialog(this);
            m_colorDialog->setOption(QColorDialog::ShowAlphaChannel, true);
            connect(m_colorDialog, &QColorDialog::colorSelected,
                    this, &ColorSwatchOverlay::onColorSelected);
            // Live preview as the user drags through the picker.
            connect(m_colorDialog, &QColorDialog::currentColorChanged,
                    this, &ColorSwatchOverlay::onColorSelected);
            connect(m_colorDialog, &QColorDialog::finished,
                    this, [this]() { m_editingStartPos = -1; });
        }

        m_colorDialog->setCurrentColor(m_colors.at(index).color);
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
    
    const int index = swatchIndexAt(event->pos());

    if (index != m_hoveredIndex) {
        m_hoveredIndex = index;

        if (index >= 0) {
            setCursor(Qt::PointingHandCursor);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            const QPoint tipPos = event->globalPosition().toPoint();
#else
            const QPoint tipPos = event->globalPos();
#endif
            QToolTip::showText(tipPos,
                tr("Click to change color: %1").arg(m_colors.at(index).colorCode));
        } else {
            setCursor(Qt::IBeamCursor);
            QToolTip::hideText();
        }

        update();
    }

    // Pass through if not on a swatch
    if (index < 0) {
        event->ignore();
    }
}

void ColorSwatchOverlay::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    
    if (m_hoveredIndex >= 0) {
        m_hoveredIndex = -1;
        update();
    }
}

int ColorSwatchOverlay::swatchIndexAt(const QPoint &pos) const
{
    for (int i = 0; i < m_colors.size(); ++i) {
        if (m_colors[i].swatchRect.contains(pos)) {
            return i;
        }
    }
    return -1;
}

int ColorSwatchOverlay::swatchIndexForStartPos(int startPos) const
{
    if (startPos < 0) {
        return -1;
    }
    for (int i = 0; i < m_colors.size(); ++i) {
        if (m_colors[i].startPos == startPos) {
            return i;
        }
    }
    return -1;
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
    if (m_editingStartPos < 0 || !color.isValid()) return;

    // The document may have been edited while the modeless dialog was open, so
    // re-resolve the swatch from its offset rather than trusting a stale entry.
    const int index = swatchIndexForStartPos(m_editingStartPos);
    if (index < 0) {
        // The color code the user clicked is gone; there is nothing to rewrite.
        m_editingStartPos = -1;
        return;
    }

    const ColorInfo info = m_colors.at(index);
    const QString newCode = colorToHex(color);

    if (info.colorCode != newCode) {
        // Replace the color in the document
        QTextCursor cursor(m_editor->document());
        cursor.setPosition(info.startPos);
        cursor.setPosition(info.startPos + info.length, QTextCursor::KeepAnchor);
        cursor.insertText(newCode);

        emit colorChanged(info.colorCode, newCode);
    }
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
