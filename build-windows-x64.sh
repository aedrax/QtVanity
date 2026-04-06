#!/bin/bash
set -euo pipefail

# Cross-compile QtVanity for Windows x86_64 using Zig
# Requires: zig, cmake, aqtinstall (pip install aqtinstall)

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-windows-x64"
QT_VERSION="6.10.2"
WIN_QT_DIR="${SCRIPT_DIR}/${QT_VERSION}/llvm-mingw_64"
LINUX_QT_DIR="${SCRIPT_DIR}/${QT_VERSION}/gcc_64"
TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/windows-x86_64-zig-toolchain.cmake"

# ── Preflight checks ────────────────────────────────────────────────
command -v zig >/dev/null 2>&1 || { echo "ERROR: zig not found in PATH"; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "ERROR: cmake not found in PATH"; exit 1; }

echo "Using zig $(zig version)"
echo "Target: x86_64-windows-gnu (Windows 64-bit)"

# ── Install Windows Qt ${QT_VERSION} (target libraries) if missing ─────────
if [ ! -d "${WIN_QT_DIR}/lib/cmake/Qt6" ]; then
    echo ""
    echo "Windows Qt ${QT_VERSION} not found at ${WIN_QT_DIR}"
    echo "Installing via aqtinstall..."
    command -v aqt >/dev/null 2>&1 || {
        echo "ERROR: aqtinstall not found. Install with: pip install aqtinstall"
        exit 1
    }
    aqt install-qt windows desktop ${QT_VERSION} win64_llvm_mingw -O "${SCRIPT_DIR}/${QT_VERSION}"
    # aqtinstall may nest under ${QT_VERSION}/${QT_VERSION}/llvm-mingw_64 — fix if needed
    if [ -d "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/llvm-mingw_64" ]; then
        mv "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/llvm-mingw_64" "${WIN_QT_DIR}"
        rmdir "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}" 2>/dev/null || true
    elif [ -d "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/llvm_mingw" ]; then
        mv "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}/llvm_mingw" "${WIN_QT_DIR}"
        rmdir "${SCRIPT_DIR}/${QT_VERSION}/${QT_VERSION}" 2>/dev/null || true
    fi
fi

# ── Resolve Windows Qt directory name variant ────────────────────────
# aqtinstall produces "llvm-mingw_64" (with hyphen) for win64_llvm_mingw,
# but older versions or manual installs may use "llvm_mingw" (with underscore).
if [ ! -d "${WIN_QT_DIR}/lib/cmake/Qt6" ]; then
    for variant in "llvm-mingw_64" "llvm_mingw"; do
        candidate="${SCRIPT_DIR}/${QT_VERSION}/${variant}"
        if [ -d "${candidate}/lib/cmake/Qt6" ]; then
            WIN_QT_DIR="${candidate}"
            break
        fi
    done
fi

if [ ! -d "${WIN_QT_DIR}/lib/cmake/Qt6" ]; then
    echo "ERROR: Cannot find Windows Qt libraries at ${WIN_QT_DIR}"
    echo "Checked variants: llvm-mingw_64, llvm_mingw under ${SCRIPT_DIR}/${QT_VERSION}/"
    exit 1
fi
echo "Windows Qt (resolved): ${WIN_QT_DIR}"

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
    -DCMAKE_PREFIX_PATH="${WIN_QT_DIR}" \
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
echo "Binary: ${BUILD_DIR}/QtVanity.exe"
