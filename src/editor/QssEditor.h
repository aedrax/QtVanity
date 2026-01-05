#ifndef QSSEDITOR_H
#define QSSEDITOR_H

#include <QWidget>
#include <QString>

class QTextEdit;
class QPushButton;
class QCheckBox;
class QTimer;
class QssSyntaxHighlighter;

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
     * @brief Returns the current stylesheet content.
     * @return The QSS content in the editor.
     */
    QString styleSheet() const;

    /**
     * @brief Sets the stylesheet content in the editor.
     * @param qss The QSS content to set.
     * 
     * This resets the unsaved changes state.
     */
    void setStyleSheet(const QString &qss);

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

public slots:
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

    QTextEdit *m_textEdit;
    QssSyntaxHighlighter *m_highlighter;
    QPushButton *m_applyButton;
    QCheckBox *m_autoApplyCheckbox;
    QTimer *m_autoApplyTimer;
    
    bool m_hasUnsavedChanges;
    int m_autoApplyDelay;
    
    // For cursor position preservation
    bool m_isApplying;

    static constexpr int DEFAULT_AUTO_APPLY_DELAY_MS = 500;
};

#endif // QSSEDITOR_H
