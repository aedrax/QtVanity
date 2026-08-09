#include "GalleryPage.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QAbstractButton>
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

    m_groups.append(groupBox);

    return groupBox;
}

int GalleryPage::applyFilter(const QString &text)
{
    const QString needle = text.trimmed();
    int visible = 0;

    for (const QPointer<QGroupBox> &group : m_groups) {
        if (!group) {
            continue;
        }

        bool matches = needle.isEmpty() ||
                       group->title().contains(needle, Qt::CaseInsensitive);

        // Also search the visible text inside the group, so a widget can be
        // found by its own label rather than only by its category.
        if (!matches) {
            for (const QLabel *label : group->findChildren<QLabel*>()) {
                if (label->text().contains(needle, Qt::CaseInsensitive)) {
                    matches = true;
                    break;
                }
            }
        }
        if (!matches) {
            for (const QAbstractButton *button : group->findChildren<QAbstractButton*>()) {
                if (button->text().contains(needle, Qt::CaseInsensitive)) {
                    matches = true;
                    break;
                }
            }
        }

        group->setVisible(matches);
        if (matches) {
            ++visible;
        }
    }

    return visible;
}
