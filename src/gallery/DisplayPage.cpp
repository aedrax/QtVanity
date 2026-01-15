#include "DisplayPage.h"

#include <QLabel>
#include <QLCDNumber>
#include <QCalendarWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QStyle>

DisplayPage::DisplayPage(QWidget *parent)
    : GalleryPage(parent)
{
    setupWidgets();
}

DisplayPage::~DisplayPage()
{
    // Qt handles child widget deletion
}

void DisplayPage::setWidgetsEnabled(bool enabled)
{
    for (QWidget *widget : m_widgets) {
        widget->setEnabled(enabled);
    }
}

void DisplayPage::setupWidgets()
{
    setupLabels();
    setupLCDNumbers();
    setupCalendar();
}

void DisplayPage::setupLabels()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Labels")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Plain text labels
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(16);
    
    QLabel *plainLabel = new QLabel(tr("Plain Text Label"), group);
    row1->addWidget(plainLabel);
    m_widgets.append(plainLabel);
    
    QLabel *wordWrapLabel = new QLabel(tr("This is a label with word wrap enabled. "
                                          "It will wrap to multiple lines when needed."), group);
    wordWrapLabel->setWordWrap(true);
    wordWrapLabel->setMaximumWidth(200);
    row1->addWidget(wordWrapLabel);
    m_widgets.append(wordWrapLabel);
    
    row1->addStretch();
    groupLayout->addLayout(row1);

    // Row 2: Rich text (HTML) label
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(16);
    
    QLabel *richTextLabel = new QLabel(group);
    richTextLabel->setText(tr("<b>Bold</b>, <i>Italic</i>, <u>Underline</u>, "
                              "<span style='color: red;'>Colored</span>"));
    richTextLabel->setTextFormat(Qt::RichText);
    row2->addWidget(richTextLabel);
    m_widgets.append(richTextLabel);
    
    QLabel *linkLabel = new QLabel(tr("<a href='https://doc.qt.io'>Qt Documentation Link</a>"), group);
    linkLabel->setTextFormat(Qt::RichText);
    linkLabel->setOpenExternalLinks(true);
    row2->addWidget(linkLabel);
    m_widgets.append(linkLabel);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
    
    // Row 3: Label with pixmap (using standard icon)
    QHBoxLayout *row3 = new QHBoxLayout();
    row3->setSpacing(16);
    
    QLabel *pixmapLabel = new QLabel(group);
    QPixmap pixmap = style()->standardIcon(QStyle::SP_ComputerIcon).pixmap(48, 48);
    pixmapLabel->setPixmap(pixmap);
    pixmapLabel->setToolTip(tr("Label with Pixmap"));
    row3->addWidget(pixmapLabel);
    m_widgets.append(pixmapLabel);
    
    QLabel *scaledPixmapLabel = new QLabel(group);
    scaledPixmapLabel->setPixmap(style()->standardIcon(QStyle::SP_DesktopIcon).pixmap(32, 32));
    scaledPixmapLabel->setScaledContents(true);
    scaledPixmapLabel->setFixedSize(64, 64);
    scaledPixmapLabel->setToolTip(tr("Scaled Pixmap Label"));
    row3->addWidget(scaledPixmapLabel);
    m_widgets.append(scaledPixmapLabel);
    
    row3->addStretch();
    groupLayout->addLayout(row3);
    
    // Row 4: Label with buddy
    QHBoxLayout *row4 = new QHBoxLayout();
    row4->setSpacing(8);
    
    QLabel *buddyLabel = new QLabel(tr("&Name:"), group);
    QLineEdit *buddyEdit = new QLineEdit(group);
    buddyEdit->setPlaceholderText(tr("Enter name here"));
    buddyLabel->setBuddy(buddyEdit);
    row4->addWidget(buddyLabel);
    row4->addWidget(buddyEdit);
    m_widgets.append(buddyLabel);
    m_widgets.append(buddyEdit);
    
    row4->addStretch();
    groupLayout->addLayout(row4);
}

