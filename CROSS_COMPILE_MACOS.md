# Cross-Compiling QtVanity for macOS (arm64) from Linux

This guide explains how to cross-compile QtVanity for Apple Silicon Macs
from a Linux host using [Zig](https://ziglang.org/) as the C/C++ cross-compiler.

## Prerequisites

| Tool | Version | Install |
|------|---------|---------|
| Zig | 0.15+ | [ziglang.org/download](https://ziglang.org/download/) or `zvm install 0.15.2` |
| CMake | 3.16+ | `apt install cmake` |
| aqtinstall | 3.x | `pipx install aqtinstall` |

## Quick Start

```bash
./build-macos-arm64.sh
```

The script handles everything automatically:
1. Downloads macOS Qt 6.10.2 libraries (if missing)
2. Downloads Linux Qt 6.10.2 host tools (if missing)
3. Generates macOS SDK framework stubs (if missing)
4. Configures and builds the project

The output binary lands at `build-macos-arm64/QtVanity.app/Contents/MacOS/QtVanity`.

## How It Works

### The Problem

Cross-compiling a Qt C++ app for macOS from Linux requires solving three issues:

1. **Compiler**: Need a C/C++ compiler that can emit Mach-O arm64 binaries
2. **Qt host tools**: `moc`, `rcc`, and `uic` must run on the build host (Linux),
   but the macOS Qt ships macOS-native binaries
3. **macOS system frameworks**: Qt links against AppKit, Metal, IOKit, etc. which
   don't exist on Linux

### The Solution

**Zig as cross-compiler** — Zig bundles a Clang-based C/C++ compiler that can
target macOS out of the box. Wrapper scripts (`cmake/zig-aarch64-macos-cc` and
`cmake/zig-aarch64-macos-cxx`) adapt zig for CMake by:
- Fixing target triple format (`aarch64-macos` vs CMake's `aarch64-apple-darwin`)
- Converting framework binary paths to `-framework` linker flags
- Adding the SDK stubs framework search path

**Dual Qt installations** — Two copies of Qt 6.10.2 are used:
- `6.10.2/macos/` — macOS Qt (headers, frameworks, cmake configs) for the target
- `6.10.2/gcc_64/` — Linux Qt (moc, rcc, uic) for the host

CMake's `QT_HOST_PATH` mechanism tells Qt's build system to use Linux tools
while linking against macOS libraries.

**TBD framework stubs** — Minimal `.tbd` (text-based dylib) files in
`cmake/macos-sdk-stubs/` satisfy zig's linker for macOS system frameworks.
These are just metadata files that say "this framework exists at this install
path" — the actual framework binaries are resolved at runtime on macOS.

## File Layout

```
cmake/
├── macos-aarch64-zig-toolchain.cmake  # CMake toolchain file
├── macos-cross-prereqs.cmake          # Fixes @rpath + OpenGL for cross-compile
├── zig-aarch64-macos-cc               # Zig C compiler wrapper
├── zig-aarch64-macos-cxx              # Zig C++ compiler wrapper
└── macos-sdk-stubs/
    ├── create-stubs.sh                # Generates .tbd framework stubs
    ├── AppKit.framework/              # System framework stubs
    ├── Metal.framework/
    ├── IOKit.framework/
    └── ...
```

## Manual Build Steps

If you prefer not to use the build script:

```bash
# 1. Install macOS Qt (target libraries)
aqt install-qt mac desktop 6.10.2 clang_64 -O 6.10.2

# 2. Install Linux Qt (host tools: moc/rcc/uic)
aqt install-qt linux desktop 6.10.2 linux_gcc_64 -O 6.10.2

# 3. Generate macOS SDK framework stubs
bash cmake/macos-sdk-stubs/create-stubs.sh

# 4. Configure
cmake -B build-macos-arm64 \
    -DCMAKE_TOOLCHAIN_FILE=cmake/macos-aarch64-zig-toolchain.cmake \
    -DCMAKE_PREFIX_PATH="$(pwd)/6.10.2/macos" \
    -DQT_HOST_PATH="$(pwd)/6.10.2/gcc_64" \
    -DQT_HOST_PATH_CMAKE_DIR="$(pwd)/6.10.2/gcc_64/lib/cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DQT_VERSION_MAJOR=6 \
    -G "Unix Makefiles"

# 5. Build
cmake --build build-macos-arm64 --parallel $(nproc)
```

## Troubleshooting

### `unable to find framework 'Foo'`

A macOS system framework is missing from the stubs. Add it to
`cmake/macos-sdk-stubs/create-stubs.sh` and re-run the script:

```bash
bash cmake/macos-sdk-stubs/create-stubs.sh
```

### `@rpath` errors during cmake configure

The toolchain's `macos-cross-prereqs.cmake` sets the required Darwin rpath
flags. Make sure you're using the toolchain file and doing a clean configure
(`rm -rf build-macos-arm64`).

### `moc: No such file or directory`

The Linux Qt host tools aren't installed. Run:

```bash
aqt install-qt linux desktop 6.10.2 linux_gcc_64 -O 6.10.2
```

### aqtinstall creates nested directories

aqtinstall sometimes creates `6.10.2/6.10.2/gcc_64` instead of `6.10.2/gcc_64`.
The build script handles this automatically. If running manually, just move
the inner directory up.

## Limitations

- The resulting binary is not code-signed (requires a Mac or Apple Developer tools)
- The `.tbd` stubs are minimal — they satisfy the linker but don't provide
  symbol-level verification. Linking errors from missing symbols will only
  surface at runtime on macOS.
- Only arm64 (Apple Silicon) is targeted. For x86_64, duplicate the wrapper
  scripts with `-target x86_64-macos`.
