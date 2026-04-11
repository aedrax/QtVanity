# Implementation Plan: Windows x86_64 Cross-Compilation

## Overview

Complete the Windows cross-compilation pipeline by fixing the existing build script's path mismatch bug, adding `CMAKE_PROJECT_INCLUDE` to the toolchain file for OpenGL resolution, creating the OpenGL prereqs file, and writing documentation. The Zig wrappers already exist and are correct.

## Tasks

- [x] 1. Add OpenGL fallback for Windows cross-compilation
  - [x] 1.1 Create `cmake/windows-cross-prereqs.cmake`
    - Create an `OpenGL::GL` imported INTERFACE target with `INTERFACE_LINK_LIBRARIES` set to `-lopengl32`
    - Create a `WrapOpenGL::WrapOpenGL` imported INTERFACE target linking to `OpenGL::GL`
    - Set `OPENGL_FOUND`, `OpenGL_FOUND`, and `WrapOpenGL_FOUND` cache variables to `TRUE`
    - Mirror the structure of `cmake/macos-cross-prereqs.cmake` but use `-lopengl32` instead of `-framework OpenGL`
    - _Requirements: 7.1, 7.2_

  - [x] 1.2 Add `CMAKE_PROJECT_INCLUDE` to the toolchain file
    - Add `set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/windows-cross-prereqs.cmake")` to `cmake/windows-x86_64-zig-toolchain.cmake`
    - Place it after the existing cross-compilation settings
    - _Requirements: 2.7, 7.2_

- [x] 2. Checkpoint - Verify toolchain and prereqs
  - Ensure the toolchain file and prereqs file are syntactically valid CMake. Ask the user if questions arise.

- [x] 3. Fix build script path mismatch and add variant resolution
  - [x] 3.1 Fix `build-windows-x64.sh` path handling
    - Change `WIN_QT_DIR` default from `llvm_mingw` to `llvm-mingw_64` (the actual aqtinstall output name)
    - Add a path variant resolution block after Qt download that checks for both `llvm-mingw_64` and `llvm_mingw` directory names
    - Update the aqtinstall nested directory fix to handle `6.10.2/6.10.2/llvm-mingw_64`
    - Print the resolved Windows Qt path to stdout for diagnostics
    - _Requirements: 3.3, 3.5, 4.1, 4.2, 4.3_

  - [x] 3.2 Verify build script preflight and moc checks
    - Ensure `zig` and `cmake` preflight checks print descriptive error messages and exit non-zero
    - Ensure `moc` verification checks both `libexec/moc` and `bin/moc` paths
    - Ensure the script exits non-zero with a descriptive message if moc is not found
    - _Requirements: 3.1, 3.2, 3.6, 3.7_

- [x] 4. Checkpoint - Verify build script
  - Ensure the build script is syntactically valid and all path references are consistent. Ask the user if questions arise.

- [x] 5. Write property-based and unit tests
  - [x] 5.1 Write property test for target flag filtering
    - **Property 1: Target flag filtering preserves non-target arguments**
    - Generate random argument lists containing `--target=<value>`, `-target <value>`, and normal compiler flags
    - Pass through the wrapper (with zig replaced by a mock that echoes args)
    - Verify target flags are absent and all other args are present in original order
    - Minimum 100 iterations
    - **Validates: Requirements 1.3**

  - [x] 5.2 Write property test for missing tool detection
    - **Property 2: Missing tool detection**
    - For each tool in {zig, cmake, moc}, run the build script in an environment where that tool is absent
    - Verify non-zero exit and error message containing the missing tool name
    - **Validates: Requirements 3.1, 3.2, 3.6, 3.7**

  - [x] 5.3 Write property test for Qt directory name variant resolution
    - **Property 3: Qt directory name variant resolution**
    - Create directory structures with each known variant name (`llvm-mingw_64`, `llvm_mingw`)
    - Run the resolution logic and verify it finds the correct path containing `lib/cmake/Qt6`
    - **Validates: Requirements 4.1, 4.2, 4.3**

- [x] 6. Create cross-compilation documentation
  - [x] 6.1 Create `CROSS_COMPILE_WINDOWS.md`
    - Add prerequisites table with version requirements (Zig 0.15+, CMake 3.16+, aqtinstall 3.x)
    - Add quick-start section showing `./build-windows-x64.sh`
    - Add architecture explanation: Zig as cross-compiler, dual Qt installations, CMake toolchain approach
    - Add file layout section listing toolchain file, Zig wrappers, prereqs file, build script
    - Add manual build steps as alternative to the build script
    - Add troubleshooting section covering: missing tools, path mismatches, OpenGL/linker errors
    - Add known limitations section (no code signing, runtime-only symbol verification)
    - Mirror the structure and style of `CROSS_COMPILE_MACOS.md`
    - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5, 5.6, 5.7_

- [x] 7. Final checkpoint - Ensure all artifacts are complete
  - Ensure all tests pass, all files are consistent, and the documentation is complete. Ask the user if questions arise.

## Notes

- Tasks marked with `*` are optional and can be skipped for faster MVP
- The Zig wrappers (`cmake/zig-x86_64-windows-cc`, `cmake/zig-x86_64-windows-cxx`) already exist and are correct — no implementation tasks needed for them
- The toolchain file (`cmake/windows-x86_64-zig-toolchain.cmake`) exists but needs the `CMAKE_PROJECT_INCLUDE` addition
- The build script (`build-windows-x64.sh`) exists but has the `llvm_mingw` vs `llvm-mingw_64` path mismatch bug
- Property tests target shell script and CMake behavior, not C++ code
- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation
