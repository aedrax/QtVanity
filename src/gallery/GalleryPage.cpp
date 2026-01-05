#include "GalleryPage.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

GalleryPage::GalleryPage(QWidget *parent)
    : QScrollArea(parent)
    , m_contentWidget(nullptr)
    , m_mainLayout(nullptr)
{
    setupUi();
}

GalleryPage::~GalleryPage()
{
    // Qt handles child widget deletion
}

void GalleryPage::setupUi()
{
    // Configure scroll area
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameShape(QFrame::NoFrame);

    // Create content widget
    m_contentWidget = new QWidget(this);
    
    // Create main layout with margins and spacing
    m_mainLayout = new QVBoxLayout(m_contentWidget);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    m_mainLayout->setSpacing(16);
    
    // Add stretch at the end to push content to top
    m_mainLayout->addStretch();

    // Set the content widget as the scroll area's widget
    setWidget(m_contentWidget);
}

QWidget* GalleryPage::contentWidget() const
{
    return m_contentWidget;
}

QVBoxLayout* GalleryPage::mainLayout() const
{
    return m_mainLayout;
}

QWidget* GalleryPage::createGroup(const QString &title)
{
    QGroupBox *groupBox = new QGroupBox(title, m_contentWidget);
    
    QVBoxLayout *groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setContentsMargins(8, 8, 8, 8);
    groupLayout->setSpacing(8);
    
    // Insert before the stretch at the end
    int insertIndex = m_mainLayout->count() - 1;
    if (insertIndex < 0) insertIndex = 0;
    m_mainLayout->insertWidget(insertIndex, groupBox);
    
    return groupBox;
}
