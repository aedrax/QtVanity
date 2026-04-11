# CMake toolchain file for cross-compiling to Linux aarch64 using Zig
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/linux-aarch64-zig-toolchain.cmake \
#         -DQT_HOST_PATH=<path-to-linux-x86_64-qt-6.10.2> \
#         -DCMAKE_PREFIX_PATH=<path-to-linux-arm64-qt-6.10.2> \
#         -B build-linux-arm64

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Use zig wrapper scripts as compilers (they handle target internally)
set(CMAKE_C_COMPILER "${CMAKE_CURRENT_LIST_DIR}/zig-aarch64-linux-cc")
set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_LIST_DIR}/zig-aarch64-linux-cxx")

# Don't let CMake add its own --target flags
unset(CMAKE_C_COMPILER_TARGET)
unset(CMAKE_CXX_COMPILER_TARGET)

# Cross-compiling: don't try to run target executables
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Pre-create OpenGL + Vulkan targets for Linux ARM64 cross-compilation
# CMAKE_PROJECT_INCLUDE runs after platform detection, before project CMakeLists
set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/linux-aarch64-cross-prereqs.cmake")

# Search paths: find target libs/headers, but use host programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
