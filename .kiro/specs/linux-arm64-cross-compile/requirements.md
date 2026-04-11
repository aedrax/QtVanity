# Requirements Document

## Introduction

Add cross-compilation support for targeting Linux aarch64 (ARM64) from a Linux x86_64 host, following the established pattern from the existing macOS aarch64 and Windows x86_64 cross-compilation setups. The project (QtVanity) is a Qt 6 C++ application that already cross-compiles for macOS and Windows using Zig as the compiler toolchain, dual Qt installations (host tools + target libraries), and CMake toolchain files. This feature extends that approach to produce Linux ARM64 ELF binaries using Zig's `aarch64-linux-gnu` target and a Qt 6.10.2 ARM64 Linux distribution.

Unlike the Windows and macOS targets, Linux-to-Linux cross-compilation shares the same OS and binary format (ELF), but differs in CPU architecture. The host Qt tools (moc, rcc, uic) from `6.10.2/gcc_64` run natively on the x86_64 build machine, while the target Qt libraries must be the `linux_arm64` variant providing aarch64 shared objects and headers.

## Glossary

- **Build_Script**: The shell script (`build-linux-arm64.sh`) that automates prerequisite installation, configuration, and building for the Linux ARM64 cross-compile target
- **Toolchain_File**: The CMake toolchain file (`cmake/linux-aarch64-zig-toolchain.cmake`) that tells CMake how to cross-compile for Linux ARM64 using Zig
- **Zig_Wrapper**: Shell scripts (`cmake/zig-aarch64-linux-cc`, `cmake/zig-aarch64-linux-cxx`) that adapt Zig's C/C++ compiler for use by CMake, filtering incompatible flags and host system include paths
- **Host_Tools**: Linux-native x86_64 Qt 6.10.2 build tools (moc, rcc, uic) installed at `6.10.2/gcc_64` that run on the build machine
- **Target_Libraries**: Linux ARM64 Qt 6.10.2 libraries installed via aqtinstall's `linux_arm64` archive at `6.10.2/gcc_arm64`
- **Cross_Compile_Documentation**: A markdown guide (`CROSS_COMPILE_LINUX_ARM64.md`) explaining prerequisites, usage, architecture, and troubleshooting for the Linux ARM64 cross-compile workflow
- **Cross_Prereqs_File**: A CMake script (`cmake/linux-aarch64-cross-prereqs.cmake`) included via `CMAKE_PROJECT_INCLUDE` that pre-creates imported targets (OpenGL, Vulkan headers) to avoid host system path contamination during cross-compilation
- **QtVanity**: The Qt 6 C++ desktop application being cross-compiled
- **aqtinstall**: A command-line tool for downloading Qt SDK components without the Qt installer

## Requirements

### Requirement 1: Zig Compiler Wrappers for Linux ARM64 Target

**User Story:** As a developer, I want Zig compiler wrapper scripts that target aarch64 Linux, so that CMake can use Zig as a drop-in cross-compiler for producing Linux ARM64 binaries.

#### Acceptance Criteria

1. THE Zig_Wrapper for C compilation SHALL invoke `zig cc` with target `aarch64-linux-gnu`
2. THE Zig_Wrapper for C++ compilation SHALL invoke `zig c++` with target `aarch64-linux-gnu`
3. WHEN CMake passes `--target=` or `-target` flags, THE Zig_Wrapper SHALL filter those flags out before forwarding remaining arguments to Zig
4. WHEN CMake passes host system include paths (`-isystem /usr/include`, `-I /usr/include`, `-isystem /usr/local/include`, `-I /usr/lib/gcc/`), THE Zig_Wrapper SHALL filter those paths out to prevent host x86_64 headers from contaminating the aarch64 build
5. THE Zig_Wrapper scripts SHALL be executable shell scripts located at `cmake/zig-aarch64-linux-cc` and `cmake/zig-aarch64-linux-cxx`

### Requirement 2: CMake Toolchain File for Linux aarch64

**User Story:** As a developer, I want a CMake toolchain file that configures cross-compilation for Linux aarch64, so that CMake correctly targets ARM64 Linux when building.

#### Acceptance Criteria

1. THE Toolchain_File SHALL set `CMAKE_SYSTEM_NAME` to `Linux` and `CMAKE_SYSTEM_PROCESSOR` to `aarch64`
2. THE Toolchain_File SHALL set `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` to the Zig_Wrapper scripts
3. THE Toolchain_File SHALL unset `CMAKE_C_COMPILER_TARGET` and `CMAKE_CXX_COMPILER_TARGET` to prevent CMake from injecting its own target flags
4. THE Toolchain_File SHALL set `CMAKE_CROSSCOMPILING` to `TRUE` and `CMAKE_TRY_COMPILE_TARGET_TYPE` to `STATIC_LIBRARY`
5. THE Toolchain_File SHALL configure `CMAKE_FIND_ROOT_PATH_MODE` so that target libraries and headers are found from the target sysroot while host programs are found from the host system
6. THE Toolchain_File SHALL include the Cross_Prereqs_File via `CMAKE_PROJECT_INCLUDE` to pre-create OpenGL and Vulkan header targets for cross-compilation

### Requirement 3: Cross-Compilation Prerequisites File

**User Story:** As a developer, I want OpenGL and Vulkan header dependencies to resolve correctly during Linux ARM64 cross-compilation, so that Qt6Gui and related modules link and configure successfully without pulling in host x86_64 system paths.

