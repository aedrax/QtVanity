cmake_minimum_required(VERSION 3.16)
project(QtVanity VERSION 1.0.0 LANGUAGES CXX)

# C++17 standard requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt auto-processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt version detection: Qt 6 default, fallback to Qt 5
if(NOT DEFINED QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Gui REQUIRED)
else()
    find_package(QT NAMES Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui REQUIRED)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui REQUIRED)

message(STATUS "Building with Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")

# Source files
set(SOURCES
    src/main.cpp
)

# Create executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_standard_project_setup()
    qt_add_executable(QtVanity ${SOURCES})
else()
    add_executable(QtVanity ${SOURCES})
endif()

# Link Qt libraries
target_link_libraries(QtVanity PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

# Include directories
target_include_directories(QtVanity PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gallery
)

# Install targets
install(TARGETS QtVanity
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install style templates
install(DIRECTORY styles/
    DESTINATION share/qtvanity/styles
    FILES_MATCHING PATTERN "*.qss"
)

# Install resources
install(DIRECTORY resources/
    DESTINATION share/qtvanity/resources
)
