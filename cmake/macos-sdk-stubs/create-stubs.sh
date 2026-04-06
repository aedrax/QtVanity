#!/bin/bash
# Create minimal .tbd framework stubs for macOS system frameworks
# needed by Qt when cross-compiling from Linux with zig.
# These tell zig's linker the frameworks exist at their standard paths.

STUB_DIR="$(cd "$(dirname "$0")" && pwd)"

create_framework_stub() {
    local name="$1"
    local install_path="$2"
    local fw_dir="$STUB_DIR/${name}.framework"
    mkdir -p "$fw_dir"
    cat > "$fw_dir/${name}.tbd" << EOF
--- !tapi-tbd
tbd-version:     4
targets:         [ x86_64-macos, arm64-macos, arm64e-macos ]
install-name:    '${install_path}'
current-version: 0
compatibility-version: 0
...
EOF
}

# System frameworks Qt depends on
create_framework_stub "AppKit"                "/System/Library/Frameworks/AppKit.framework/Versions/C/AppKit"
create_framework_stub "OpenGL"                "/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL"
create_framework_stub "ImageIO"               "/System/Library/Frameworks/ImageIO.framework/Versions/A/ImageIO"
create_framework_stub "Metal"                 "/System/Library/Frameworks/Metal.framework/Versions/A/Metal"
create_framework_stub "IOKit"                 "/System/Library/Frameworks/IOKit.framework/Versions/A/IOKit"
create_framework_stub "DiskArbitration"        "/System/Library/Frameworks/DiskArbitration.framework/Versions/A/DiskArbitration"
create_framework_stub "UniformTypeIdentifiers" "/System/Library/Frameworks/UniformTypeIdentifiers.framework/Versions/A/UniformTypeIdentifiers"
create_framework_stub "Foundation"            "/System/Library/Frameworks/Foundation.framework/Versions/C/Foundation"
create_framework_stub "CoreFoundation"        "/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation"
create_framework_stub "CoreGraphics"          "/System/Library/Frameworks/CoreGraphics.framework/Versions/A/CoreGraphics"
create_framework_stub "CoreText"              "/System/Library/Frameworks/CoreText.framework/Versions/A/CoreText"
create_framework_stub "CoreServices"          "/System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices"
create_framework_stub "Security"              "/System/Library/Frameworks/Security.framework/Versions/A/Security"
create_framework_stub "SystemConfiguration"   "/System/Library/Frameworks/SystemConfiguration.framework/Versions/A/SystemConfiguration"
create_framework_stub "IOSurface"             "/System/Library/Frameworks/IOSurface.framework/Versions/A/IOSurface"
create_framework_stub "QuartzCore"            "/System/Library/Frameworks/QuartzCore.framework/Versions/A/QuartzCore"
create_framework_stub "Carbon"                "/System/Library/Frameworks/Carbon.framework/Versions/A/Carbon"
create_framework_stub "Cocoa"                 "/System/Library/Frameworks/Cocoa.framework/Versions/A/Cocoa"
create_framework_stub "ApplicationServices"   "/System/Library/Frameworks/ApplicationServices.framework/Versions/A/ApplicationServices"
create_framework_stub "AGL"                   "/System/Library/Frameworks/AGL.framework/Versions/A/AGL"

echo "Created macOS framework stubs in: $STUB_DIR"