void DisplayPage::setupLCDNumbers()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("LCD Numbers")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Row 1: Different display modes
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(16);
    
    // Decimal mode
    QVBoxLayout *decimalLayout = new QVBoxLayout();
    QLabel *decimalLabel = new QLabel(tr("Decimal (42)"), group);
    QLCDNumber *decimalLcd = new QLCDNumber(group);
    decimalLcd->setDigitCount(4);
    decimalLcd->setMode(QLCDNumber::Dec);
    decimalLcd->display(42);
    decimalLcd->setSegmentStyle(QLCDNumber::Filled);
    decimalLayout->addWidget(decimalLabel);
    decimalLayout->addWidget(decimalLcd);
    row1->addLayout(decimalLayout);
    m_widgets.append(decimalLabel);
    m_widgets.append(decimalLcd);
    
    // Hexadecimal mode
    QVBoxLayout *hexLayout = new QVBoxLayout();
    QLabel *hexLabel = new QLabel(tr("Hexadecimal (0x2A)"), group);
    QLCDNumber *hexLcd = new QLCDNumber(group);
    hexLcd->setDigitCount(4);
    hexLcd->setMode(QLCDNumber::Hex);
    hexLcd->display(42);  // Will show "2A"
    hexLcd->setSegmentStyle(QLCDNumber::Outline);
    hexLayout->addWidget(hexLabel);
    hexLayout->addWidget(hexLcd);
    row1->addLayout(hexLayout);
    m_widgets.append(hexLabel);
    m_widgets.append(hexLcd);
    
    // Binary mode
    QVBoxLayout *binLayout = new QVBoxLayout();
    QLabel *binLabel = new QLabel(tr("Binary (101010)"), group);
    QLCDNumber *binLcd = new QLCDNumber(group);
    binLcd->setDigitCount(8);
    binLcd->setMode(QLCDNumber::Bin);
    binLcd->display(42);  // Will show "101010"
    binLcd->setSegmentStyle(QLCDNumber::Flat);
    binLayout->addWidget(binLabel);
    binLayout->addWidget(binLcd);
    row1->addLayout(binLayout);
    m_widgets.append(binLabel);
    m_widgets.append(binLcd);
    
    row1->addStretch();
    groupLayout->addLayout(row1);
    
    // Row 2: Different segment styles
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->setSpacing(16);
    
    // Filled style
    QVBoxLayout *filledLayout = new QVBoxLayout();
    QLabel *filledLabel = new QLabel(tr("Filled Style"), group);
    QLCDNumber *filledLcd = new QLCDNumber(group);
    filledLcd->setDigitCount(5);
    filledLcd->display(12345);
    filledLcd->setSegmentStyle(QLCDNumber::Filled);
    filledLayout->addWidget(filledLabel);
    filledLayout->addWidget(filledLcd);
    row2->addLayout(filledLayout);
    m_widgets.append(filledLabel);
    m_widgets.append(filledLcd);
    
    // Outline style
    QVBoxLayout *outlineLayout = new QVBoxLayout();
    QLabel *outlineLabel = new QLabel(tr("Outline Style"), group);
    QLCDNumber *outlineLcd = new QLCDNumber(group);
    outlineLcd->setDigitCount(5);
    outlineLcd->display(12345);
    outlineLcd->setSegmentStyle(QLCDNumber::Outline);
    outlineLayout->addWidget(outlineLabel);
    outlineLayout->addWidget(outlineLcd);
    row2->addLayout(outlineLayout);
    m_widgets.append(outlineLabel);
    m_widgets.append(outlineLcd);
    
    // Flat style
    QVBoxLayout *flatLayout = new QVBoxLayout();
    QLabel *flatLabel = new QLabel(tr("Flat Style"), group);
    QLCDNumber *flatLcd = new QLCDNumber(group);
    flatLcd->setDigitCount(5);
    flatLcd->display(12345);
    flatLcd->setSegmentStyle(QLCDNumber::Flat);
    flatLayout->addWidget(flatLabel);
    flatLayout->addWidget(flatLcd);
    row2->addLayout(flatLayout);
    m_widgets.append(flatLabel);
    m_widgets.append(flatLcd);
    
    row2->addStretch();
    groupLayout->addLayout(row2);
}

void DisplayPage::setupCalendar()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Calendar Widget")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(16);
    
    // Calendar with date selection enabled
    QCalendarWidget *calendar = new QCalendarWidget(group);
    calendar->setGridVisible(true);
    calendar->setSelectionMode(QCalendarWidget::SingleSelection);
    calendar->setFirstDayOfWeek(Qt::Monday);
    calendar->setNavigationBarVisible(true);
    row->addWidget(calendar);
    m_widgets.append(calendar);
    
    row->addStretch();
    groupLayout->addLayout(row);
}
