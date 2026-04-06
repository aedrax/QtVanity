# CMake toolchain file for cross-compiling to Windows x86_64 using Zig
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/windows-x86_64-zig-toolchain.cmake \
#         -DQT_HOST_PATH=<path-to-linux-qt-6.10.2> \
#         -DCMAKE_PREFIX_PATH=<path-to-windows-qt-6.10.2> \
#         -B build-windows-x64

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# Use zig wrapper scripts as compilers
set(CMAKE_C_COMPILER "${CMAKE_CURRENT_LIST_DIR}/zig-x86_64-windows-cc")
set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_LIST_DIR}/zig-x86_64-windows-cxx")

# Don't let CMake add its own --target flags
unset(CMAKE_C_COMPILER_TARGET)
unset(CMAKE_CXX_COMPILER_TARGET)

# Windows executable suffix
set(CMAKE_EXECUTABLE_SUFFIX ".exe")

# Cross-compiling: don't try to run target executables
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Windows-specific settings
set(WIN32 TRUE)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")
set(CMAKE_IMPORT_LIBRARY_SUFFIX ".dll.a")
set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

# Pre-create OpenGL targets for Windows cross-compilation
# CMAKE_PROJECT_INCLUDE runs after platform detection, before project CMakeLists
set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/windows-cross-prereqs.cmake")

# Search paths: find target libs/headers, but use host programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
