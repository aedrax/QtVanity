# Cross-Compiling QtVanity for Linux ARM64 (aarch64) from Linux x86_64

This guide explains how to cross-compile QtVanity for Linux ARM64 (aarch64)
from a Linux x86_64 host using [Zig](https://ziglang.org/) as the C/C++ cross-compiler.

## Prerequisites

| Tool | Version | Install |
|------|---------|---------|
| Zig | 0.15+ | [ziglang.org/download](https://ziglang.org/download/) or `zvm install 0.15.2` |
| CMake | 3.16+ | `apt install cmake` |
| aqtinstall | 3.x | `pipx install aqtinstall` |
| g++-aarch64-linux-gnu | any | `apt install g++-aarch64-linux-gnu` |

> The `g++-aarch64-linux-gnu` package provides aarch64 libstdc++ headers and
> libraries. This is required because the Qt ARM64 libraries are built with
> GCC/libstdc++, while Zig bundles libc++ which has an incompatible ABI.

## Quick Start

```bash
./build-linux-arm64.sh
```

The script handles everything automatically:
1. Downloads Linux ARM64 Qt 6.10.2 libraries (if missing)
2. Downloads Linux x86_64 Qt 6.10.2 host tools (if missing)
3. Configures and builds the project

The output binary lands at `build-linux-arm64/QtVanity`.

## How It Works

### The Problem

Cross-compiling a Qt C++ app for Linux ARM64 from Linux x86_64 requires
solving three issues:

1. **Compiler**: Need a C/C++ compiler that can emit aarch64 ELF binaries
2. **Qt host tools**: `moc`, `rcc`, and `uic` must run on the build host (x86_64),
   but the ARM64 Qt ships ARM64-native binaries
3. **OpenGL**: Qt6Gui links against OpenGL, which isn't available for the
   aarch64 target on the x86_64 host

### The Solution

**Zig as cross-compiler** — Zig bundles a Clang-based C/C++ compiler that can
target `aarch64-linux-gnu` out of the box, eliminating the need for a separate
cross-compilation toolchain (e.g., `aarch64-linux-gnu-gcc`). Wrapper scripts
(`cmake/zig-aarch64-linux-cc` and `cmake/zig-aarch64-linux-cxx`) adapt Zig for
CMake by filtering out `--target`/`-target` flags that CMake injects (Zig
handles targeting internally) and stripping host x86_64 system include paths
to prevent header contamination.

**Dual Qt installations** — Two copies of Qt 6.10.2 are used:
- `6.10.2/gcc_arm64/` — Linux ARM64 Qt (headers, shared libs, cmake configs) for the target
- `6.10.2/gcc_64/` — Linux x86_64 Qt (moc, rcc, uic) for the host

CMake's `QT_HOST_PATH` mechanism tells Qt's build system to use x86_64 tools
while linking against ARM64 libraries.

**OpenGL fallback** — A prereqs file (`cmake/linux-aarch64-cross-prereqs.cmake`)
creates imported `OpenGL::GL` and `WrapOpenGL::WrapOpenGL` targets that link
against `-lGL`, satisfying Qt6Gui's OpenGL dependency without requiring an
aarch64 OpenGL installation on the x86_64 host.

**Linux-to-Linux advantage** — Unlike the Windows and macOS cross-compile
targets, Linux-to-Linux cross-compilation shares the same OS and binary format
(ELF). This means `CMAKE_SYSTEM_NAME` stays `Linux`, so all existing
CMakeLists.txt Linux-specific sections (GNUInstallDirs, CPack DEB/TGZ, desktop
file, icon installation) activate naturally without modification.

## File Layout

```
cmake/
├── linux-aarch64-zig-toolchain.cmake    # CMake toolchain file
├── linux-aarch64-cross-prereqs.cmake    # OpenGL + Vulkan fallback for cross-compile
├── zig-aarch64-linux-cc                 # Zig C compiler wrapper
└── zig-aarch64-linux-cxx               # Zig C++ compiler wrapper

build-linux-arm64.sh                     # Automated build script
CROSS_COMPILE_LINUX_ARM64.md             # This file
```

## Manual Build Steps

If you prefer not to use the build script:

```bash
# 1. Install Linux ARM64 Qt (target libraries)
aqt install-qt linux desktop 6.10.2 linux_arm64 -O 6.10.2

# 2. Install Linux Qt (host tools: moc/rcc/uic)
aqt install-qt linux desktop 6.10.2 linux_gcc_64 -O 6.10.2

# 3. Configure
cmake -B build-linux-arm64 \
    -DCMAKE_TOOLCHAIN_FILE=cmake/linux-aarch64-zig-toolchain.cmake \
    -DCMAKE_PREFIX_PATH="$(pwd)/6.10.2/gcc_arm64" \
    -DQT_HOST_PATH="$(pwd)/6.10.2/gcc_64" \
    -DQT_HOST_PATH_CMAKE_DIR="$(pwd)/6.10.2/gcc_64/lib/cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DQT_VERSION_MAJOR=6 \
    -G "Unix Makefiles"

# 4. Build
cmake --build build-linux-arm64 --parallel $(nproc)
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

### `ERROR: Cannot find Linux ARM64 Qt libraries`

The build script checks for both `gcc_arm64` (current aqtinstall output)
and `linux_gcc_arm64` (potential variant name) under `6.10.2/`. If neither
is found, re-run the aqtinstall command manually:

```bash
aqt install-qt linux desktop 6.10.2 linux_arm64 -O 6.10.2
```

Then check which directory name was created and verify it contains
`lib/cmake/Qt6`.

### `ERROR: Cannot find moc in Linux Qt installation`

The Linux Qt host tools aren't installed. Run:

```bash
aqt install-qt linux desktop 6.10.2 linux_gcc_64 -O 6.10.2
```

### aqtinstall creates nested directories

aqtinstall sometimes creates `6.10.2/6.10.2/gcc_arm64` instead of
`6.10.2/gcc_arm64`. The build script handles this automatically. If
running manually, move the inner directory up:

```bash
mv 6.10.2/6.10.2/gcc_arm64 6.10.2/gcc_arm64
```

### OpenGL / linker errors during configure

The toolchain's `linux-aarch64-cross-prereqs.cmake` creates fallback OpenGL
and Vulkan targets automatically via `CMAKE_PROJECT_INCLUDE`. If you see
OpenGL-related errors, make sure you're using the toolchain file and doing
a clean configure:

```bash
rm -rf build-linux-arm64
```

Then re-run the configure step.

### Host header contamination

If you see errors about incompatible headers or architecture mismatches,
the Zig wrappers may not be filtering all host include paths. The wrappers
strip `-isystem /usr/include*`, `-I /usr/include*`, `-isystem /usr/local/include*`,
`-I /usr/local/include*`, and `-isystem /usr/lib/gcc/*` / `-I /usr/lib/gcc/*`.
If CMake is injecting other host paths, you may need to extend the filtering
in `cmake/zig-aarch64-linux-cc` and `cmake/zig-aarch64-linux-cxx`.

## Limitations

- No runtime testing on x86_64 host — the resulting binary is an aarch64 ELF
  executable that requires ARM64 hardware or QEMU to run
- OpenGL is resolved at runtime on the target — `-lGL` links against `libGL.so`
  which must be present on the target ARM64 system
- Vulkan headers are stubbed — actual Vulkan support depends on the target
  system having Vulkan libraries and drivers installed
