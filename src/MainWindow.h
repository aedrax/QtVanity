#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDockWidget>

class QDockWidget;
class QMenu;
class QAction;
class QActionGroup;
class WidgetGallery;
class QssEditor;
class StyleManager;
class ThemeManager;
class VariableManager;
class VariablePanel;
class SettingsManager;

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

    /**
     * @brief Returns the variable manager.
     * @return Pointer to the VariableManager.
     */
    VariableManager* variableManager() const;

    /**
     * @brief Returns the variable panel.
     * @return Pointer to the VariablePanel.
     */
    VariablePanel* variablePanel() const;

    /**
     * @brief Returns the settings manager.
     * @return Pointer to the SettingsManager.
     */
    SettingsManager* settingsManager() const;

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
    void onRegenerateStyle();
    void onVariableChanged(const QString &name, const QString &value);
    void onVariableRemoved(const QString &name);
    void onVariablesCleared();
    
    // Project file actions
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onSaveProjectAs();
    void onExportQss();
    void onProjectLoaded();
    void onProjectSaved();
    void onProjectLoadError(const QString &error);
    void onProjectSaveError(const QString &error);
    
    // Recent projects
    void onOpenRecentProject(const QString &filePath);
    void onClearRecentProjects();

private:
    void setupCentralWidget();
    void setupMenuBar();
    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupHelpMenu();
    void setupTemplatesSubmenu(QMenu *parentMenu);
    void setupRecentProjectsMenu();
    void updateRecentProjectsMenu();
    void setupConnections();
    void updateWindowTitle();
    void updateThemeActions();
    bool maybeSave();
    bool maybeSaveProject();
    void setProjectModified(bool modified);
    void clearProject();

    QDockWidget *m_variablePanelDock;
    QDockWidget *m_galleryDock;
    WidgetGallery *m_gallery;
    QssEditor *m_editor;
    StyleManager *m_styleManager;
    ThemeManager *m_themeManager;
    VariableManager *m_variableManager;
    VariablePanel *m_variablePanel;
    SettingsManager *m_settingsManager;

    // Menus
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;
    QMenu *m_templatesMenu;
    QMenu *m_themeMenu;
    QMenu *m_recentProjectsMenu;

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
    
    // Project file actions
    QAction *m_newProjectAction;
    QAction *m_openProjectAction;
    QAction *m_saveProjectAction;
    QAction *m_saveProjectAsAction;
    QAction *m_exportQssAction;
    QAction *m_clearRecentAction;
    
    // Dock widget toggle actions
    QAction *m_showVariablePanelAction;
    QAction *m_showGalleryAction;

    QString m_currentFilePath;
    QString m_currentProjectPath;
    bool m_projectModified;
};

#endif // MAINWINDOW_H
