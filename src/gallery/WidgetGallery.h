#ifndef WIDGETGALLERY_H
#define WIDGETGALLERY_H

#include <QWidget>
#include <QHash>

class QTabWidget;
class QCheckBox;
class ButtonsPage;
class InputsPage;
class ViewsPage;
class ContainersPage;
class DialogsPage;
class DisplayPage;
class MainWindowPage;
class AdvancedPage;
class CustomWidgetsPage;
class PluginManager;

/**
 * @brief Container widget organizing all demonstration widgets into categorized tabs.
 * 
 * WidgetGallery provides a comprehensive gallery of Qt widgets organized by category:
 * - Buttons: QPushButton, QToolButton, QRadioButton, QCheckBox
 * - Inputs: QLineEdit, QTextEdit, QSpinBox, QComboBox, QSlider, etc.
 * - Views: QListView, QTreeView, QTableView
 * - Containers: QGroupBox, QTabWidget, QStackedWidget, QScrollArea, QFrame
 * - Dialogs: QMessageBox triggers, QFileDialog, QColorDialog, QProgressBar
 * - Display: QLabel, QLCDNumber, QCalendarWidget
 * - Main Window: QToolBar, QStatusBar, QMenuBar, QDockWidget, QSplitter
 * - Advanced: QMdiArea, QListView, QTreeView, QTableView, QColumnView, QGraphicsView
 * 
 * Features:
 * - Toggle controls for enabled/disabled states
 * - Toggle controls for read-only states (input widgets)
 * - Propagates state changes to all gallery pages
 */
class WidgetGallery : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a WidgetGallery.
     * @param parent The parent widget.
     */
    explicit WidgetGallery(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~WidgetGallery();

public slots:
    /**
     * @brief Enables or disables all widgets in the gallery.
     * @param enabled true to enable widgets, false to disable.
     * 
     * Propagates the enabled state to all gallery pages.
     */
    void setWidgetsEnabled(bool enabled);

    /**
     * @brief Sets read-only state for input widgets.
     * @param readOnly true to set read-only, false for editable.
     * 
     * Propagates to InputsPage for widgets that support read-only mode.
     */
    void setInputsReadOnly(bool readOnly);

    /**
     * @brief Filters the visible widget groups across every page.
     * @param text Filter text; empty restores everything.
     */
    void setFilter(const QString &text);

    /**
     * @brief Returns true if @p widget is part of the gallery's own controls.
     *
     * The gallery contains both specimens (which the state toggles act on) and
     * the controls that drive them. Callers that walk the widget tree need to
     * tell them apart; identifying controls by their label text breaks as soon
     * as a control is added or renamed.
     */
    static bool isGalleryControl(const QWidget *widget);

    /**
     * @brief Mirrors the gallery left-to-right.
     * @param rightToLeft true for RTL layout.
     *
     * Layout direction changes which side padding, margins and sub-controls
     * land on, and is easy to get wrong in a stylesheet without checking.
     */
    void setRightToLeft(bool rightToLeft);

    /**
     * @brief Scales the font of every gallery widget.
     * @param percent Percentage of the application font size, e.g. 150.
     */
    void setFontScale(int percent);

    /**
     * @brief Sets every checkable widget in the gallery checked or unchecked.
     *
     * :checked is one of the few pseudo-states that can actually be pinned;
     * :hover and :pressed cannot be held without injecting events.
     */
    void setCheckablesChecked(bool checked);

    /**
     * @brief Sets the PluginManager for custom widget support.
     * @param pluginManager Pointer to the PluginManager instance.
     * 
     * The PluginManager is used to create the CustomWidgetsPage and
     * connect plugin refresh signals. This should be called after
     * construction but before the gallery is displayed.
     */
    void setPluginManager(PluginManager *pluginManager);

signals:
    /**
     * @brief Emitted when the enabled state toggle changes.
     * @param enabled The new enabled state.
     */
    void widgetsEnabledChanged(bool enabled);

    /**
     * @brief Emitted when the read-only state toggle changes.
     * @param readOnly The new read-only state.
     */
    void inputsReadOnlyChanged(bool readOnly);

private slots:
    void onEnabledToggled(bool checked);
    void onReadOnlyToggled(bool checked);

private:
    void setupUi();
    void setupFilterRow();
    void setupPages();
    void setupToggleControls();

    QTabWidget *m_tabWidget;
    QCheckBox *m_enabledCheckBox;
    QCheckBox *m_readOnlyCheckBox;

    ButtonsPage *m_buttonsPage;
    InputsPage *m_inputsPage;
    ViewsPage *m_viewsPage;
    ContainersPage *m_containersPage;
    DialogsPage *m_dialogsPage;
    DisplayPage *m_displayPage;
    MainWindowPage *m_mainWindowPage;
    AdvancedPage *m_advancedPage;
    CustomWidgetsPage *m_customWidgetsPage;
    PluginManager *m_pluginManager;

    class QLineEdit *m_filterEdit;
    class QCheckBox *m_rightToLeftCheckBox;
    class QSpinBox *m_fontScaleSpin;
    class QCheckBox *m_checkedCheckBox;

    /// Read-only state each input had before the toggle first touched it.
    QHash<QWidget*, bool> m_originalReadOnly;
};

#endif // WIDGETGALLERY_H
