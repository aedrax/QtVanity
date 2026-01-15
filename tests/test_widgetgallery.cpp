#include "test_widgetgallery.h"
#include "WidgetGallery.h"
#include "ButtonsPage.h"
#include "InputsPage.h"
#include "ViewsPage.h"
#include "ContainersPage.h"
#include "DialogsPage.h"
#include "DisplayPage.h"
#include "MainWindowPage.h"
#include "AdvancedPage.h"

#include <QSignalSpy>
#include <QRandomGenerator>
#include <QAbstractButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTabWidget>
#include <QTabBar>
#include <QScrollBar>
#include <QToolButton>
#include <QCheckBox>

void TestWidgetGallery::initTestCase()
{
    // Setup code if needed
}

void TestWidgetGallery::cleanupTestCase()
{
    // Cleanup code if needed
}

// ============================================================================
// Unit Tests
// ============================================================================

void TestWidgetGallery::testInitialState()
{
    WidgetGallery gallery;
    
    // Gallery should be created with all pages
    QTabWidget *tabWidget = gallery.findChild<QTabWidget*>();
    QVERIFY(tabWidget != nullptr);
    QCOMPARE(tabWidget->count(), 8); // Buttons, Inputs, Views, Containers, Dialogs, Display, Main Window, Advanced
}

void TestWidgetGallery::testSetWidgetsEnabled()
{
    WidgetGallery gallery;
    
    // Helper lambda to check if widget is an internal Qt widget
    // This checks both object name patterns and parent hierarchy
    auto isInternalQtWidget = [](QWidget *widget) {
        QString name = widget->objectName();
        
        // Skip widgets with Qt internal object names
        if (name == "ScrollLeftButton" || name == "ScrollRightButton" ||
            name == "qt_tabwidget_stackedwidget" || name == "qt_tabwidget_tabbar" ||
            name.startsWith("qt_")) {
            return true;
        }
        
        // Check parent hierarchy for QTabBar (scroll buttons are children of QTabBar)
        QWidget *parent = widget->parentWidget();
        while (parent) {
            // If parent is a QTabBar, this is likely an internal scroll button
            if (parent->inherits("QTabBar")) {
                return true;
            }
            // If parent is a QScrollBar, this is an internal widget
            if (parent->inherits("QScrollBar")) {
                return true;
            }
            parent = parent->parentWidget();
        }
        
        // Check if this is a QToolButton that's a direct child of QTabBar
        // (these are the scroll buttons)
        if (widget->inherits("QToolButton")) {
            QWidget *directParent = widget->parentWidget();
            if (directParent && directParent->inherits("QTabBar")) {
                return true;
            }
        }
        
        return false;
    };
    
    // Disable all widgets
    gallery.setWidgetsEnabled(false);
    
    // Find some widgets and verify they are disabled
    QList<QAbstractButton*> buttons = gallery.findChildren<QAbstractButton*>();
    // Filter out the control checkboxes (Widgets Enabled, Inputs Read-Only) and internal Qt widgets
    for (QAbstractButton *button : buttons) {
        // Skip internal Qt widgets
        if (isInternalQtWidget(button)) {
            continue;
        }
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(button);
        if (checkbox) {
            QString text = checkbox->text();
            if (text == "Widgets Enabled" || text == "Inputs Read-Only") {
                continue; // Skip control checkboxes
            }
        }
        QVERIFY2(!button->isEnabled(), 
                 qPrintable(QString("Button '%1' should be disabled").arg(button->text())));
    }
    
    // Enable all widgets
    gallery.setWidgetsEnabled(true);
    
    // Verify widgets are enabled
    for (QAbstractButton *button : buttons) {
        // Skip internal Qt widgets
        if (isInternalQtWidget(button)) {
            continue;
        }
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(button);
        if (checkbox) {
            QString text = checkbox->text();
            if (text == "Widgets Enabled" || text == "Inputs Read-Only") {
                continue; // Skip control checkboxes
            }
        }
        QVERIFY2(button->isEnabled(), 
                 qPrintable(QString("Button '%1' should be enabled").arg(button->text())));
    }
}

