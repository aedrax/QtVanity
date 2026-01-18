# QtVanity

**QtVanity** is a QSS (Qt Style Sheets) editor and a "kitchen sink" widget demonstration aimed at developers and designers who want to perfect the look and feel of their Qt applications.

<img width="1920" height="1156" alt="image" src="https://github.com/user-attachments/assets/234ec4a0-dd6d-4c3f-b0e1-532a003199f7" />


It serves two primary purposes:
1.  **The Runway (Demo):** A gallery displaying every available Qt Widget in various states (enabled, disabled, focused, checked, etc.), allowing you to see exactly how your styles affect the entire component library.
2.  **The Mirror (Editor):** A live QSS editor that allows you to write style sheets and see the changes apply instantly to the running application.

## Features

* **Real-time Editing:** Type QSS code and see changes apply instantly to the entire application.
* **Complete Widget Zoo:** Includes specific tabs/pages for all standard widgets (Buttons, Inputs, Views, Containers, Dialogs, Display, MainWindow, Advanced) to make sure no UI element is left unstyled.
* **State Simulation:** Easily toggle widgets between enabled/disabled and read-only/editable states to test pseudo-states like `:disabled` or `:hover`.
* **Dual-Stack Support:** Native support for both **Qt 5 (5.15+)** and **Qt 6**.
* **Syntax Highlighting:** Full syntax highlighting for QSS selectors, properties, pseudo-states, sub-controls, and comments.
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
  - `QssEditor`: Main text editor widget
  - `QssSyntaxHighlighter`: Syntax highlighting for QSS
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
- `styles/`: Predefined QSS templates (dark.qss, light.qss, solarized.qss).
- `resources/`: Application icons and platform-specific resources.
- `tests/`: Qt Test-based unit tests.
- `cmake/`: Platform-specific deployment helper scripts.

## Usage

1. Launch QtVanity.
2. On the left (or top) pane, navigate through the tabs to view different categories of widgets.
3. On the right (or bottom) pane, enter your QSS code.
4. Press **Apply** (or enable "Auto-Apply") to see your styles take effect immediately.
5. Use **File > Load Style...** to import existing `.qss` files to test them against the full widget suite.

## Contributing

Pull requests are welcome!

- If you find a widget configuration or state that is missing from the gallery, please submit a PR to `src/gallery`.
- Make sure any logic changes are guarded with `#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)` if they rely on Qt 6 specific APIs.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
