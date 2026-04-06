#!/bin/bash
set -euo pipefail

# Cross-compile QtVanity for macOS arm64 using Zig
# Requires: zig, cmake, aqtinstall (pip install aqtinstall)

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-macos-arm64"
QT_VERSION="6.10.2"
MACOS_QT_DIR="${SCRIPT_DIR}/${QT_VERSION}/macos"
LINUX_QT_DIR="${SCRIPT_DIR}/${QT_VERSION}/gcc_64"
TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/macos-aarch64-zig-toolchain.cmake"
STUBS_SCRIPT="${SCRIPT_DIR}/cmake/macos-sdk-stubs/create-stubs.sh"

# ── Preflight checks ────────────────────────────────────────────────
command -v zig >/dev/null 2>&1 || { echo "ERROR: zig not found in PATH"; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "ERROR: cmake not found in PATH"; exit 1; }

echo "Using zig $(zig version)"
echo "Target: aarch64-macos (Apple Silicon)"

# ── Install macOS Qt ${QT_VERSION} (target libraries) if missing ───────────
if [ ! -d "${MACOS_QT_DIR}/lib/cmake/Qt6" ]; then
    echo ""
    echo "macOS Qt ${QT_VERSION} not found at ${MACOS_QT_DIR}"
    echo "Installing via aqtinstall..."
    command -v aqt >/dev/null 2>&1 || {
        echo "ERROR: aqtinstall not found. Install with: pip install aqtinstall"
        exit 1
    }
    aqt install-qt mac desktop ${QT_VERSION} clang_64 -O "${SCRIPT_DIR}/${QT_VERSION}"
    # aqtinstall may nest under ${QT_VERSION}/${QT_VERSION}/macos — fix if needed
    if [ -d "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/macos" ]; then
        mv "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/macos" "${MACOS_QT_DIR}"
        rmdir "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}" 2>/dev/null || true
    fi
fi
echo "macOS Qt: ${MACOS_QT_DIR}"

# ── Install Linux Qt ${QT_VERSION} for host tools (moc/rcc/uic) if missing ─
if [ ! -d "${LINUX_QT_DIR}" ]; then
    echo ""
    echo "Linux Qt ${QT_VERSION} host tools not found at ${LINUX_QT_DIR}"
    echo "Installing via aqtinstall..."
    command -v aqt >/dev/null 2>&1 || {
        echo "ERROR: aqtinstall not found. Install with: pip install aqtinstall"
        exit 1
    }
    aqt install-qt linux desktop ${QT_VERSION} linux_gcc_64 -O "${SCRIPT_DIR}/${QT_VERSION}"
    # aqtinstall may nest under ${QT_VERSION}/${QT_VERSION}/gcc_64 — fix if needed
    if [ -d "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/gcc_64" ]; then
        mv "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/gcc_64" "${LINUX_QT_DIR}"
        rmdir "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}" 2>/dev/null || true
    fi
fi

# Verify host moc exists
HOST_MOC="${LINUX_QT_DIR}/libexec/moc"
if [ ! -x "${HOST_MOC}" ]; then
    HOST_MOC="${LINUX_QT_DIR}/bin/moc"
fi
if [ ! -x "${HOST_MOC}" ]; then
    echo "ERROR: Cannot find moc in Linux Qt installation at ${LINUX_QT_DIR}"
    exit 1
fi
echo "Host moc: ${HOST_MOC} ($(${HOST_MOC} --version 2>&1))"

# ── Generate macOS SDK framework stubs if missing ────────────────────
if [ ! -d "${SCRIPT_DIR}/cmake/macos-sdk-stubs/AppKit.framework" ]; then
    echo "Generating macOS SDK framework stubs..."
    bash "${STUBS_SCRIPT}"
fi

# ── Configure ────────────────────────────────────────────────────────
echo ""
echo "Configuring..."
cmake -B "${BUILD_DIR}" \
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
    -DCMAKE_PREFIX_PATH="${MACOS_QT_DIR}" \
    -DQT_HOST_PATH="${LINUX_QT_DIR}" \
    -DQT_HOST_PATH_CMAKE_DIR="${LINUX_QT_DIR}/lib/cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DQT_VERSION_MAJOR=6 \
    -G "Unix Makefiles"

# ── Build ────────────────────────────────────────────────────────────
echo ""
echo "Building..."
cmake --build "${BUILD_DIR}" --parallel "$(nproc)"

echo ""
echo "Build complete. Output in: ${BUILD_DIR}"
echo "Binary: ${BUILD_DIR}/QtVanity.app/Contents/MacOS/QtVanity"
