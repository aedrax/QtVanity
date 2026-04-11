# Requirements Document

## Introduction

Add cross-compilation support for targeting x86_64 Windows from a Linux host, following the same pattern established by the existing macOS aarch64 cross-compilation setup. The project (QtVanity) is a Qt 6 C++ application that already cross-compiles for macOS using Zig as the compiler toolchain, dual Qt installations (host tools + target libraries), and CMake toolchain files. This feature extends that approach to produce Windows `.exe` binaries using Zig's `x86_64-windows-gnu` target and the Qt 6.10.2 `llvm-mingw` Windows distribution.

## Glossary

- **Build_Script**: The shell script (`build-windows-x64.sh`) that automates prerequisite installation, configuration, and building for the Windows cross-compile target
- **Toolchain_File**: The CMake toolchain file (`cmake/windows-x86_64-zig-toolchain.cmake`) that tells CMake how to cross-compile for Windows using Zig
- **Zig_Wrapper**: Shell scripts (`cmake/zig-x86_64-windows-cc`, `cmake/zig-x86_64-windows-cxx`) that adapt Zig's C/C++ compiler for use by CMake, filtering incompatible flags
- **Host_Tools**: Linux-native Qt 6.10.2 build tools (moc, rcc, uic) installed at `6.10.2/gcc_64` that run on the build machine
- **Target_Libraries**: Windows Qt 6.10.2 libraries installed via aqtinstall's `win64_llvm_mingw` archive at `6.10.2/llvm-mingw_64`
- **Cross_Compile_Documentation**: A markdown guide (`CROSS_COMPILE_WINDOWS.md`) explaining prerequisites, usage, architecture, and troubleshooting for the Windows cross-compile workflow
- **QtVanity**: The Qt 6 C++ desktop application being cross-compiled
- **aqtinstall**: A command-line tool for downloading Qt SDK components without the Qt installer

## Requirements

### Requirement 1: Zig Compiler Wrappers for Windows Target

**User Story:** As a developer, I want Zig compiler wrapper scripts that target x86_64 Windows, so that CMake can use Zig as a drop-in cross-compiler for producing Windows binaries.

#### Acceptance Criteria

1. THE Zig_Wrapper for C compilation SHALL invoke `zig cc` with target `x86_64-windows-gnu`
2. THE Zig_Wrapper for C++ compilation SHALL invoke `zig c++` with target `x86_64-windows-gnu`
3. WHEN CMake passes `--target=` or `-target` flags, THE Zig_Wrapper SHALL filter those flags out before forwarding remaining arguments to Zig
4. THE Zig_Wrapper scripts SHALL be executable shell scripts located at `cmake/zig-x86_64-windows-cc` and `cmake/zig-x86_64-windows-cxx`

### Requirement 2: CMake Toolchain File for Windows x86_64

**User Story:** As a developer, I want a CMake toolchain file that configures cross-compilation for Windows x86_64, so that CMake correctly targets Windows when building.

#### Acceptance Criteria

1. THE Toolchain_File SHALL set `CMAKE_SYSTEM_NAME` to `Windows` and `CMAKE_SYSTEM_PROCESSOR` to `AMD64`
2. THE Toolchain_File SHALL set `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` to the Zig_Wrapper scripts
3. THE Toolchain_File SHALL unset `CMAKE_C_COMPILER_TARGET` and `CMAKE_CXX_COMPILER_TARGET` to prevent CMake from injecting its own target flags
4. THE Toolchain_File SHALL set `CMAKE_EXECUTABLE_SUFFIX` to `.exe`
5. THE Toolchain_File SHALL set `CMAKE_CROSSCOMPILING` to `TRUE` and `CMAKE_TRY_COMPILE_TARGET_TYPE` to `STATIC_LIBRARY`
6. THE Toolchain_File SHALL configure Windows-specific library naming conventions (`.dll`, `.dll.a`, `.a` suffixes)
7. THE Toolchain_File SHALL configure `CMAKE_FIND_ROOT_PATH_MODE` so that target libraries and headers are found from the target sysroot while host programs are found from the host system

### Requirement 3: Automated Build Script

**User Story:** As a developer, I want a single build script that handles all prerequisites and builds QtVanity for Windows, so that I can cross-compile with one command.

#### Acceptance Criteria

