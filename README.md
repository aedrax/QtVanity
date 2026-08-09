# QtVanity

**QtVanity** is a QSS (Qt Style Sheets) editor and a "kitchen sink" widget demonstration aimed at developers and designers who want to perfect the look and feel of their Qt applications.

<img width="1920" height="1156" alt="image" src="https://github.com/user-attachments/assets/234ec4a0-dd6d-4c3f-b0e1-532a003199f7" />


It serves two primary purposes:
1.  **The Runway (Demo):** A gallery displaying every available Qt Widget in various states (enabled, disabled, focused, checked, etc.), allowing you to see exactly how your styles affect the entire component library.
2.  **The Mirror (Editor):** A live QSS editor that allows you to write style sheets and see the changes apply instantly to the running application.

## Features

* **Real-time Editing:** Type QSS code and see the Widget Gallery restyle as you type.
* **Scoped Preview:** By default the stylesheet under test is applied to the gallery only, so a stylesheet that renders text invisible cannot take the editor and menus with it. Toggle **Style whole app** in the toolbar to preview `QMainWindow`, `QDockWidget`, `QMenuBar` and `QStatusBar` rules.
* **Complete Widget Zoo:** Includes specific tabs/pages for all standard widgets (Buttons, Inputs, Views, Containers, Dialogs, Display, MainWindow, Advanced) to make sure no UI element is left unstyled.
* **State Simulation:** Toggle the gallery between enabled/disabled, read-only/editable and checked/unchecked states, mirror it right-to-left, and scale its fonts. (`:hover` and `:pressed` cannot be pinned on live widgets and are not offered.)
* **Dual-Stack Support:** Native support for both **Qt 5 (5.15+)** and **Qt 6**.
* **Code Editor:** Line numbers, current-line highlighting, bracket matching, auto-indent, Ctrl+scroll zoom, find/replace, and completion for selectors, properties, pseudo-states, sub-controls and your own variables.
* **Syntax Highlighting:** Full syntax highlighting for QSS selectors, properties, pseudo-states, sub-controls, and comments, in light and dark schemes.
* **Problem Reporting:** Qt reports a malformed stylesheet by logging a warning and applying nothing. QtVanity captures that and shows it under the editor, along with a count of undefined `${variable}` references.
* **Color Swatch Overlay:** Visual color preview for color values in your stylesheet.
* **Variable Management:** Define and manage QSS variables for reusable style values.

## Prerequisites

* C++17 compatible compiler (GCC, Clang, or MSVC)
* CMake (3.16 or higher)
* **Qt SDK**: Either Qt 5 or Qt 6 must be installed on your system.

## Building

QtVanity uses CMake to handle the build configuration for both Qt 5 and Qt 6. By default, it will attempt to find Qt 6; if not found, it falls back to Qt 5.

### 1. Clone the repository
```bash
git clone https://github.com/aedrax/QtVanity.git
cd qtvanity
mkdir build
cd build
```

### 2. Configure

**Option A: Building with Qt 6 (Recommended)**
If Qt 6 is in your PATH, simply run:

```bash
cmake ..
```

If you need to specify the path explicitly:

```bash
cmake -DQT_VERSION_MAJOR=6 -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2019_64" ..
```

**Option B: Building with Qt 5**
To force the build to use Qt 5:

```bash
cmake -DQT_VERSION_MAJOR=5 -DCMAKE_PREFIX_PATH="/path/to/qt5" ..
```

### 3. Compile

```bash
cmake --build . --config Release
```

### 4. Run Tests (Optional)

```bash
ctest --output-on-failure
```

## Creating Distribution Packages

QtVanity uses CPack to generate platform-native installers and packages. After building, you can create packages using the following commands.

### Linux Packages

**Debian/Ubuntu (.deb):**
```bash
cpack -G DEB
```

**Tarball (.tar.gz):**
```bash
cpack -G TGZ
```

**AppImage (requires linuxdeploy):**

First, install linuxdeploy and the Qt plugin somewhere in your PATH:
```bash
# Download to /usr/local/bin or ~/bin (must be in PATH)
sudo wget -O /usr/local/bin/linuxdeploy https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
sudo wget -O /usr/local/bin/linuxdeploy-plugin-qt https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
sudo chmod +x /usr/local/bin/linuxdeploy /usr/local/bin/linuxdeploy-plugin-qt
```

Then **re-run CMake** to detect linuxdeploy and create the appimage target:
```bash
cmake ..
cmake --build . --target appimage
```

