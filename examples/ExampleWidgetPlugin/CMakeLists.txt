# =============================================================================
# Example Widget Plugin for QtVanity
# =============================================================================
#
# This CMakeLists.txt demonstrates how to build a custom widget plugin for
# QtVanity. The plugin implements the WidgetPluginInterface to provide custom
# widgets that can be previewed in QtVanity's Custom Widgets gallery page.
#
# Build Instructions:
# -------------------
# 1. Create a build directory:
#    mkdir build && cd build
#
# 2. Configure with CMake (adjust QTVANITY_SOURCE_DIR to point to QtVanity):
#    cmake .. -DQTVANITY_SOURCE_DIR=/path/to/qtvanity
#
# 3. Build the plugin:
#    cmake --build .
#
# 4. Copy the resulting plugin to QtVanity's plugin directory:
#    - Linux: ~/.local/share/QtVanity/plugins/
#    - Windows: %APPDATA%/QtVanity/plugins/
#    - macOS: ~/Library/Application Support/QtVanity/plugins/
#
# =============================================================================

cmake_minimum_required(VERSION 3.16)
project(ExampleWidgetPlugin VERSION 1.0.0 LANGUAGES CXX)

# =============================================================================
# C++ Standard Configuration
# =============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# Qt Auto-Processing
# =============================================================================

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# =============================================================================
# Find Qt
# =============================================================================

# Qt version detection: Qt 6 default, fallback to Qt 5
if(NOT DEFINED QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Gui REQUIRED)
else()
    find_package(QT NAMES Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui REQUIRED)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui REQUIRED)

message(STATUS "Building ExampleWidgetPlugin with Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")

# =============================================================================
# QtVanity Plugin Interface
# =============================================================================

# Path to QtVanity source directory (for WidgetPluginInterface.h)
# Users should set this when configuring the build
if(NOT DEFINED QTVANITY_SOURCE_DIR)
    # Default: assume we're in examples/ExampleWidgetPlugin/ within QtVanity
    set(QTVANITY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
endif()

# Verify the plugin interface header exists
set(PLUGIN_INTERFACE_DIR "${QTVANITY_SOURCE_DIR}/src/plugins")
if(NOT EXISTS "${PLUGIN_INTERFACE_DIR}/WidgetPluginInterface.h")
    message(FATAL_ERROR 
        "WidgetPluginInterface.h not found at ${PLUGIN_INTERFACE_DIR}\n"
        "Please set QTVANITY_SOURCE_DIR to the QtVanity source directory:\n"
        "  cmake .. -DQTVANITY_SOURCE_DIR=/path/to/qtvanity"
    )
endif()

message(STATUS "Using QtVanity plugin interface from: ${PLUGIN_INTERFACE_DIR}")

# =============================================================================
# Plugin Source Files
# =============================================================================

set(PLUGIN_SOURCES
    ExampleWidgetPlugin.cpp
    ExampleWidgetPlugin.h
)

# =============================================================================
# Create Plugin Library
# =============================================================================

# Build as MODULE library (required for Qt plugins)
add_library(${PROJECT_NAME} MODULE ${PLUGIN_SOURCES})

# =============================================================================
# Include Directories
# =============================================================================

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLUGIN_INTERFACE_DIR}
)

# =============================================================================
# Link Qt Libraries
# =============================================================================

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

# =============================================================================
# Plugin Output Configuration
# =============================================================================

# Set output name (without lib prefix on Unix)
set_target_properties(${PROJECT_NAME} PROPERTIES
    # Remove 'lib' prefix on Unix systems
    PREFIX ""
    # Set output directory
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    # Plugin-specific properties
    POSITION_INDEPENDENT_CODE ON
)

# Platform-specific suffix configuration
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".dll"
    )
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".dylib"
    )
else()
    # Linux/Unix
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".so"
    )
endif()

# =============================================================================
# Installation (Optional)
# =============================================================================

# Install the plugin to a standard location
# Users can customize PLUGIN_INSTALL_DIR
if(NOT DEFINED PLUGIN_INSTALL_DIR)
    if(WIN32)
        set(PLUGIN_INSTALL_DIR "$ENV{APPDATA}/QtVanity/plugins")
    elseif(APPLE)
        set(PLUGIN_INSTALL_DIR "$ENV{HOME}/Library/Application Support/QtVanity/plugins")
    else()
        set(PLUGIN_INSTALL_DIR "$ENV{HOME}/.local/share/QtVanity/plugins")
    endif()
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${PLUGIN_INSTALL_DIR}"
    RUNTIME DESTINATION "${PLUGIN_INSTALL_DIR}"
)

message(STATUS "Plugin will be installed to: ${PLUGIN_INSTALL_DIR}")

# =============================================================================
# Build Summary
# =============================================================================

message(STATUS "")
message(STATUS "ExampleWidgetPlugin Configuration Summary:")
message(STATUS "  Qt Version: ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Plugin Interface: ${PLUGIN_INTERFACE_DIR}")
message(STATUS "  Output Directory: ${CMAKE_BINARY_DIR}/plugins")
message(STATUS "")
