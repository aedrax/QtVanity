# CMake toolchain file for cross-compiling to macOS aarch64 using Zig
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/macos-aarch64-zig-toolchain.cmake \
#         -DQT_HOST_PATH=<path-to-linux-qt-6.10.2> \
#         -DCMAKE_PREFIX_PATH=<path-to-macos-qt-6.10.2> \
#         -B build-macos-arm64

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

# Use zig wrapper scripts as compilers (they handle target internally)
set(CMAKE_C_COMPILER "${CMAKE_CURRENT_LIST_DIR}/zig-aarch64-macos-cc")
set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_LIST_DIR}/zig-aarch64-macos-cxx")

# Don't let CMake add its own --target or -arch flags
unset(CMAKE_C_COMPILER_TARGET)
unset(CMAKE_CXX_COMPILER_TARGET)
set(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "13" CACHE STRING "Minimum macOS deployment target")

# Cross-compiling: don't try to run target executables
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# macOS framework support
set(CMAKE_FIND_FRAMEWORK FIRST)
set(CMAKE_FIND_APPBUNDLE FIRST)

# Linker flags for macOS
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-rpath,@executable_path/../Frameworks")

# Pre-create macOS framework targets and fix platform flags
# CMAKE_PROJECT_INCLUDE runs after platform detection, before project CMakeLists
set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/macos-cross-prereqs.cmake")

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
