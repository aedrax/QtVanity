# Implementation Plan: Linux aarch64 (ARM64) Cross-Compilation

## Overview

Add cross-compilation support for targeting Linux aarch64 from a Linux x86_64 host. Create Zig compiler wrappers, a CMake toolchain file, a cross-compilation prerequisites file, an automated build script, update CMakeLists.txt for aarch64 architecture mapping, and write documentation. Follows the established pattern from the Windows x86_64 and macOS aarch64 cross-compilation setups.

## Tasks

- [x] 1. Create Zig compiler wrappers for Linux ARM64
  - [x] 1.1 Create `cmake/zig-aarch64-linux-cc`
    - Create an executable shell script that invokes `zig cc -target aarch64-linux-gnu`
    - Filter out `--target=*` and `-target <value>` flags from CMake
    - Filter out host system include paths: `-isystem /usr/include*`, `-I /usr/include*`, `-isystem /usr/local/include*`, `-I /usr/local/include*`, `-isystem /usr/lib/gcc/*`, `-I /usr/lib/gcc/*` (both space-separated and concatenated forms)
    - Forward all remaining arguments to zig in original order
    - Follow the pattern of `cmake/zig-x86_64-windows-cc` (host include filtering) rather than `cmake/zig-aarch64-macos-cc` (framework handling)
    - _Requirements: 1.1, 1.3, 1.4, 1.5_

  - [x] 1.2 Create `cmake/zig-aarch64-linux-cxx`
    - Create an executable shell script that invokes `zig c++ -target aarch64-linux-gnu`
    - Same flag and host include path filtering as the CC wrapper
    - _Requirements: 1.2, 1.3, 1.4, 1.5_

  - [x] 1.3 Write property test for argument filtering
    - **Property 1: Argument filtering preserves non-target, non-host-include arguments**
    - Create `tests/cross-compile/test_linux_arm64_arg_filtering.py`
    - Generate random argument lists containing `--target=<value>`, `-target <value>`, host system include paths (`-isystem /usr/include`, `-I /usr/local/include`, `-isystem /usr/lib/gcc/`), and normal compiler flags
    - Pass through the wrapper (with zig replaced by a mock that echoes args)
    - Verify target flags and host include paths are absent and all other args are present in original order
    - Minimum 100 iterations using Hypothesis
    - **Validates: Requirements 1.3, 1.4**

- [x] 2. Create CMake toolchain and prerequisites files
  - [x] 2.1 Create `cmake/linux-aarch64-zig-toolchain.cmake`
    - Set `CMAKE_SYSTEM_NAME` to `Linux` and `CMAKE_SYSTEM_PROCESSOR` to `aarch64`
    - Set `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` to the Zig wrapper scripts
    - Unset `CMAKE_C_COMPILER_TARGET` and `CMAKE_CXX_COMPILER_TARGET`
    - Set `CMAKE_CROSSCOMPILING` to `TRUE` and `CMAKE_TRY_COMPILE_TARGET_TYPE` to `STATIC_LIBRARY`
    - Set `CMAKE_FIND_ROOT_PATH_MODE_*` for cross-compilation search paths
    - Set `CMAKE_PROJECT_INCLUDE` to `linux-aarch64-cross-prereqs.cmake`
    - Follow the pattern of `cmake/windows-x86_64-zig-toolchain.cmake` but without Windows-specific settings (no `.exe` suffix, no `WIN32`, no DLL settings)
    - _Requirements: 2.1, 2.2, 2.3, 2.4, 2.5, 2.6_

  - [x] 2.2 Create `cmake/linux-aarch64-cross-prereqs.cmake`
    - Create `OpenGL::GL` imported INTERFACE target with `INTERFACE_LINK_LIBRARIES` set to `-lGL`
    - Create `WrapOpenGL::WrapOpenGL` imported INTERFACE target linking to `OpenGL::GL`
    - Create `Vulkan::Headers` imported INTERFACE target with empty include path
    - Create `WrapVulkanHeaders::WrapVulkanHeaders` imported INTERFACE target
    - Set `OPENGL_FOUND`, `OpenGL_FOUND`, `WrapOpenGL_FOUND`, `Vulkan_FOUND`, `WrapVulkanHeaders_FOUND` cache variables
    - Guard all target creation with `if(NOT TARGET ...)` to avoid duplicates
    - Mirror `cmake/windows-cross-prereqs.cmake` but use `-lGL` instead of `-lopengl32`
    - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5_

- [x] 3. Checkpoint - Verify toolchain, prereqs, and wrappers
  - Ensure all CMake files are syntactically valid and wrapper scripts are consistent with the design. Ask the user if questions arise.

