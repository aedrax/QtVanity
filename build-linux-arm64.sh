#!/bin/bash
set -euo pipefail

# Cross-compile QtVanity for Linux aarch64 (ARM64) using Zig
# Requires: zig, cmake, aqtinstall (pip install aqtinstall)

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-linux-arm64"
QT_VERSION="6.10.2"
ARM64_QT_DIR="${SCRIPT_DIR}/${QT_VERSION}/gcc_arm64"
LINUX_QT_DIR="${SCRIPT_DIR}/${QT_VERSION}/gcc_64"
TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/linux-aarch64-zig-toolchain.cmake"

# ── Preflight checks ────────────────────────────────────────────────
command -v zig >/dev/null 2>&1 || { echo "ERROR: zig not found in PATH"; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "ERROR: cmake not found in PATH"; exit 1; }

# Check for aarch64 cross-compile libstdc++ (needed because Qt ARM64 libs use libstdc++)
if [ ! -d "/usr/aarch64-linux-gnu/include/c++" ]; then
    echo "ERROR: aarch64 cross-compile C++ headers not found."
    echo "Install with: sudo apt install g++-aarch64-linux-gnu"
    exit 1
fi

echo "Using zig $(zig version)"
echo "Target: aarch64-linux-gnu (Linux ARM64)"

# ── Install Linux ARM64 Qt ${QT_VERSION} (target libraries) if missing ─────
if [ ! -d "${ARM64_QT_DIR}/lib/cmake/Qt6" ]; then
    echo ""
    echo "Linux ARM64 Qt ${QT_VERSION} not found at ${ARM64_QT_DIR}"
    echo "Installing via aqtinstall..."
    command -v aqt >/dev/null 2>&1 || {
        echo "ERROR: aqtinstall not found. Install with: pip install aqtinstall"
        exit 1
    }
    aqt install-qt linux_arm64 desktop ${QT_VERSION} linux_gcc_arm64 -O "${SCRIPT_DIR}/${QT_VERSION}"
    # aqtinstall may nest under ${QT_VERSION}/${QT_VERSION}/gcc_arm64 — fix if needed
    if [ -d "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/gcc_arm64" ]; then
        mv "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/gcc_arm64" "${ARM64_QT_DIR}"
        rmdir "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}" 2>/dev/null || true
    elif [ -d "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/linux_gcc_arm64" ]; then
        mv "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/linux_gcc_arm64" "${ARM64_QT_DIR}"
        rmdir "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}" 2>/dev/null || true
    fi
fi

# ── Resolve Linux ARM64 Qt directory name variant ────────────────────
# aqtinstall produces "gcc_arm64" for linux_arm64,
# but older versions or manual installs may use "linux_gcc_arm64".
if [ ! -d "${ARM64_QT_DIR}/lib/cmake/Qt6" ]; then
    for variant in "gcc_arm64" "linux_gcc_arm64"; do
        candidate="${SCRIPT_DIR}/${QT_VERSION}/${variant}"
        if [ -d "${candidate}/lib/cmake/Qt6" ]; then
            ARM64_QT_DIR="${candidate}"
            break
        fi
    done
fi

if [ ! -d "${ARM64_QT_DIR}/lib/cmake/Qt6" ]; then
    echo "ERROR: Cannot find Linux ARM64 Qt libraries at ${ARM64_QT_DIR}"
    echo "Checked variants: gcc_arm64, linux_gcc_arm64 under ${SCRIPT_DIR}/${QT_VERSION}/"
    exit 1
fi
echo "Linux ARM64 Qt (resolved): ${ARM64_QT_DIR}"

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

# ── Configure ────────────────────────────────────────────────────────
echo ""
echo "Configuring..."
cmake -B "${BUILD_DIR}" \
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
    -DCMAKE_PREFIX_PATH="${ARM64_QT_DIR}" \
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
echo "Binary: ${BUILD_DIR}/QtVanity"