#### Acceptance Criteria

1. WHEN cross-compiling for Linux ARM64, THE Cross_Prereqs_File SHALL create an imported `OpenGL::GL` target that links against `-lGL`
2. WHEN cross-compiling for Linux ARM64, THE Cross_Prereqs_File SHALL create an imported `WrapOpenGL::WrapOpenGL` target that depends on `OpenGL::GL`
3. WHEN cross-compiling for Linux ARM64, THE Cross_Prereqs_File SHALL create an imported `Vulkan::Headers` target with an empty include path to prevent CMake from searching host system directories
4. WHEN cross-compiling for Linux ARM64, THE Cross_Prereqs_File SHALL create an imported `WrapVulkanHeaders::WrapVulkanHeaders` target
5. IF any of the imported targets already exist, THEN THE Cross_Prereqs_File SHALL skip creation of those targets to avoid duplicate target errors

### Requirement 4: Automated Build Script

**User Story:** As a developer, I want a single build script that handles all prerequisites and builds QtVanity for Linux ARM64, so that I can cross-compile with one command.

#### Acceptance Criteria

1. WHEN invoked, THE Build_Script SHALL verify that `zig` and `cmake` are available in PATH
2. IF `zig` or `cmake` is not found, THEN THE Build_Script SHALL print a descriptive error message and exit with a non-zero status
3. WHEN the Linux ARM64 Target_Libraries are not present at the expected path, THE Build_Script SHALL download them using aqtinstall with the `linux_arm64` archive identifier
4. WHEN the Linux x86_64 Host_Tools are not present at the expected path, THE Build_Script SHALL download them using aqtinstall with the `linux_gcc_64` archive identifier
5. WHEN aqtinstall creates a nested directory structure (e.g., `6.10.2/6.10.2/gcc_arm64`), THE Build_Script SHALL relocate the inner directory to the expected path
6. THE Build_Script SHALL verify that the host `moc` tool exists and is executable before proceeding to configuration
7. IF the host `moc` tool is not found, THEN THE Build_Script SHALL print a descriptive error message and exit with a non-zero status
8. THE Build_Script SHALL invoke CMake with the Toolchain_File, the Linux ARM64 Target_Libraries as `CMAKE_PREFIX_PATH`, and the Linux x86_64 Host_Tools as `QT_HOST_PATH`
9. THE Build_Script SHALL build the project in Release mode with testing disabled
10. THE Build_Script SHALL produce a Linux ARM64 executable at `build-linux-arm64/QtVanity`

### Requirement 5: Qt Installation Path Consistency

**User Story:** As a developer, I want the build script to reference the correct Qt installation directory name, so that the build does not fail due to path mismatches.

#### Acceptance Criteria

1. THE Build_Script SHALL reference the Linux ARM64 Target_Libraries directory using the name that aqtinstall actually produces for the `linux_arm64` archive
2. WHEN the aqtinstall output directory name differs from the expected path (e.g., `gcc_arm64` vs `linux_gcc_arm64`), THE Build_Script SHALL handle the discrepancy by checking for known directory name variants
3. THE Build_Script SHALL print the resolved Linux ARM64 Qt path to stdout for diagnostic purposes

### Requirement 6: Cross-Compilation Documentation

**User Story:** As a developer, I want a documentation file explaining the Linux ARM64 cross-compilation process, so that I can understand prerequisites, usage, architecture, and troubleshooting steps.

#### Acceptance Criteria

1. THE Cross_Compile_Documentation SHALL list all prerequisites with version requirements (Zig, CMake, aqtinstall)
2. THE Cross_Compile_Documentation SHALL provide a quick-start section showing how to run the Build_Script
3. THE Cross_Compile_Documentation SHALL explain the architecture: Zig as cross-compiler, dual Qt installations (host tools vs target libraries), and the CMake toolchain approach
4. THE Cross_Compile_Documentation SHALL describe the file layout of all cross-compilation artifacts (toolchain file, Zig wrappers, prereqs file, build script)
5. THE Cross_Compile_Documentation SHALL include manual build steps as an alternative to the Build_Script
6. THE Cross_Compile_Documentation SHALL include a troubleshooting section covering common failure modes (missing tools, path issues, linker errors, host header contamination)
7. THE Cross_Compile_Documentation SHALL document known limitations of the cross-compiled binary (e.g., no runtime testing on x86_64 host, OpenGL resolved at runtime on target)

### Requirement 7: Linux-Specific CMake Compatibility

**User Story:** As a developer, I want the cross-compilation to work with the existing CMakeLists.txt Linux configuration, so that the cross-compiled build produces a correctly configured Linux ARM64 executable.

#### Acceptance Criteria

1. WHEN cross-compiling for Linux ARM64, THE Toolchain_File SHALL ensure that `CMAKE_SYSTEM_NAME` is `Linux` so that the existing CMakeLists.txt Linux-specific sections (GNUInstallDirs, CPack DEB/TGZ, desktop file, icon installation) activate correctly
2. WHEN cross-compiling for Linux ARM64, THE build system SHALL set `CPACK_DEBIAN_PACKAGE_ARCHITECTURE` to `arm64` based on the `CMAKE_SYSTEM_PROCESSOR` value of `aarch64`
3. WHEN Qt 6 deployment script generation is invoked during cross-compilation, THE build system SHALL handle the `NO_UNSUPPORTED_PLATFORM_ERROR` flag gracefully without failing the configure step