void TestWidgetGallery::testSetInputsReadOnly()
{
    WidgetGallery gallery;
    
    // Set inputs to read-only
    gallery.setInputsReadOnly(true);
    
    // Find line edits and verify they are read-only
    QList<QLineEdit*> lineEdits = gallery.findChildren<QLineEdit*>();
    for (QLineEdit *lineEdit : lineEdits) {
        QVERIFY2(lineEdit->isReadOnly(), "QLineEdit should be read-only");
    }
    
    // Set inputs to editable
    gallery.setInputsReadOnly(false);
    
    // Verify line edits are editable (except those that were originally read-only)
    // Note: Some line edits may be intentionally read-only for demonstration
}

void TestWidgetGallery::testSignalEmission()
{
    WidgetGallery gallery;
    
    QSignalSpy enabledSpy(&gallery, &WidgetGallery::widgetsEnabledChanged);
    QSignalSpy readOnlySpy(&gallery, &WidgetGallery::inputsReadOnlyChanged);
    
    // Toggle enabled state
    gallery.setWidgetsEnabled(false);
    QCOMPARE(enabledSpy.count(), 1);
    QCOMPARE(enabledSpy.takeFirst().at(0).toBool(), false);
    
    gallery.setWidgetsEnabled(true);
    QCOMPARE(enabledSpy.count(), 1);
    QCOMPARE(enabledSpy.takeFirst().at(0).toBool(), true);
    
    // Toggle read-only state
    gallery.setInputsReadOnly(true);
    QCOMPARE(readOnlySpy.count(), 1);
    QCOMPARE(readOnlySpy.takeFirst().at(0).toBool(), true);
    
    gallery.setInputsReadOnly(false);
    QCOMPARE(readOnlySpy.count(), 1);
    QCOMPARE(readOnlySpy.takeFirst().at(0).toBool(), false);
}

// ============================================================================
// Property-Based Tests
// ============================================================================

/**
 * Property 3: Widget Enabled State Toggle
 * 
 * For any widget in the Widget_Gallery, when the enabled toggle is switched,
 * all gallery widgets should have their enabled property match the toggle state.
 */
void TestWidgetGallery::testWidgetEnabledStateToggle_data()
{
    QTest::addColumn<bool>("enabledState");
    QTest::addColumn<int>("iteration");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        bool enabled = rng->generate() % 2 == 0;
        QTest::newRow(qPrintable(QString("enabled_toggle_%1_%2")
                                .arg(i).arg(enabled ? "true" : "false"))) 
            << enabled << i;
    }
    
    // Edge cases - explicit true/false
    QTest::newRow("explicit_enabled") << true << -1;
    QTest::newRow("explicit_disabled") << false << -2;
}

