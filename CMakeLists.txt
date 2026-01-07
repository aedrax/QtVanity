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
    src/MainWindow.cpp
    src/MainWindow.h
    src/editor/StyleManager.cpp
    src/editor/StyleManager.h
    src/editor/QssSyntaxHighlighter.cpp
    src/editor/QssSyntaxHighlighter.h
    src/editor/QssEditor.cpp
    src/editor/QssEditor.h
    src/gallery/GalleryPage.cpp
    src/gallery/GalleryPage.h
    src/gallery/ButtonsPage.cpp
    src/gallery/ButtonsPage.h
    src/gallery/InputsPage.cpp
    src/gallery/InputsPage.h
    src/gallery/ViewsPage.cpp
    src/gallery/ViewsPage.h
    src/gallery/ContainersPage.cpp
    src/gallery/ContainersPage.h
    src/gallery/DialogsPage.cpp
    src/gallery/DialogsPage.h
    src/gallery/WidgetGallery.cpp
    src/gallery/WidgetGallery.h
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

# =============================================================================
# Deployment Script Integration
# =============================================================================

# Qt 6 deployment using qt_generate_deploy_app_script
if(QT_VERSION_MAJOR EQUAL 6)
    # Generate deployment script for Qt 6
    # This handles platform-specific deployment (windeployqt, macdeployqt, etc.)
    qt_generate_deploy_app_script(
        TARGET QtVanity
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    
    # Install the deployment script to be executed during installation
    install(SCRIPT ${deploy_script})
    
    message(STATUS "Qt 6 deployment script configured")
else()
    # Qt 5 fallback - manual deployment handling
    # For Qt 5, deployment must be handled manually or via platform-specific tools
    message(STATUS "Qt 5 detected - manual deployment required")
    message(STATUS "  Windows: Use windeployqt manually after build")
    message(STATUS "  macOS: Use macdeployqt manually after build")
    message(STATUS "  Linux: Make sure Qt libraries are available in system or use linuxdeployqt")
    
    # Create a helper script for Qt 5 deployment (optional convenience)
    if(WIN32)
        # Windows Qt 5 deployment helper
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy_qt5_windows.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/deploy_qt5_windows.cmake"
            @ONLY
        )
    elseif(APPLE)
        # macOS Qt 5 deployment helper
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy_qt5_macos.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/deploy_qt5_macos.cmake"
            @ONLY
        )
    endif()
endif()

# =============================================================================
# Install Targets
# =============================================================================

# Platform-specific install destinations
if(WIN32)
    set(QTVANITY_INSTALL_BINDIR ".")
    set(QTVANITY_INSTALL_DATADIR ".")
elseif(APPLE)
    set(QTVANITY_INSTALL_BINDIR ".")
    set(QTVANITY_INSTALL_DATADIR "QtVanity.app/Contents/Resources")
else()
    # Linux/Unix
    include(GNUInstallDirs)
    set(QTVANITY_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}")
    set(QTVANITY_INSTALL_DATADIR "${CMAKE_INSTALL_DATADIR}/qtvanity")
endif()

# Install executable
install(TARGETS QtVanity
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${QTVANITY_INSTALL_BINDIR}
)

# Install style templates
install(DIRECTORY styles/
    DESTINATION ${QTVANITY_INSTALL_DATADIR}/styles
    FILES_MATCHING PATTERN "*.qss"
)

# Install resources
install(DIRECTORY resources/
    DESTINATION ${QTVANITY_INSTALL_DATADIR}/resources
    PATTERN ".gitkeep" EXCLUDE
)

# =============================================================================
# macOS Bundle Configuration
# =============================================================================

if(APPLE)
    set_target_properties(QtVanity PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.qtvanity.QtVanity"
        MACOSX_BUNDLE_BUNDLE_NAME "QtVanity"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in"
    )
    
    # Copy styles into the bundle
    install(DIRECTORY styles/
        DESTINATION QtVanity.app/Contents/Resources/styles
        FILES_MATCHING PATTERN "*.qss"
    )
endif()

# =============================================================================
# Windows Configuration
# =============================================================================

if(WIN32)
    set_target_properties(QtVanity PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()


# Testing configuration
option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
    enable_testing()
    
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)
    
    # Create a library target for testable code
    add_library(qtvanity_lib STATIC
        src/MainWindow.cpp
        src/MainWindow.h
        src/editor/StyleManager.cpp
        src/editor/StyleManager.h
        src/editor/QssSyntaxHighlighter.cpp
        src/editor/QssSyntaxHighlighter.h
        src/editor/QssEditor.cpp
        src/editor/QssEditor.h
        src/gallery/GalleryPage.cpp
        src/gallery/GalleryPage.h
        src/gallery/ButtonsPage.cpp
        src/gallery/ButtonsPage.h
        src/gallery/InputsPage.cpp
        src/gallery/InputsPage.h
        src/gallery/ViewsPage.cpp
        src/gallery/ViewsPage.h
        src/gallery/ContainersPage.cpp
        src/gallery/ContainersPage.h
        src/gallery/DialogsPage.cpp
        src/gallery/DialogsPage.h
        src/gallery/WidgetGallery.cpp
        src/gallery/WidgetGallery.h
    )
    
    target_link_libraries(qtvanity_lib PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Gui
    )
    
    target_include_directories(qtvanity_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gallery
    )
    
    # Test executable
    add_executable(qtvanity_tests
        tests/test_main.cpp
        tests/test_stylemanager.cpp
        tests/test_stylemanager.h
        tests/test_qsssyntaxhighlighter.cpp
        tests/test_qsssyntaxhighlighter.h
        tests/test_qsseditor.cpp
        tests/test_qsseditor.h
        tests/test_widgetgallery.cpp
        tests/test_widgetgallery.h
        tests/test_mainwindow.cpp
        tests/test_mainwindow.h
    )
    
    target_link_libraries(qtvanity_tests PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        qtvanity_lib
    )
    
    target_include_directories(qtvanity_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    
    # Register tests with CTest
    add_test(NAME qtvanity_tests COMMAND qtvanity_tests)
endif()
