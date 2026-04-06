# Cross-Compiling QtVanity for Windows (x86_64) from Linux

This guide explains how to cross-compile QtVanity for 64-bit Windows
from a Linux host using [Zig](https://ziglang.org/) as the C/C++ cross-compiler.

## Prerequisites

| Tool | Version | Install |
|------|---------|---------|
| Zig | 0.15+ | [ziglang.org/download](https://ziglang.org/download/) or `zvm install 0.15.2` |
| CMake | 3.16+ | `apt install cmake` |
| aqtinstall | 3.x | `pipx install aqtinstall` |

## Quick Start

```bash
./build-windows-x64.sh
```

The script handles everything automatically:
1. Downloads Windows Qt 6.10.2 libraries (if missing)
2. Downloads Linux Qt 6.10.2 host tools (if missing)
3. Configures and builds the project

The output binary lands at `build-windows-x64/QtVanity.exe`.

## How It Works

### The Problem

Cross-compiling a Qt C++ app for Windows from Linux requires solving three issues:

1. **Compiler**: Need a C/C++ compiler that can emit PE/COFF x86_64 binaries
2. **Qt host tools**: `moc`, `rcc`, and `uic` must run on the build host (Linux),
   but the Windows Qt ships Windows-native binaries
3. **OpenGL**: Qt6Gui links against OpenGL, which isn't natively available on
   the Linux host for a Windows target

### The Solution

**Zig as cross-compiler** — Zig bundles a Clang-based C/C++ compiler that can
target `x86_64-windows-gnu` out of the box, eliminating the need for a separate
MinGW installation. Wrapper scripts (`cmake/zig-x86_64-windows-cc` and
`cmake/zig-x86_64-windows-cxx`) adapt Zig for CMake by filtering out
`--target`/`-target` flags that CMake injects (Zig handles targeting internally).

**Dual Qt installations** — Two copies of Qt 6.10.2 are used:
- `6.10.2/llvm-mingw_64/` — Windows Qt (headers, static libs, cmake configs) for the target
- `6.10.2/gcc_64/` — Linux Qt (moc, rcc, uic) for the host

CMake's `QT_HOST_PATH` mechanism tells Qt's build system to use Linux tools
while linking against Windows libraries.

**OpenGL fallback** — A prereqs file (`cmake/windows-cross-prereqs.cmake`)
creates imported `OpenGL::GL` and `WrapOpenGL::WrapOpenGL` targets that link
against `-lopengl32`, satisfying Qt6Gui's OpenGL dependency without requiring
a Windows SDK on the Linux host.

## File Layout

```
cmake/
├── windows-x86_64-zig-toolchain.cmake  # CMake toolchain file
├── windows-cross-prereqs.cmake         # OpenGL fallback for cross-compile
├── zig-x86_64-windows-cc              # Zig C compiler wrapper
└── zig-x86_64-windows-cxx             # Zig C++ compiler wrapper

build-windows-x64.sh                   # Automated build script
CROSS_COMPILE_WINDOWS.md               # This file
```

## Manual Build Steps

If you prefer not to use the build script:

```bash
# 1. Install Windows Qt (target libraries)
aqt install-qt windows desktop 6.10.2 win64_llvm_mingw -O 6.10.2

# 2. Install Linux Qt (host tools: moc/rcc/uic)
aqt install-qt linux desktop 6.10.2 linux_gcc_64 -O 6.10.2

# 3. Configure
cmake -B build-windows-x64 \
    -DCMAKE_TOOLCHAIN_FILE=cmake/windows-x86_64-zig-toolchain.cmake \
    -DCMAKE_PREFIX_PATH="$(pwd)/6.10.2/llvm-mingw_64" \
    -DQT_HOST_PATH="$(pwd)/6.10.2/gcc_64" \
    -DQT_HOST_PATH_CMAKE_DIR="$(pwd)/6.10.2/gcc_64/lib/cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DQT_VERSION_MAJOR=6 \
    -G "Unix Makefiles"

# 4. Build
cmake --build build-windows-x64 --parallel $(nproc)
```

## Troubleshooting

### `ERROR: zig not found in PATH`

Install Zig 0.15+ from [ziglang.org/download](https://ziglang.org/download/)
or via a version manager:

```bash
zvm install 0.15.2
```

### `ERROR: cmake not found in PATH`

Install CMake 3.16+:

```bash
apt install cmake
```

### `ERROR: Cannot find Windows Qt libraries`

The build script checks for both `llvm-mingw_64` (current aqtinstall output)
and `llvm_mingw` (legacy name) under `6.10.2/`. If neither is found, re-run
the aqtinstall command manually:

```bash
aqt install-qt windows desktop 6.10.2 win64_llvm_mingw -O 6.10.2
```

Then check which directory name was created and verify it contains
`lib/cmake/Qt6`.

### `ERROR: Cannot find moc in Linux Qt installation`

The Linux Qt host tools aren't installed. Run:

```bash
aqt install-qt linux desktop 6.10.2 linux_gcc_64 -O 6.10.2
```

### aqtinstall creates nested directories

aqtinstall sometimes creates `6.10.2/6.10.2/llvm-mingw_64` instead of
`6.10.2/llvm-mingw_64`. The build script handles this automatically. If
running manually, move the inner directory up:

```bash
mv 6.10.2/6.10.2/llvm-mingw_64 6.10.2/llvm-mingw_64
```

### OpenGL / linker errors during configure

The toolchain's `windows-cross-prereqs.cmake` creates fallback OpenGL targets
automatically via `CMAKE_PROJECT_INCLUDE`. If you see OpenGL-related errors,
make sure you're using the toolchain file and doing a clean configure:

```bash
rm -rf build-windows-x64
```

Then re-run the configure step.

## Limitations

- The resulting `.exe` is not code-signed (requires Windows SDK or third-party signing tools)
- Symbol verification is runtime-only — the Zig toolchain links against
  `-lopengl32` as an import library, so missing or mismatched symbols in
  `opengl32.dll` will only surface at runtime on Windows
- Only x86_64 is targeted. For ARM64 Windows, the wrapper scripts and
  toolchain file would need to be duplicated with a different Zig target triple
