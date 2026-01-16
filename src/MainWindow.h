#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QSplitter;
class QMenu;
class QAction;
class QActionGroup;
class WidgetGallery;
class QssEditor;
class StyleManager;
class ThemeManager;

/**
 * @brief Main application window for QtVanity.
 * 
 * MainWindow provides:
 * - Split-pane layout with WidgetGallery and QssEditor
 * - Menu bar with File, Edit, and Help menus
 * - File operations: Load Style, Save Style
 * - Edit operations: Apply Style
 * - Template loading submenu
 * - Unsaved changes handling on close/load
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the MainWindow.
     * @param parent The parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~MainWindow();

    /**
     * @brief Returns the splitter widget.
     * @return Pointer to the QSplitter.
     */
    QSplitter* splitter() const;

    /**
     * @brief Returns the widget gallery.
     * @return Pointer to the WidgetGallery.
     */
    WidgetGallery* gallery() const;

    /**
     * @brief Returns the QSS editor.
     * @return Pointer to the QssEditor.
     */
    QssEditor* editor() const;

    /**
     * @brief Returns the style manager.
     * @return Pointer to the StyleManager.
     */
    StyleManager* styleManager() const;

    /**
     * @brief Returns the theme manager.
     * @return Pointer to the ThemeManager.
     */
    ThemeManager* themeManager() const;

protected:
    /**
     * @brief Handles close event with unsaved changes check.
     * @param event The close event.
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLoadStyle();
    void onSaveStyle();
    void onApplyStyle();
    void onToggleStyle();
    void onLoadTemplate(const QString &templateName);
    void onStyleApplied();
    void onStyleCleared();
    void onStyleModeChanged(bool customActive);
    void onLoadError(const QString &error);
    void onSaveError(const QString &error);
    void onStyleChangeError(const QString &error);
    void onUnsavedChangesChanged(bool hasChanges);
    void onAbout();
    void onThemeDark();
    void onThemeLight();
    void onThemeSystem();
    void onThemeModeChanged();

private:
    void setupCentralWidget();
    void setupMenuBar();
    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupHelpMenu();
    void setupTemplatesSubmenu(QMenu *parentMenu);
    void setupConnections();
    void updateWindowTitle();
    void updateThemeActions();
    bool maybeSave();

    QSplitter *m_splitter;
    WidgetGallery *m_gallery;
    QssEditor *m_editor;
    StyleManager *m_styleManager;
    ThemeManager *m_themeManager;

    // Menus
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;
    QMenu *m_templatesMenu;
    QMenu *m_themeMenu;

    // Actions
    QAction *m_loadAction;
    QAction *m_saveAction;
    QAction *m_exitAction;
    QAction *m_applyAction;
    QAction *m_toggleStyleAction;
    QAction *m_aboutAction;
    QAction *m_aboutQtAction;
    QAction *m_themeDarkAction;
    QAction *m_themeLightAction;
    QAction *m_themeSystemAction;
    QActionGroup *m_themeActionGroup;

    QString m_currentFilePath;
};

#endif // MAINWINDOW_H
