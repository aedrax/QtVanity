# Pre-create imported targets for Linux ARM64 cross-compilation from
# Linux x86_64 using Zig. Included via CMAKE_PROJECT_INCLUDE so it runs
# after CMake's platform detection but before the project's find_package
# calls.

# ── Create aarch64 libGL stub if missing ─────────────────────────────
# Zig's linker needs an actual .so to resolve -lGL, but the host only
# has x86_64 libGL.  We generate a minimal empty aarch64 shared library
# that satisfies the linker; the real libGL.so is resolved at runtime
# on the target ARM64 system.
set(_GL_STUB_DIR "${CMAKE_CURRENT_LIST_DIR}/linux-aarch64-stubs")
set(_GL_STUB_LIB "${_GL_STUB_DIR}/libGL.so")
if(NOT EXISTS "${_GL_STUB_LIB}")
    file(MAKE_DIRECTORY "${_GL_STUB_DIR}")
    # Create a trivial C file and compile it into an aarch64 shared lib
    file(WRITE "${_GL_STUB_DIR}/_gl_stub.c" "void __gl_stub(void) {}\n")
    execute_process(
        COMMAND "${CMAKE_C_COMPILER}" -shared -o "${_GL_STUB_LIB}"
                "${_GL_STUB_DIR}/_gl_stub.c"
        RESULT_VARIABLE _gl_stub_result
    )
    if(NOT _gl_stub_result EQUAL 0)
        message(WARNING "Failed to create libGL.so stub for cross-compilation")
    endif()
endif()

# ── OpenGL shared library stub ───────────────────────────────────────
# Qt6Gui depends on WrapOpenGL. Point at our aarch64 stub so the linker
# is satisfied; the real libGL.so is resolved at runtime on the target.
if(NOT TARGET OpenGL::GL)
    add_library(OpenGL::GL INTERFACE IMPORTED)
    set_target_properties(OpenGL::GL PROPERTIES
        INTERFACE_LINK_LIBRARIES "-L${_GL_STUB_DIR};-lGL"
    )
    set(OPENGL_FOUND TRUE CACHE BOOL "" FORCE)
    set(OpenGL_FOUND TRUE CACHE BOOL "" FORCE)
endif()

if(NOT TARGET WrapOpenGL::WrapOpenGL)
    add_library(WrapOpenGL::WrapOpenGL INTERFACE IMPORTED)
    target_link_libraries(WrapOpenGL::WrapOpenGL INTERFACE OpenGL::GL)
    set(WrapOpenGL_FOUND TRUE CACHE BOOL "" FORCE)
endif()

# ── Vulkan header stub ───────────────────────────────────────────────
# Qt6Gui's WrapVulkanHeaders dependency triggers find_package(Vulkan).
# On the Linux x86_64 host this finds /usr/include, injecting host
# system headers (-isystem /usr/include) that conflict with the ARM64
# target build.  Pre-create the targets with an empty include path so
# CMake never searches the host filesystem.
if(NOT TARGET Vulkan::Headers)
    add_library(Vulkan::Headers INTERFACE IMPORTED)
    set(Vulkan_FOUND TRUE CACHE BOOL "" FORCE)
    set(Vulkan_INCLUDE_DIR "" CACHE PATH "" FORCE)
    set(Vulkan_INCLUDE_DIRS "" CACHE PATH "" FORCE)
endif()

if(NOT TARGET WrapVulkanHeaders::WrapVulkanHeaders)
    add_library(WrapVulkanHeaders::WrapVulkanHeaders INTERFACE IMPORTED)
    set(WrapVulkanHeaders_FOUND TRUE CACHE BOOL "" FORCE)
endif()
