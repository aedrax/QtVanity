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


# =============================================================================
# CPack Configuration for Linux Packaging
# =============================================================================

include(InstallRequiredSystemLibraries)

# Common package metadata
set(CPACK_PACKAGE_NAME "QtVanity")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QSS Editor and Widget Gallery for Qt")
set(CPACK_PACKAGE_DESCRIPTION "QtVanity is a QSS (Qt Style Sheets) editor and widget demonstration tool for Qt developers and designers. It provides a comprehensive gallery of Qt widgets in various states alongside a live QSS editor for real-time style development and testing.")
set(CPACK_PACKAGE_VENDOR "QtVanity Project")
set(CPACK_PACKAGE_CONTACT "paulsorensen5@gmail.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/aedrax/qtvanity")

# License and documentation
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# Package file name format
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

# Linux-specific CPack configuration
if(UNIX AND NOT APPLE)
    # Set generators for Linux: DEB and TGZ
    set(CPACK_GENERATOR "DEB;TGZ")
    
    # -----------------------------------------------------------------------------
    # DEB Package Configuration (Debian/Ubuntu)
    # -----------------------------------------------------------------------------
    set(CPACK_DEBIAN_PACKAGE_NAME "qtvanity")
    set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "QtVanity Maintainer <paulsorensen5@gmail.com>")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${CPACK_PACKAGE_HOMEPAGE_URL}")
    
    # Architecture detection
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i686|i386")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
    else()
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    
    # Dependencies based on Qt version
    if(QT_VERSION_MAJOR EQUAL 6)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6 (>= 6.0.0), libqt6gui6 (>= 6.0.0), libqt6core6 (>= 6.0.0)")
    else()
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.15.0), libqt5gui5 (>= 5.15.0), libqt5core5a (>= 5.15.0)")
    endif()
    
    # Enable automatic dependency detection (shlibdeps)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    
    # DEB-specific file name
    set(CPACK_DEBIAN_FILE_NAME "qtvanity_${PROJECT_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
    
    # -----------------------------------------------------------------------------
    # TGZ Archive Configuration
    # -----------------------------------------------------------------------------
    set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)
    # TGZ uses the default CPACK_PACKAGE_FILE_NAME
    
    # -----------------------------------------------------------------------------
    # Install desktop file and icon for Linux
    # -----------------------------------------------------------------------------
    
    # Create desktop file
    set(DESKTOP_FILE_CONTENT "[Desktop Entry]
Name=QtVanity
Comment=QSS Editor and Widget Gallery for Qt
Exec=qtvanity
Icon=qtvanity
Terminal=false
Type=Application
Categories=Development;Qt;
Keywords=Qt;QSS;StyleSheet;CSS;Editor;
")
    file(WRITE "${CMAKE_BINARY_DIR}/qtvanity.desktop" "${DESKTOP_FILE_CONTENT}")
    
    # Install desktop file
    install(FILES "${CMAKE_BINARY_DIR}/qtvanity.desktop"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
    )
    
    # Install icons at multiple sizes for proper desktop integration
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/qtvanity.png")
        # Find ImageMagick for icon scaling
        find_program(CONVERT_EXECUTABLE convert
            HINTS ENV PATH
            DOC "ImageMagick convert for scaling icons"
        )
        
        if(CONVERT_EXECUTABLE)
            # Standard icon sizes for freedesktop.org hicolor theme
            set(ICON_SIZES 16 32 48 64 128 256 512)
            
            foreach(SIZE ${ICON_SIZES})
                set(ICON_OUTPUT_DIR "${CMAKE_BINARY_DIR}/icons/${SIZE}x${SIZE}")
                set(ICON_OUTPUT_FILE "${ICON_OUTPUT_DIR}/qtvanity.png")
                
                # Create scaled icon at configure time
                file(MAKE_DIRECTORY "${ICON_OUTPUT_DIR}")
                execute_process(
                    COMMAND ${CONVERT_EXECUTABLE} 
                        "${CMAKE_SOURCE_DIR}/resources/qtvanity.png"
                        -resize ${SIZE}x${SIZE}
                        "${ICON_OUTPUT_FILE}"
                    RESULT_VARIABLE CONVERT_RESULT
                )
                
                if(CONVERT_RESULT EQUAL 0)
                    install(FILES "${ICON_OUTPUT_FILE}"
                        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/${SIZE}x${SIZE}/apps"
                    )
                endif()
            endforeach()
            
            message(STATUS "Icon scaling configured using ImageMagick")
        else()
            # Fallback: install original high-res icon
            message(STATUS "ImageMagick not found - installing original icon only")
            install(FILES "${CMAKE_SOURCE_DIR}/resources/qtvanity.png"
                DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps"
            )
        endif()
        
        # Also install to pixmaps as fallback for older systems
        install(FILES "${CMAKE_SOURCE_DIR}/resources/qtvanity.png"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/pixmaps"
        )
    endif()
    
endif()

# =============================================================================
# CPack Configuration for Windows Packaging
# =============================================================================

if(WIN32)
    # Set generators for Windows: NSIS, WIX, IFW, ZIP
    set(CPACK_GENERATOR "NSIS;WIX;IFW;ZIP")
    
    # -----------------------------------------------------------------------------
    # NSIS Installer Configuration
    # -----------------------------------------------------------------------------
    
    # Display name shown in installer and Add/Remove Programs
    set(CPACK_NSIS_DISPLAY_NAME "QtVanity ${PROJECT_VERSION}")
    set(CPACK_NSIS_PACKAGE_NAME "QtVanity")
    
    # Publisher information
    set(CPACK_NSIS_PUBLISHER "QtVanity Project")
    set(CPACK_NSIS_HELP_LINK "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    
    # Install/Uninstall registry keys
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "QtVanity.exe")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    
    # Start menu shortcuts
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\QtVanity.lnk' '$INSTDIR\\\\QtVanity.exe'"
    )
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$SMPROGRAMS\\\\$START_MENU\\\\QtVanity.lnk'"
    )
    
    # Enable component-based installation
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    
    # Modify PATH option (optional - disabled by default)
    set(CPACK_NSIS_MODIFY_PATH OFF)
    
    # Request admin privileges for installation
    set(CPACK_NSIS_DEFINES "RequestExecutionLevel admin")
    
    # MUI (Modern UI) settings
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/qtvanity.ico")
        set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/qtvanity.ico")
        set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/qtvanity.ico")
    endif()
    
    # Branding text
    set(CPACK_NSIS_BRANDING_TEXT "QtVanity - QSS Editor and Widget Gallery")
    
    # -----------------------------------------------------------------------------
    # WiX MSI Installer Configuration
    # -----------------------------------------------------------------------------
    
    # Unique upgrade GUID - IMPORTANT: Generate once and keep constant for upgrades
    # Generated using: uuidgen or https://www.uuidgenerator.net/
    set(CPACK_WIX_UPGRADE_GUID "A1B2C3D4-E5F6-7890-ABCD-EF1234567890")
    
    # Product GUID (auto-generated per version is fine)
    # set(CPACK_WIX_PRODUCT_GUID "...")  # Let WiX auto-generate
    
    # MSI package settings
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/qtvanity.ico")
        set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/resources/qtvanity.ico")
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/wix_banner.bmp")
        set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/resources/wix_banner.bmp")
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/wix_dialog.bmp")
        set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/resources/wix_dialog.bmp")
    endif()
    
    # License file for WiX
    set(CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/LICENSE")
    
    # Program menu folder
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "QtVanity")
    
    # Property settings
    set(CPACK_WIX_PROPERTY_ARPHELPLINK "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_WIX_PROPERTY_ARPCONTACT "${CPACK_PACKAGE_CONTACT}")
    
    # Culture/language
    set(CPACK_WIX_CULTURES "en-US")
    
    # Install scope (per-machine installation)
    set(CPACK_WIX_INSTALL_SCOPE "perMachine")
    
    # -----------------------------------------------------------------------------
    # Qt Installer Framework (IFW) Configuration
    # -----------------------------------------------------------------------------
    
    # IFW root directory (set via environment or CMake variable)
    if(DEFINED ENV{QTIFWDIR})
        set(CPACK_IFW_ROOT "$ENV{QTIFWDIR}")
    elseif(DEFINED QTIFWDIR)
        set(CPACK_IFW_ROOT "${QTIFWDIR}")
    endif()
    
    # Package metadata for IFW
    set(CPACK_IFW_PACKAGE_TITLE "QtVanity Installer")
    set(CPACK_IFW_PACKAGE_PUBLISHER "QtVanity Project")
    set(CPACK_IFW_PACKAGE_NAME "com.qtvanity.qtvanity")
    set(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY "QtVanity")
    
    # Target directory
    set(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDir@/QtVanity")
    
    # Installer window settings
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/qtvanity.ico")
        set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/resources/qtvanity.ico")
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/qtvanity.png")
        set(CPACK_IFW_PACKAGE_LOGO "${CMAKE_SOURCE_DIR}/resources/qtvanity.png")
    endif()
    
    # Maintenance tool settings
    set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "QtVanityMaintenanceTool")
    set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_INI_FILE "")
    
    # Allow non-ASCII characters in paths
    set(CPACK_IFW_PACKAGE_ALLOW_NON_ASCII_CHARACTERS ON)
    
    # Create desktop shortcut
    set(CPACK_IFW_PACKAGE_RUN_PROGRAM "@TargetDir@/QtVanity.exe")
    set(CPACK_IFW_PACKAGE_RUN_PROGRAM_DESCRIPTION "Launch QtVanity")
    
    # -----------------------------------------------------------------------------
    # ZIP Archive Configuration
    # -----------------------------------------------------------------------------
    
    # ZIP is included in CPACK_GENERATOR above
    # Uses default CPACK_PACKAGE_FILE_NAME for archive name
    # No additional configuration needed for basic ZIP archive
    
    # Archive-specific settings (applies to ZIP and other archive formats)
    set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)
    
    # -----------------------------------------------------------------------------
    # Windows-specific install configuration
    # -----------------------------------------------------------------------------
    
    # Install Visual C++ runtime (if using MSVC)
    if(MSVC)
        set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP FALSE)
        include(InstallRequiredSystemLibraries)
    endif()
    