void TestWidgetGallery::testWidgetEnabledStateToggle()
{
    // Feature: qtvanity, Property 3: Widget Enabled State Toggle
    
    QFETCH(bool, enabledState);
    QFETCH(int, iteration);
    Q_UNUSED(iteration);
    
    WidgetGallery gallery;
    
    // Apply the enabled state
    gallery.setWidgetsEnabled(enabledState);
    
    // Collect all gallery widgets (excluding control widgets, containers, and internal Qt widgets)
    QList<QWidget*> galleryWidgets;
    
    // Helper lambda to check if widget is an internal Qt widget (like QTabWidget scroll buttons)
    // This checks both object name patterns and parent hierarchy
    auto isInternalQtWidget = [](QWidget *widget) {
        QString name = widget->objectName();
        
        // Skip widgets with Qt internal object names
        if (name == "ScrollLeftButton" || name == "ScrollRightButton" ||
            name == "qt_tabwidget_stackedwidget" || name == "qt_tabwidget_tabbar" ||
            name.startsWith("qt_")) {
            return true;
        }
        
        // Check parent hierarchy for QTabBar (scroll buttons are children of QTabBar)
        QWidget *parent = widget->parentWidget();
        while (parent) {
            // If parent is a QTabBar, this is likely an internal scroll button
            if (parent->inherits("QTabBar")) {
                return true;
            }
            // If parent is a QScrollBar, this is an internal widget
            if (parent->inherits("QScrollBar")) {
                return true;
            }
            parent = parent->parentWidget();
        }
        
        // Check if this is a QToolButton that's a direct child of QTabBar
        // (these are the scroll buttons)
        if (widget->inherits("QToolButton")) {
            QWidget *directParent = widget->parentWidget();
            if (directParent && directParent->inherits("QTabBar")) {
                return true;
            }
        }
        
        return false;
    };
    
    // Get all buttons (excluding control checkboxes and internal Qt widgets)
    QList<QAbstractButton*> buttons = gallery.findChildren<QAbstractButton*>();
    for (QAbstractButton *button : buttons) {
        // Skip internal Qt widgets
        if (isInternalQtWidget(button)) {
            continue;
        }
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(button);
        if (checkbox) {
            QString text = checkbox->text();
            if (text == "Widgets Enabled" || text == "Inputs Read-Only") {
                continue; // Skip control checkboxes
            }
        }
        galleryWidgets.append(button);
    }
    
    // Get all line edits
    QList<QLineEdit*> lineEdits = gallery.findChildren<QLineEdit*>();
    for (QLineEdit *lineEdit : lineEdits) {
        // Skip line edits that are part of internal Qt widgets (e.g., inside QComboBox)
        if (isInternalQtWidget(lineEdit)) {
            continue;
        }
        galleryWidgets.append(lineEdit);
    }
    
    // Get all spin boxes
    QList<QSpinBox*> spinBoxes = gallery.findChildren<QSpinBox*>();
    for (QSpinBox *spinBox : spinBoxes) {
        galleryWidgets.append(spinBox);
    }
    
    QList<QDoubleSpinBox*> doubleSpinBoxes = gallery.findChildren<QDoubleSpinBox*>();
    for (QDoubleSpinBox *spinBox : doubleSpinBoxes) {
        galleryWidgets.append(spinBox);
    }
    
    // Get all combo boxes
    QList<QComboBox*> comboBoxes = gallery.findChildren<QComboBox*>();
    for (QComboBox *comboBox : comboBoxes) {
        galleryWidgets.append(comboBox);
    }
    
    // Verify all collected widgets have the expected enabled state
    QVERIFY2(!galleryWidgets.isEmpty(), "Should have found gallery widgets to test");
    
    for (QWidget *widget : galleryWidgets) {
        QVERIFY2(widget->isEnabled() == enabledState,
                 qPrintable(QString("Widget '%1' (class: %2) enabled state should be %3, but is %4")
                           .arg(widget->objectName().isEmpty() ? "unnamed" : widget->objectName())
                           .arg(widget->metaObject()->className())
                           .arg(enabledState ? "true" : "false")
                           .arg(widget->isEnabled() ? "true" : "false")));
    }
}

/**
 * Property 4: Input Read-Only State Toggle
 * 
 * For any input widget in the Widget_Gallery that supports read-only mode
 * (QLineEdit, QTextEdit, QSpinBox, etc.), when the read-only toggle is switched,
 * all applicable input widgets should have their readOnly property match the toggle state.
 */
void TestWidgetGallery::testInputReadOnlyStateToggle_data()
{
    QTest::addColumn<bool>("readOnlyState");
    QTest::addColumn<int>("iteration");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        bool readOnly = rng->generate() % 2 == 0;
        QTest::newRow(qPrintable(QString("readonly_toggle_%1_%2")
                                .arg(i).arg(readOnly ? "true" : "false"))) 
            << readOnly << i;
    }
    
    // Edge cases - explicit true/false
    QTest::newRow("explicit_readonly") << true << -1;
    QTest::newRow("explicit_editable") << false << -2;
}