> **Note:** The `appimage` target is only available if CMake finds `linuxdeploy` during configuration. Check the CMake output for "AppImage target configured" to confirm it's available.

### Windows Packages

**NSIS Installer:**
```bash
cpack -G NSIS
```

**WiX MSI Installer:**
```bash
cpack -G WIX
```

**ZIP Archive (Portable):**
```bash
cpack -G ZIP
```

**Qt Installer Framework:**
```bash
cpack -G IFW
```

### macOS Packages

**DMG Disk Image:**
```bash
cpack -G DragNDrop
```

**productbuild Package:**
```bash
cpack -G productbuild
```

**Qt Installer Framework:**
```bash
cpack -G IFW
```

### Package Output

Generated packages will be placed in the build directory with names like:
- `qtvanity_1.0.0_amd64.deb` (Linux DEB)
- `QtVanity-1.0.0-Linux-x86_64.tar.gz` (Linux TGZ)
- `QtVanity-1.0.0-win64.exe` (Windows NSIS)
- `QtVanity-1.0.0.dmg` (macOS DMG)

## Project Structure

- `src/main.cpp`: Entry point detecting version and launching the app.
- `src/MainWindow.cpp/h`: Main application window.
- `src/editor/`: QSS editor components including:
  - `QssEditor`: Editor pane, holding the code editor, find bar and problems strip
  - `CodeEditor`: QPlainTextEdit with gutter, bracket matching, indent and completion
  - `QssSyntaxHighlighter`: Syntax highlighting for QSS
  - `QssVocabulary`: The QSS word lists shared by the highlighter and the completer
  - `QssDiagnostics`: Captures the parse errors Qt logs when applying a stylesheet
  - `StyleManager`: Style application management
  - `ThemeManager`: Theme loading and management
  - `VariableManager` & `VariablePanel`: QSS variable handling
  - `SettingsManager`: Application settings persistence
  - `FindReplaceBar`: Find and replace functionality
  - `ColorSwatchOverlay`: Color preview overlay
- `src/gallery/`: Widget demonstration pages by category:
  - `ButtonsPage`: QPushButton, QToolButton, QRadioButton, QCheckBox
  - `InputsPage`: QLineEdit, QTextEdit, QSpinBox, QComboBox, QSlider
  - `ViewsPage`: QListView, QTreeView, QTableView, QColumnView
  - `ContainersPage`: QGroupBox, QTabWidget, QScrollArea, QSplitter
  - `DialogsPage`: QMessageBox, QFileDialog, QColorDialog, QFontDialog
  - `DisplayPage`: QLabel, QProgressBar, QLCDNumber, QCalendarWidget
  - `MainWindowPage`: QMenuBar, QToolBar, QStatusBar, QDockWidget
  - `AdvancedPage`: Complex widget configurations
- `styles/`: Predefined templates as `.qvp` project files (dark, light, solarized, shadcn, spotify, outrun, vscode-dark, vscode-light).
- `resources/`: Application icons and platform-specific resources.
- `tests/`: Qt Test-based unit tests.
- `cmake/`: Platform-specific deployment helper scripts.

## Usage

1. Launch QtVanity. On first run it loads a starter template so you can see the effect immediately.
2. The **Widget Gallery** dock shows every standard Qt widget, grouped into tabs. Use the filter box to find one across all tabs.
3. The centre pane is the QSS editor. With **Live preview** on (the default) the gallery restyles shortly after you stop typing; otherwise press **Apply**.
4. The **Variables** dock defines reusable values. Reference one as `${name}` in the editor; rename a variable and its references are rewritten for you.
5. Use **File → Load Template** for a predefined style, **File → Import QSS...** to bring in a plain `.qss` file, and **File → Export QSS...** to write out the resolved stylesheet with all variables substituted.

### Project files

QtVanity projects are `.qvp` files: JSON holding your variables alongside the
stylesheet template that references them.

```json
{
  "version": 1,
  "variables": { "background": "#1e1e1e", "radius": "6px" },
  "qssTemplate": "QWidget { background: ${background}; border-radius: ${radius}; }"
}
```

`Export QSS` writes the same content with every `${...}` resolved, ready to
ship in your own application.

## Contributing

Pull requests are welcome!

- If you find a widget configuration or state that is missing from the gallery, please submit a PR to `src/gallery`.
- Make sure any logic changes are guarded with `#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)` if they rely on Qt 6 specific APIs.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
