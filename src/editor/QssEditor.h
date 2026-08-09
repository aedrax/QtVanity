#ifndef QSSEDITOR_H
#define QSSEDITOR_H

#include <QWidget>
#include <QString>
#include <QStringList>

class QTextEdit;
class QPushButton;
class QCheckBox;
class QComboBox;
class QTimer;
class QssSyntaxHighlighter;
class ColorSwatchOverlay;
class FindReplaceBar;

/**
 * @brief QSS code editor widget with syntax highlighting and auto-apply.
 * 
 * The QssEditor provides:
 * - Multi-line text editing area for QSS code
 * - Syntax highlighting via QssSyntaxHighlighter
 * - Apply button for manual style application
 * - Auto-apply mode with configurable delay
 * - Unsaved changes tracking
 * - Cursor position preservation after style application
 */
class QssEditor : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a QssEditor widget.
     * @param parent The parent widget.
     */
    explicit QssEditor(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor.
     */
    ~QssEditor();

    /**
     * @brief Returns the QSS text being edited.
     * @return The QSS content in the editor.
     *
     * Deliberately not named styleSheet(): that would shadow the non-virtual,
     * Q_PROPERTY-backed QWidget::styleSheet(), so this widget's notion of
     * "stylesheet" (the document being edited) and Qt's (the widget's own CSS)
     * would silently diverge depending on the static type of the caller.
     */
    QString qssText() const;

    /**
     * @brief Sets the QSS text being edited.
     * @param qss The QSS content to set.
     *
     * This resets the unsaved changes state. See qssText() for why this is not
     * called setStyleSheet().
     */
    void setQssText(const QString &qss);

    /**
     * @brief Returns whether there are unsaved changes.
     * @return true if the content has been modified since last save/load.
     */
    bool hasUnsavedChanges() const;

    /**
     * @brief Marks the content as saved (clears unsaved changes flag).
     */
    void markAsSaved();

    /**
     * @brief Returns whether auto-apply mode is enabled.
     * @return true if auto-apply is enabled.
     */
    bool isAutoApplyEnabled() const;

    /**
     * @brief Enables or disables auto-apply mode.
     * @param enabled true to enable auto-apply.
     */
    void setAutoApplyEnabled(bool enabled);

    /**
     * @brief Returns the auto-apply delay in milliseconds.
     * @return The delay in milliseconds.
     */
    int autoApplyDelay() const;

    /**
     * @brief Sets the auto-apply delay.
     * @param ms The delay in milliseconds.
     */
    void setAutoApplyDelay(int ms);

    /**
     * @brief Returns the internal QTextEdit widget.
     * @return Pointer to the text edit widget.
     * 
     * Useful for testing and advanced customization.
     */
    QTextEdit* textEdit() const;

    /**
     * @brief Returns whether custom style mode is active.
     * @return true if custom QSS is being applied, false if default style.
     */
    bool isCustomStyleActive() const;

    /**
     * @brief Sets the style mode.
     * @param customActive true for custom QSS, false for default style.
     */
    void setCustomStyleActive(bool customActive);

    /**
     * @brief Toggles between custom and default style modes.
     */
    void toggleStyleMode();

    /**
     * @brief Sets the available QStyles for the style selector.
     * @param styles List of style names from QStyleFactory::keys().
     */
    void setAvailableStyles(const QStringList &styles);

    /**
     * @brief Sets the currently selected style in the selector.
     * @param styleName The style name to select.
     */
    void setCurrentStyle(const QString &styleName);

    /**
     * @brief Returns the currently selected style name.
     * @return The selected style name (without any markers).
     */
    QString currentStyle() const;

    /**
     * @brief Marks a style as the platform default in the selector.
     * @param styleName The style name to mark as default.
     * 
     * The default style will be displayed with "(Default)" suffix.
     */
    void setDefaultStyleMarker(const QString &styleName);

    /**
     * @brief Sets the syntax highlighter color scheme.
     * @param dark true for dark background colors, false for light.
     */
    void setDarkColorScheme(bool dark);

    /**
     * @brief Enables or disables color swatch overlay.
     * @param enabled true to show clickable color swatches.
     */
    void setColorSwatchesEnabled(bool enabled);

    /**
     * @brief Returns whether color swatches are enabled.
     */
    bool colorSwatchesEnabled() const;

    /**
     * @brief Shows the find bar for text search.
     */
    void showFindBar();

    /**
     * @brief Shows the replace bar for find and replace.
     */
    void showReplaceBar();

    /**
     * @brief Hides the find/replace bar.
     */
    void hideFindReplaceBar();

    /**
     * @brief Returns the FindReplaceBar widget.
     * @return Pointer to the find/replace bar.
     */
    FindReplaceBar* findReplaceBar() const;

    /**
     * @brief Refreshes the color swatch overlay.
     * 
     * Call this after applying a global stylesheet to ensure
     * the color swatches remain visible and properly positioned.
     */
    void refreshColorSwatches();

signals:
    /**
     * @brief Emitted when the user requests style application.
     * @param qss The QSS content to apply.
     */
    void applyRequested(const QString &qss);

    /**
     * @brief Emitted when the editor content changes.
     */
    void contentsChanged();

    /**
     * @brief Emitted when the unsaved changes state changes.
     * @param hasChanges true if there are unsaved changes.
     */
    void unsavedChangesChanged(bool hasChanges);

    /**
     * @brief Emitted when the style mode changes.
     * @param customActive true if custom mode is now active.
     */
    void styleModeChanged(bool customActive);

    /**
     * @brief Emitted when default style should be applied.
     */
    void defaultStyleRequested();

    /**
     * @brief Emitted when the user requests a QStyle change.
     * @param styleName The requested style name.
     */
    void styleChangeRequested(const QString &styleName);

public slots:
    /**
     * @brief Cancels any pending auto-apply.
     *
     * Useful for teardown: a timer left running would apply a stylesheet after
     * the caller believed it had finished.
     */
    void cancelPendingApply();

    /**
     * @brief Triggers style application.
     * 
     * Emits applyRequested with the current content.
     */
    void apply();

private slots:
    void onTextChanged();
    void onAutoApplyTimeout();
    void onApplyClicked();
    void onAutoApplyToggled(bool checked);

private:
    void setupUi();
    void setupConnections();
    void setupFindReplaceShortcuts();

    QTextEdit *m_textEdit;
    QssSyntaxHighlighter *m_highlighter;
    ColorSwatchOverlay *m_colorSwatchOverlay;
    FindReplaceBar *m_findReplaceBar;
    QPushButton *m_applyButton;
    QPushButton *m_toggleButton;
    QCheckBox *m_autoApplyCheckbox;
    QComboBox *m_styleCombo;
    QTimer *m_autoApplyTimer;
    
    bool m_hasUnsavedChanges;
    bool m_customStyleActive;
    int m_autoApplyDelay;
    QString m_defaultStyleName;
    
    // For cursor position preservation
    bool m_isApplying;

    static constexpr int DEFAULT_AUTO_APPLY_DELAY_MS = 500;
};

#endif // QSSEDITOR_H