1. WHEN invoked, THE Build_Script SHALL verify that `zig` and `cmake` are available in PATH
2. IF `zig` or `cmake` is not found, THEN THE Build_Script SHALL print a descriptive error message and exit with a non-zero status
3. WHEN the Windows Target_Libraries are not present at the expected path, THE Build_Script SHALL download them using aqtinstall with the `win64_llvm_mingw` archive identifier
4. WHEN the Linux Host_Tools are not present at the expected path, THE Build_Script SHALL download them using aqtinstall with the `linux_gcc_64` archive identifier
5. WHEN aqtinstall creates a nested directory structure (e.g., `6.10.2/6.10.2/llvm-mingw_64`), THE Build_Script SHALL relocate the inner directory to the expected path
6. THE Build_Script SHALL verify that the host `moc` tool exists and is executable before proceeding to configuration
7. IF the host `moc` tool is not found, THEN THE Build_Script SHALL print a descriptive error message and exit with a non-zero status
8. THE Build_Script SHALL invoke CMake with the Toolchain_File, the Windows Target_Libraries as `CMAKE_PREFIX_PATH`, and the Linux Host_Tools as `QT_HOST_PATH`
9. THE Build_Script SHALL build the project in Release mode with testing disabled
10. THE Build_Script SHALL produce a Windows executable at `build-windows-x64/QtVanity.exe`

### Requirement 4: Qt Installation Path Consistency

**User Story:** As a developer, I want the build script to reference the correct Qt installation directory name, so that the build does not fail due to path mismatches.

#### Acceptance Criteria

1. THE Build_Script SHALL reference the Windows Target_Libraries directory using the name that aqtinstall actually produces for the `win64_llvm_mingw` archive
2. WHEN the aqtinstall output directory name differs from the expected path (e.g., `llvm-mingw_64` vs `llvm_mingw`), THE Build_Script SHALL handle the discrepancy by checking for known directory name variants
3. THE Build_Script SHALL print the resolved Windows Qt path to stdout for diagnostic purposes

### Requirement 5: Cross-Compilation Documentation

**User Story:** As a developer, I want a documentation file explaining the Windows cross-compilation process, so that I can understand prerequisites, usage, architecture, and troubleshooting steps.

#### Acceptance Criteria

1. THE Cross_Compile_Documentation SHALL list all prerequisites with version requirements (Zig, CMake, aqtinstall)
2. THE Cross_Compile_Documentation SHALL provide a quick-start section showing how to run the Build_Script
3. THE Cross_Compile_Documentation SHALL explain the architecture: Zig as cross-compiler, dual Qt installations (host tools vs target libraries), and the CMake toolchain approach
4. THE Cross_Compile_Documentation SHALL describe the file layout of all cross-compilation artifacts (toolchain file, Zig wrappers, build script)
5. THE Cross_Compile_Documentation SHALL include manual build steps as an alternative to the Build_Script
6. THE Cross_Compile_Documentation SHALL include a troubleshooting section covering common failure modes (missing tools, path issues, linker errors)
7. THE Cross_Compile_Documentation SHALL document known limitations of the cross-compiled binary (e.g., no code signing, runtime-only symbol verification)

### Requirement 6: Windows-Specific CMake Compatibility

**User Story:** As a developer, I want the cross-compilation to work with the existing CMakeLists.txt Windows configuration, so that the cross-compiled build produces a correctly configured Windows executable.

#### Acceptance Criteria

1. WHEN cross-compiling for Windows, THE Toolchain_File SHALL ensure that `WIN32` is set to `TRUE` so that the existing CMakeLists.txt Windows-specific sections activate correctly
2. WHEN cross-compiling for Windows, THE QtVanity target SHALL have the `WIN32_EXECUTABLE` property set to `TRUE` by the existing CMakeLists.txt
3. WHEN Qt 6 deployment script generation is invoked during cross-compilation, THE build system SHALL handle the `NO_UNSUPPORTED_PLATFORM_ERROR` flag gracefully without failing the configure step

### Requirement 7: OpenGL Dependency Resolution for Windows Cross-Compilation

**User Story:** As a developer, I want OpenGL dependencies to resolve correctly during Windows cross-compilation, so that Qt6Gui and related modules link successfully.

#### Acceptance Criteria

1. WHEN cross-compiling for Windows, THE build system SHALL resolve the OpenGL dependency required by Qt6Gui without requiring a native Windows OpenGL SDK on the Linux host
2. IF the standard `FindOpenGL` module fails to locate OpenGL during cross-compilation, THEN THE build system SHALL provide a fallback mechanism (e.g., a custom find module or toolchain-level target definition) that satisfies Qt's `WrapOpenGL` dependency