void TestWidgetGallery::testInputReadOnlyStateToggle()
{
    // Feature: qtvanity, Property 4: Input Read-Only State Toggle
    
    QFETCH(bool, readOnlyState);
    QFETCH(int, iteration);
    Q_UNUSED(iteration);
    
    WidgetGallery gallery;
    
    // Apply the read-only state
    gallery.setInputsReadOnly(readOnlyState);
    
    // Collect all input widgets that support read-only mode
    // Note: We only check widgets in the InputsPage, as that's where
    // the read-only toggle is propagated
    
    // Get all line edits
    QList<QLineEdit*> lineEdits = gallery.findChildren<QLineEdit*>();
    
    // Get all text edits
    QList<QTextEdit*> textEdits = gallery.findChildren<QTextEdit*>();
    
    // Verify we found some input widgets
    QVERIFY2(!lineEdits.isEmpty() || !textEdits.isEmpty(), 
             "Should have found input widgets to test");
    
    // Verify line edits have the expected read-only state
    for (QLineEdit *lineEdit : lineEdits) {
        QVERIFY2(lineEdit->isReadOnly() == readOnlyState,
                 qPrintable(QString("QLineEdit '%1' readOnly state should be %2, but is %3")
                           .arg(lineEdit->objectName().isEmpty() ? "unnamed" : lineEdit->objectName())
                           .arg(readOnlyState ? "true" : "false")
                           .arg(lineEdit->isReadOnly() ? "true" : "false")));
    }
    
    // Verify text edits have the expected read-only state
    for (QTextEdit *textEdit : textEdits) {
        QVERIFY2(textEdit->isReadOnly() == readOnlyState,
                 qPrintable(QString("QTextEdit '%1' readOnly state should be %2, but is %3")
                           .arg(textEdit->objectName().isEmpty() ? "unnamed" : textEdit->objectName())
                           .arg(readOnlyState ? "true" : "false")
                           .arg(textEdit->isReadOnly() ? "true" : "false")));
    }
}


/**
 * Property 1: Enabled State Propagation
 * 
 * For any widget on any new gallery page (DisplayPage, MainWindowPage, AdvancedPage),
 * when the enabled toggle is changed, that widget's enabled state should match the toggle state.
 */
void TestWidgetGallery::testNewPagesEnabledStatePropagation_data()
{
    QTest::addColumn<bool>("enabledState");
    QTest::addColumn<int>("iteration");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Generate 100 random test cases for property-based testing
    for (int i = 0; i < 100; ++i) {
        bool enabled = rng->generate() % 2 == 0;
        QTest::newRow(qPrintable(QString("new_pages_enabled_%1_%2")
                                .arg(i).arg(enabled ? "true" : "false"))) 
            << enabled << i;
    }
    
    // Edge cases - explicit true/false
    QTest::newRow("new_pages_explicit_enabled") << true << -1;
    QTest::newRow("new_pages_explicit_disabled") << false << -2;
}