endif()

# Include CPack module (must be after all CPACK_* variables are set)
include(CPack)

# =============================================================================
# AppImage Build Target (Linux only)
# =============================================================================

if(UNIX AND NOT APPLE)
    # Find linuxdeploy for AppImage creation
    find_program(LINUXDEPLOY_EXECUTABLE 
        NAMES linuxdeploy linuxdeploy-x86_64.AppImage
        HINTS ENV PATH
        DOC "linuxdeploy executable for creating AppImages"
    )
    
    find_program(LINUXDEPLOY_QT_PLUGIN
        NAMES linuxdeploy-plugin-qt linuxdeploy-plugin-qt-x86_64.AppImage
        HINTS ENV PATH
        DOC "linuxdeploy Qt plugin for bundling Qt dependencies"
    )
    
    if(LINUXDEPLOY_EXECUTABLE)
        # Create AppDir structure
        set(APPDIR "${CMAKE_BINARY_DIR}/AppDir")
        
        # Create AppImage target
        add_custom_target(appimage
            COMMENT "Creating AppImage package..."
            
            # Clean previous AppDir
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${APPDIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${APPDIR}"
            
            # Install to AppDir
            COMMAND ${CMAKE_COMMAND} -E env DESTDIR="${APPDIR}"
                ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --prefix /usr
            
            # Run linuxdeploy with Qt plugin
            COMMAND ${CMAKE_COMMAND} -E env 
                QMAKE=$<IF:$<BOOL:${QT_QMAKE_EXECUTABLE}>,${QT_QMAKE_EXECUTABLE},qmake>
                ${LINUXDEPLOY_EXECUTABLE}
                --appdir "${APPDIR}"
                --executable "${APPDIR}/usr/bin/QtVanity"
                --desktop-file "${APPDIR}/usr/share/applications/qtvanity.desktop"
                $<$<BOOL:${LINUXDEPLOY_QT_PLUGIN}>:--plugin qt>
                --output appimage
            
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            DEPENDS QtVanity
            VERBATIM
        )
        
        message(STATUS "AppImage target configured (use 'make appimage' or 'cmake --build . --target appimage')")
        message(STATUS "  linuxdeploy found: ${LINUXDEPLOY_EXECUTABLE}")
        if(LINUXDEPLOY_QT_PLUGIN)
            message(STATUS "  Qt plugin found: ${LINUXDEPLOY_QT_PLUGIN}")
        else()
            message(STATUS "  Qt plugin not found - Qt libraries may need manual handling")
        endif()
    else()
        message(STATUS "linuxdeploy not found - AppImage target will not be available")
        message(STATUS "  To enable AppImage creation, install linuxdeploy:")
        message(STATUS "    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage")
        message(STATUS "    chmod +x linuxdeploy-x86_64.AppImage")
        message(STATUS "  Also install the Qt plugin:")
        message(STATUS "    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage")
        message(STATUS "    chmod +x linuxdeploy-plugin-qt-x86_64.AppImage")
    endif()
endif()

# =============================================================================
# Testing Configuration
# =============================================================================

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
