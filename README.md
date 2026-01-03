# QtVanity

**QtVanity** is a QSS (Qt Style Sheets) editor and a "kitchen sink" widget demonstration aimed at developers and designers who want to perfect the look and feel of their Qt applications.

It serves two primary purposes:
1.  **The Runway (Demo):** A gallery displaying every available Qt Widget in various states (enabled, disabled, focused, checked, etc.), allowing you to see exactly how your styles affect the entire component library.
2.  **The Mirror (Editor):** A live QSS editor that allows you to write style sheets and see the changes apply instantly to the running application.

## Features

* **Real-time Editing:** Type QSS code and see changes apply instantly to the entire application.
* **Complete Widget Zoo:** Includes specific tabs/pages for all standard widgets (Buttons, Inputs, Views, Containers, Dialogs) to make sure no UI element is left unstyled.
* **State Simulation:** Easily toggle widgets between enabled/disabled and read-only/editable states to test pseudo-states like `:disabled` or `:hover`.
* **Dual-Stack Support:** Native support for both **Qt 5 (5.15+)** and **Qt 6**.
* **Syntax Highlighting:** (Planned) Basic syntax highlighting for the QSS editor.

## Prerequisites

* C++20 compatible compiler (GCC, Clang, or MSVC)
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

3. Compile

```bash
cmake --build . --config Release
```

## Project Structure

- `src/main.cpp`: Entry point detecting version and launching the app.
- `src/editor/`: Contains the logic for the QSS text editor and syntax highlighter.
- `src/gallery/`: Contains the "Widget Zoo" classes. Broken down by category (e.g., `InputWidgets`, `ItemViews`, `Layouts`).
- `styles/`: Predefined QSS templates (e.g., Dark, Light, Solarized) to use as starting points.
- `resources/`: Default themes and icon assets.

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