- [x] 4. Create build script and update CMakeLists.txt
  - [x] 4.1 Create `build-linux-arm64.sh`
    - Add preflight checks for `zig` and `cmake` with descriptive error messages
    - Download Linux ARM64 Qt target libraries via aqtinstall `linux_arm64` if missing
    - Download Linux x86_64 Qt host tools via aqtinstall `linux_gcc_64` if missing
    - Handle aqtinstall nested directory structure (`6.10.2/6.10.2/gcc_arm64`)
    - Add Qt directory name variant resolution checking `gcc_arm64` and `linux_gcc_arm64`
    - Print resolved Linux ARM64 Qt path to stdout
    - Verify host `moc` exists at `libexec/moc` or `bin/moc`
    - Invoke CMake with toolchain file, `CMAKE_PREFIX_PATH` for ARM64 Qt, `QT_HOST_PATH` for x86_64 Qt
    - Build in Release mode with testing disabled
    - Output binary at `build-linux-arm64/QtVanity`
    - Follow the pattern of `build-windows-x64.sh`
    - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 4.10, 5.1, 5.2, 5.3_

  - [x] 4.2 Verify CMakeLists.txt aarch64 architecture mapping
    - Confirm the existing CMakeLists.txt already maps `aarch64|arm64` to `CPACK_DEBIAN_PACKAGE_ARCHITECTURE` of `arm64`
    - Confirm `NO_UNSUPPORTED_PLATFORM_ERROR` is present in `qt_generate_deploy_app_script`
    - If either is missing, add the necessary lines
    - _Requirements: 7.1, 7.2, 7.3_

  - [x] 4.3 Write property test for missing tool detection
    - **Property 2: Missing tool detection**
    - Create `tests/cross-compile/test_linux_arm64_missing_tool_detection.py`
    - For each tool in {zig, cmake, moc}, run the build script in an environment where that tool is absent
    - Verify non-zero exit and error message containing the missing tool name
    - Follow the pattern of `tests/cross-compile/test_missing_tool_detection.py`
    - **Validates: Requirements 4.1, 4.2, 4.6, 4.7**

  - [x] 4.4 Write property test for Qt directory name variant resolution
    - **Property 3: Qt directory name variant resolution**
    - Create `tests/cross-compile/test_linux_arm64_qt_dir_variant_resolution.py`
    - Create directory structures with each known variant name (`gcc_arm64`, `linux_gcc_arm64`)
    - Run the resolution logic and verify it finds the correct path containing `lib/cmake/Qt6`
    - Follow the pattern of `tests/cross-compile/test_qt_dir_variant_resolution.py`
    - **Validates: Requirements 5.1, 5.2, 5.3**

- [x] 5. Checkpoint - Verify build script and tests
  - Ensure the build script is syntactically valid, all path references are consistent, and CMakeLists.txt mappings are correct. Ask the user if questions arise.

- [x] 6. Create cross-compilation documentation
  - [x] 6.1 Create `CROSS_COMPILE_LINUX_ARM64.md`
    - Add prerequisites table with version requirements (Zig 0.15+, CMake 3.16+, aqtinstall 3.x)
    - Add quick-start section showing `./build-linux-arm64.sh`
    - Add architecture explanation: Zig as cross-compiler, dual Qt installations (host tools vs target libraries), CMake toolchain approach
    - Add file layout section listing toolchain file, Zig wrappers, prereqs file, build script
    - Add manual build steps as alternative to the build script
    - Add troubleshooting section covering: missing tools, path mismatches, host header contamination, linker errors
    - Add known limitations section (no runtime testing on x86_64 host, OpenGL/Vulkan resolved at runtime on target)
    - Mirror the structure and style of `CROSS_COMPILE_WINDOWS.md`
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5, 6.6, 6.7_

- [x] 7. Final checkpoint - Ensure all artifacts are complete
  - Ensure all tests pass, all files are consistent, and the documentation is complete. Ask the user if questions arise.

## Notes

- Tasks marked with `*` are optional and can be skipped for faster MVP
- The Zig wrappers follow the Windows wrapper pattern (host include filtering) rather than the macOS pattern (framework handling)
- The toolchain file is the simplest of the three since Linux-to-Linux cross-compilation shares the same OS and binary format
- The prereqs file mirrors `windows-cross-prereqs.cmake` with `-lGL` instead of `-lopengl32`
- The CMakeLists.txt already has the `aarch64|arm64` mapping for `CPACK_DEBIAN_PACKAGE_ARCHITECTURE` — task 4.2 verifies this
- Property tests use Hypothesis (Python), consistent with existing tests in `tests/cross-compile/`
- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation
