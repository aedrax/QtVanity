#include "DisplayPage.h"

#include <QLabel>
#include <QLCDNumber>
#include <QCalendarWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QFrame>
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
    setupTextVariants();
    setupBadges();
    setupAvatarAndSkeleton();
    setupCardsAndAlerts();
    setupSeparators();
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

void DisplayPage::setupTextVariants()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Text Variants (shadcn)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(16);
    
    // Large text
    QLabel *largeLabel = new QLabel(tr("Large Text"), group);
    largeLabel->setProperty("large", "true");
    row->addWidget(largeLabel);
    m_widgets.append(largeLabel);
    
    // Normal text (for comparison)
    QLabel *normalLabel = new QLabel(tr("Normal Text"), group);
    row->addWidget(normalLabel);
    m_widgets.append(normalLabel);
    
    // Small text
    QLabel *smallLabel = new QLabel(tr("Small Text"), group);
    smallLabel->setProperty("small", "true");
    row->addWidget(smallLabel);
    m_widgets.append(smallLabel);
    
    // Muted text
    QLabel *mutedLabel = new QLabel(tr("Muted Text"), group);
    mutedLabel->setProperty("muted", "true");
    row->addWidget(mutedLabel);
    m_widgets.append(mutedLabel);
    
    row->addStretch();
    groupLayout->addLayout(row);
}

void DisplayPage::setupBadges()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Badges (shadcn)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(12);
    
    // Default badge
    QLabel *defaultBadge = new QLabel(tr("Badge"), group);
    defaultBadge->setProperty("badge", "true");
    row->addWidget(defaultBadge);
    m_widgets.append(defaultBadge);
    
    // Secondary badge
    QLabel *secondaryBadge = new QLabel(tr("Secondary"), group);
    secondaryBadge->setProperty("badge", "secondary");
    row->addWidget(secondaryBadge);
    m_widgets.append(secondaryBadge);
    
    // Destructive badge
    QLabel *destructiveBadge = new QLabel(tr("Destructive"), group);
    destructiveBadge->setProperty("badge", "destructive");
    row->addWidget(destructiveBadge);
    m_widgets.append(destructiveBadge);
    
    // Outline badge
    QLabel *outlineBadge = new QLabel(tr("Outline"), group);
    outlineBadge->setProperty("badge", "outline");
    row->addWidget(outlineBadge);
    m_widgets.append(outlineBadge);
    
    row->addStretch();
    groupLayout->addLayout(row);
}

void DisplayPage::setupAvatarAndSkeleton()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Avatar & Skeleton (shadcn)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(24);
    
    // Avatar
    QVBoxLayout *avatarCol = new QVBoxLayout();
    QLabel *avatarTitle = new QLabel(tr("Avatar:"), group);
    QLabel *avatar = new QLabel(tr("AB"), group);
    avatar->setProperty("avatar", "true");
    avatar->setAlignment(Qt::AlignCenter);
    avatarCol->addWidget(avatarTitle);
    avatarCol->addWidget(avatar);
    row->addLayout(avatarCol);
    m_widgets.append(avatarTitle);
    m_widgets.append(avatar);
    
    // Skeleton loading placeholders
    QVBoxLayout *skeletonCol = new QVBoxLayout();
    QLabel *skeletonTitle = new QLabel(tr("Skeleton Loading:"), group);
    skeletonCol->addWidget(skeletonTitle);
    m_widgets.append(skeletonTitle);
    
    QWidget *skeletonLine1 = new QWidget(group);
    skeletonLine1->setProperty("skeleton", "true");
    skeletonLine1->setFixedSize(200, 20);
    skeletonCol->addWidget(skeletonLine1);
    m_widgets.append(skeletonLine1);
    
    QWidget *skeletonLine2 = new QWidget(group);
    skeletonLine2->setProperty("skeleton", "true");
    skeletonLine2->setFixedSize(150, 20);
    skeletonCol->addWidget(skeletonLine2);
    m_widgets.append(skeletonLine2);
    
    QWidget *skeletonLine3 = new QWidget(group);
    skeletonLine3->setProperty("skeleton", "true");
    skeletonLine3->setFixedSize(180, 20);
    skeletonCol->addWidget(skeletonLine3);
    m_widgets.append(skeletonLine3);
    
    row->addLayout(skeletonCol);
    row->addStretch();
    groupLayout->addLayout(row);
}