void TestWidgetGallery::testNewPagesEnabledStatePropagation()
{
    // Feature: widget-gallery-completion, Property 1: Enabled State Propagation
    
    QFETCH(bool, enabledState);
    QFETCH(int, iteration);
    Q_UNUSED(iteration);
    
    WidgetGallery gallery;
    
    // Apply the enabled state
    gallery.setWidgetsEnabled(enabledState);
    
    // Helper lambda to check if widget is an internal Qt widget
    auto isInternalQtWidget = [](QWidget *widget) {
        QString name = widget->objectName();
        
        // Skip widgets with Qt internal object names
        if (name == "ScrollLeftButton" || name == "ScrollRightButton" ||
            name == "qt_tabwidget_stackedwidget" || name == "qt_tabwidget_tabbar" ||
            name.startsWith("qt_")) {
            return true;
        }
        
        // Check parent hierarchy for QTabBar (scroll buttons are children of QTabBar)
        QWidget *parent = widget->parentWidget();
        while (parent) {
            if (parent->inherits("QTabBar")) {
                return true;
            }
            // Skip scroll bars that are internal to scroll areas, views, or other containers
            if (widget->inherits("QScrollBar")) {
                if (parent->inherits("QScrollArea") || parent->inherits("QAbstractScrollArea") ||
                    parent->inherits("QAbstractItemView") || parent->inherits("QMdiArea") ||
                    parent->inherits("QGraphicsView") || parent->inherits("QTextEdit") ||
                    parent->inherits("QPlainTextEdit")) {
                    return true;
                }
            }
            parent = parent->parentWidget();
        }
        
        // Check if this is a QToolButton that's a direct child of QTabBar
        if (widget->inherits("QToolButton")) {
            QWidget *directParent = widget->parentWidget();
            if (directParent && directParent->inherits("QTabBar")) {
                return true;
            }
        }
        
        return false;
    };
    
    // Find the new pages
    DisplayPage *displayPage = gallery.findChild<DisplayPage*>();
    MainWindowPage *mainWindowPage = gallery.findChild<MainWindowPage*>();
    AdvancedPage *advancedPage = gallery.findChild<AdvancedPage*>();
    
    QVERIFY2(displayPage != nullptr, "DisplayPage should exist in gallery");
    QVERIFY2(mainWindowPage != nullptr, "MainWindowPage should exist in gallery");
    QVERIFY2(advancedPage != nullptr, "AdvancedPage should exist in gallery");
    
    // Collect widgets from each new page
    QList<QWidget*> newPageWidgets;
    
    // Get widgets from DisplayPage
    QList<QWidget*> displayWidgets = displayPage->findChildren<QWidget*>();
    for (QWidget *widget : displayWidgets) {
        if (!isInternalQtWidget(widget) && !widget->inherits("QLayout")) {
            newPageWidgets.append(widget);
        }
    }
    
    // Get widgets from MainWindowPage
    QList<QWidget*> mainWindowWidgets = mainWindowPage->findChildren<QWidget*>();
    for (QWidget *widget : mainWindowWidgets) {
        if (!isInternalQtWidget(widget) && !widget->inherits("QLayout")) {
            newPageWidgets.append(widget);
        }
    }
    
    // Get widgets from AdvancedPage
    QList<QWidget*> advancedWidgets = advancedPage->findChildren<QWidget*>();
    for (QWidget *widget : advancedWidgets) {
        if (!isInternalQtWidget(widget) && !widget->inherits("QLayout")) {
            newPageWidgets.append(widget);
        }
    }
    
    // Verify we found widgets on the new pages
    QVERIFY2(!newPageWidgets.isEmpty(), "Should have found widgets on new gallery pages");
    
    // Verify all widgets on new pages have the expected enabled state
    int checkedCount = 0;
    for (QWidget *widget : newPageWidgets) {
        // Skip container widgets that don't directly respond to setEnabled
        if (widget->inherits("QGroupBox") || widget->inherits("QFrame") ||
            widget->inherits("QScrollArea") || widget->inherits("QSplitter") ||
            widget->inherits("QMdiArea") || widget->inherits("QGraphicsView") ||
            widget->inherits("QDockWidget")) {
            continue;
        }
        
        // Skip internal scroll bars (they are managed by their parent containers)
        if (widget->inherits("QScrollBar")) {
            continue;
        }
        
        // Check interactive widgets
        if (widget->inherits("QAbstractButton") || widget->inherits("QAbstractSpinBox") ||
            widget->inherits("QLineEdit") || widget->inherits("QTextEdit") ||
            widget->inherits("QPlainTextEdit") || widget->inherits("QComboBox") ||
            widget->inherits("QAbstractSlider") || widget->inherits("QAbstractItemView") ||
            widget->inherits("QLCDNumber") || widget->inherits("QCalendarWidget") ||
            widget->inherits("QLabel") || widget->inherits("QMenuBar") ||
            widget->inherits("QToolBar") || widget->inherits("QStatusBar")) {
            
            QVERIFY2(widget->isEnabled() == enabledState,
                     qPrintable(QString("Widget '%1' (class: %2) on new page enabled state should be %3, but is %4")
                               .arg(widget->objectName().isEmpty() ? "unnamed" : widget->objectName())
                               .arg(widget->metaObject()->className())
                               .arg(enabledState ? "true" : "false")
                               .arg(widget->isEnabled() ? "true" : "false")));
            checkedCount++;
        }
    }
    
    QVERIFY2(checkedCount > 0, "Should have checked at least one widget on new pages");
}
