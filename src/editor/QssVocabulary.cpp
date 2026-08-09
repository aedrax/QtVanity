#include "QssVocabulary.h"

namespace QssVocabulary {

const QStringList &selectors()
{
    static const QStringList list = {
        QStringLiteral("QAbstractButton"), QStringLiteral("QAbstractItemView"),
        QStringLiteral("QAbstractScrollArea"), QStringLiteral("QAbstractSpinBox"),
        QStringLiteral("QCalendarWidget"), QStringLiteral("QCheckBox"),
        QStringLiteral("QColumnView"), QStringLiteral("QComboBox"),
        QStringLiteral("QDateEdit"), QStringLiteral("QDateTimeEdit"),
        QStringLiteral("QDialog"), QStringLiteral("QDialogButtonBox"),
        QStringLiteral("QDockWidget"), QStringLiteral("QDoubleSpinBox"),
        QStringLiteral("QFontComboBox"), QStringLiteral("QFrame"),
        QStringLiteral("QGroupBox"), QStringLiteral("QHeaderView"),
        QStringLiteral("QLabel"), QStringLiteral("QLineEdit"),
        QStringLiteral("QListView"), QStringLiteral("QListWidget"),
        QStringLiteral("QMainWindow"), QStringLiteral("QMdiArea"),
        QStringLiteral("QMdiSubWindow"), QStringLiteral("QMenu"),
        QStringLiteral("QMenuBar"), QStringLiteral("QMessageBox"),
        QStringLiteral("QPlainTextEdit"), QStringLiteral("QProgressBar"),
        QStringLiteral("QPushButton"), QStringLiteral("QRadioButton"),
        QStringLiteral("QScrollArea"), QStringLiteral("QScrollBar"),
        QStringLiteral("QSizeGrip"), QStringLiteral("QSlider"),
        QStringLiteral("QSpinBox"), QStringLiteral("QSplitter"),
        QStringLiteral("QStatusBar"), QStringLiteral("QTabBar"),
        QStringLiteral("QTabWidget"), QStringLiteral("QTableView"),
        QStringLiteral("QTableWidget"), QStringLiteral("QTextEdit"),
        QStringLiteral("QTimeEdit"), QStringLiteral("QToolBar"),
        QStringLiteral("QToolBox"), QStringLiteral("QToolButton"),
        QStringLiteral("QToolTip"), QStringLiteral("QTreeView"),
        QStringLiteral("QTreeWidget"), QStringLiteral("QWidget")
    };
    return list;
}

const QStringList &properties()
{
    static const QStringList list = {
        QStringLiteral("accent-color"), QStringLiteral("alternate-background-color"),
        QStringLiteral("background"), QStringLiteral("background-attachment"),
        QStringLiteral("background-clip"), QStringLiteral("background-color"),
        QStringLiteral("background-image"), QStringLiteral("background-origin"),
        QStringLiteral("background-position"), QStringLiteral("background-repeat"),
        QStringLiteral("border"), QStringLiteral("border-bottom"),
        QStringLiteral("border-bottom-color"), QStringLiteral("border-bottom-left-radius"),
        QStringLiteral("border-bottom-right-radius"), QStringLiteral("border-bottom-style"),
        QStringLiteral("border-bottom-width"), QStringLiteral("border-color"),
        QStringLiteral("border-image"), QStringLiteral("border-left"),
        QStringLiteral("border-left-color"), QStringLiteral("border-left-style"),
        QStringLiteral("border-left-width"), QStringLiteral("border-radius"),
        QStringLiteral("border-right"), QStringLiteral("border-right-color"),
        QStringLiteral("border-right-style"), QStringLiteral("border-right-width"),
        QStringLiteral("border-style"), QStringLiteral("border-top"),
        QStringLiteral("border-top-color"), QStringLiteral("border-top-left-radius"),
        QStringLiteral("border-top-right-radius"), QStringLiteral("border-top-style"),
        QStringLiteral("border-top-width"), QStringLiteral("border-width"),
        QStringLiteral("bottom"), QStringLiteral("button-layout"),
        QStringLiteral("color"), QStringLiteral("dialogbuttonbox-buttons-have-icons"),
        QStringLiteral("font"), QStringLiteral("font-family"),
        QStringLiteral("font-size"), QStringLiteral("font-style"),
        QStringLiteral("font-weight"), QStringLiteral("gridline-color"),
        QStringLiteral("height"), QStringLiteral("icon"),
        QStringLiteral("icon-size"), QStringLiteral("image"),
        QStringLiteral("image-position"), QStringLiteral("left"),
        QStringLiteral("letter-spacing"), QStringLiteral("lineedit-password-character"),
        QStringLiteral("lineedit-password-mask-delay"), QStringLiteral("margin"),
        QStringLiteral("margin-bottom"), QStringLiteral("margin-left"),
        QStringLiteral("margin-right"), QStringLiteral("margin-top"),
        QStringLiteral("max-height"), QStringLiteral("max-width"),
        QStringLiteral("messagebox-text-interaction-flags"), QStringLiteral("min-height"),
        QStringLiteral("min-width"), QStringLiteral("opacity"),
        QStringLiteral("outline"), QStringLiteral("outline-bottom-left-radius"),
        QStringLiteral("outline-bottom-right-radius"), QStringLiteral("outline-color"),
        QStringLiteral("outline-offset"), QStringLiteral("outline-radius"),
        QStringLiteral("outline-style"), QStringLiteral("outline-top-left-radius"),
        QStringLiteral("outline-top-right-radius"), QStringLiteral("padding"),
        QStringLiteral("padding-bottom"), QStringLiteral("padding-left"),
        QStringLiteral("padding-right"), QStringLiteral("padding-top"),
        QStringLiteral("paint-alternating-row-colors-for-empty-area"),
        QStringLiteral("placeholder-text-color"), QStringLiteral("position"),
        QStringLiteral("right"), QStringLiteral("selection-background-color"),
        QStringLiteral("selection-color"), QStringLiteral("show-decoration-selected"),
        QStringLiteral("spacing"), QStringLiteral("subcontrol-origin"),
        QStringLiteral("subcontrol-position"), QStringLiteral("text-align"),
        QStringLiteral("text-decoration"), QStringLiteral("titlebar-show-tooltips-on-buttons"),
        QStringLiteral("top"), QStringLiteral("widget-animation-duration"),
        QStringLiteral("width"), QStringLiteral("word-spacing"),
        QStringLiteral("-qt-background-role"), QStringLiteral("-qt-style-features")
    };
    return list;
}

const QStringList &pseudoStates()
{
    static const QStringList list = {
        QStringLiteral("active"), QStringLiteral("adjoins-item"),
        QStringLiteral("alternate"), QStringLiteral("bottom"),
        QStringLiteral("checked"), QStringLiteral("closable"),
        QStringLiteral("closed"), QStringLiteral("default"),
        QStringLiteral("disabled"), QStringLiteral("edit-focus"),
        QStringLiteral("editable"), QStringLiteral("enabled"),
        QStringLiteral("exclusive"), QStringLiteral("first"),
        QStringLiteral("flat"), QStringLiteral("floatable"),
        QStringLiteral("focus"), QStringLiteral("has-children"),
        QStringLiteral("has-siblings"), QStringLiteral("horizontal"),
        QStringLiteral("hover"), QStringLiteral("indeterminate"),
        QStringLiteral("last"), QStringLiteral("left"),
        QStringLiteral("maximized"), QStringLiteral("middle"),
        QStringLiteral("minimized"), QStringLiteral("movable"),
        QStringLiteral("next-selected"), QStringLiteral("no-frame"),
        QStringLiteral("non-exclusive"), QStringLiteral("off"),
        QStringLiteral("on"), QStringLiteral("only-one"),
        QStringLiteral("open"), QStringLiteral("pressed"),
        QStringLiteral("previous-selected"), QStringLiteral("read-only"),
        QStringLiteral("right"), QStringLiteral("selected"),
        QStringLiteral("top"), QStringLiteral("unchecked"),
        QStringLiteral("vertical"), QStringLiteral("window")
    };
    return list;
}

const QStringList &subControls()
{
    static const QStringList list = {
        QStringLiteral("add-line"), QStringLiteral("add-page"),
        QStringLiteral("branch"), QStringLiteral("chunk"),
        QStringLiteral("close-button"), QStringLiteral("corner"),
        QStringLiteral("down-arrow"), QStringLiteral("down-button"),
        QStringLiteral("drop-down"), QStringLiteral("float-button"),
        QStringLiteral("groove"), QStringLiteral("handle"),
        QStringLiteral("icon"), QStringLiteral("indicator"),
        QStringLiteral("item"), QStringLiteral("left-arrow"),
        QStringLiteral("left-corner"), QStringLiteral("menu-arrow"),
        QStringLiteral("menu-button"), QStringLiteral("menu-indicator"),
        QStringLiteral("pane"), QStringLiteral("right-arrow"),
        QStringLiteral("right-corner"), QStringLiteral("scroller"),
        QStringLiteral("section"), QStringLiteral("separator"),
        QStringLiteral("sub-line"), QStringLiteral("sub-page"),
        QStringLiteral("tab"), QStringLiteral("tab-bar"),
        QStringLiteral("tear"), QStringLiteral("tearoff"),
        QStringLiteral("text"), QStringLiteral("title"),
        QStringLiteral("up-arrow"), QStringLiteral("up-button")
    };
    return list;
}

const QStringList &valueKeywords()
{
    static const QStringList list = {
        // Named colours
        QStringLiteral("transparent"), QStringLiteral("black"), QStringLiteral("white"),
        QStringLiteral("red"), QStringLiteral("green"), QStringLiteral("blue"),
        QStringLiteral("yellow"), QStringLiteral("cyan"), QStringLiteral("magenta"),
        QStringLiteral("gray"), QStringLiteral("grey"), QStringLiteral("darkgray"),
        QStringLiteral("darkgrey"), QStringLiteral("lightgray"), QStringLiteral("lightgrey"),
        QStringLiteral("darkred"), QStringLiteral("darkgreen"), QStringLiteral("darkblue"),
        QStringLiteral("darkcyan"), QStringLiteral("darkmagenta"), QStringLiteral("darkyellow"),
        QStringLiteral("orange"), QStringLiteral("pink"), QStringLiteral("purple"),
        QStringLiteral("brown"), QStringLiteral("navy"), QStringLiteral("teal"),
        QStringLiteral("olive"), QStringLiteral("maroon"), QStringLiteral("aqua"),
        QStringLiteral("fuchsia"), QStringLiteral("lime"), QStringLiteral("silver"),
        // Border styles
        QStringLiteral("none"), QStringLiteral("solid"), QStringLiteral("dashed"),
        QStringLiteral("dotted"), QStringLiteral("double"), QStringLiteral("groove"),
        QStringLiteral("ridge"), QStringLiteral("inset"), QStringLiteral("outset"),
        QStringLiteral("dot-dash"), QStringLiteral("dot-dot-dash"),
        // Font weight and style
        QStringLiteral("normal"), QStringLiteral("bold"), QStringLiteral("italic"),
        QStringLiteral("oblique"), QStringLiteral("underline"), QStringLiteral("overline"),
        QStringLiteral("line-through"),
        // Position and alignment
        QStringLiteral("relative"), QStringLiteral("absolute"), QStringLiteral("top"),
        QStringLiteral("bottom"), QStringLiteral("left"), QStringLiteral("right"),
        QStringLiteral("center"), QStringLiteral("margin"), QStringLiteral("border"),
        QStringLiteral("padding"), QStringLiteral("content"), QStringLiteral("repeat"),
        QStringLiteral("repeat-x"), QStringLiteral("repeat-y"), QStringLiteral("no-repeat"),
        QStringLiteral("scroll"), QStringLiteral("fixed")
    };
    return list;
}

} // namespace QssVocabulary