void DisplayPage::setupCardsAndAlerts()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Cards & Alerts (shadcn)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    QHBoxLayout *row = new QHBoxLayout();
    row->setSpacing(16);
    
    // Card
    QFrame *card = new QFrame(group);
    card->setProperty("card", "true");
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    QLabel *cardTitle = new QLabel(tr("Card Title"), card);
    cardTitle->setProperty("large", "true");
    QLabel *cardContent = new QLabel(tr("This is a card component with\nsome content inside."), card);
    cardContent->setProperty("muted", "true");
    cardLayout->addWidget(cardTitle);
    cardLayout->addWidget(cardContent);
    card->setMinimumWidth(200);
    row->addWidget(card);
    m_widgets.append(card);
    
    // Alert
    QFrame *alert = new QFrame(group);
    alert->setProperty("alert", "true");
    QVBoxLayout *alertLayout = new QVBoxLayout(alert);
    QLabel *alertTitle = new QLabel(tr("Heads up!"), alert);
    alertTitle->setProperty("large", "true");
    QLabel *alertContent = new QLabel(tr("You can add components to your app\nusing the CLI."), alert);
    alertLayout->addWidget(alertTitle);
    alertLayout->addWidget(alertContent);
    alert->setMinimumWidth(200);
    row->addWidget(alert);
    m_widgets.append(alert);
    
    // Destructive Alert
    QFrame *destructiveAlert = new QFrame(group);
    destructiveAlert->setProperty("alert", "destructive");
    QVBoxLayout *destAlertLayout = new QVBoxLayout(destructiveAlert);
    QLabel *destAlertTitle = new QLabel(tr("Error"), destructiveAlert);
    destAlertTitle->setProperty("large", "true");
    QLabel *destAlertContent = new QLabel(tr("Your session has expired.\nPlease log in again."), destructiveAlert);
    destAlertLayout->addWidget(destAlertTitle);
    destAlertLayout->addWidget(destAlertContent);
    destructiveAlert->setMinimumWidth(200);
    row->addWidget(destructiveAlert);
    m_widgets.append(destructiveAlert);
    
    row->addStretch();
    groupLayout->addLayout(row);
}

void DisplayPage::setupSeparators()
{
    QGroupBox *group = qobject_cast<QGroupBox*>(createGroup(tr("Separators (shadcn)")));
    QVBoxLayout *groupLayout = qobject_cast<QVBoxLayout*>(group->layout());
    
    // Horizontal separator
    QLabel *hSepLabel = new QLabel(tr("Horizontal Separator:"), group);
    groupLayout->addWidget(hSepLabel);
    m_widgets.append(hSepLabel);
    
    QFrame *hSeparator = new QFrame(group);
    hSeparator->setProperty("separator", "horizontal");
    groupLayout->addWidget(hSeparator);
    m_widgets.append(hSeparator);
    
    // Vertical separator demo
    QHBoxLayout *vSepRow = new QHBoxLayout();
    vSepRow->setSpacing(16);
    
    QLabel *leftLabel = new QLabel(tr("Left Content"), group);
    vSepRow->addWidget(leftLabel);
    m_widgets.append(leftLabel);
    
    QFrame *vSeparator = new QFrame(group);
    vSeparator->setProperty("separator", "vertical");
    vSeparator->setFixedHeight(40);
    vSepRow->addWidget(vSeparator);
    m_widgets.append(vSeparator);
    
    QLabel *rightLabel = new QLabel(tr("Right Content"), group);
    vSepRow->addWidget(rightLabel);
    m_widgets.append(rightLabel);
    
    vSepRow->addStretch();
    groupLayout->addLayout(vSepRow);
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
