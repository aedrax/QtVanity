#ifndef WIDGETGALLERY_H
#define WIDGETGALLERY_H

#include <QWidget>

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
};

#endif // WIDGETGALLERY_H
