# Pre-create imported targets for Windows cross-compilation from Linux
# using Zig. Included via CMAKE_PROJECT_INCLUDE so it runs after CMake's
# platform detection but before the project's find_package calls.

# ── OpenGL import library stub ───────────────────────────────────────
# Qt6Gui depends on WrapOpenGL. On Windows it links against opengl32.dll
# via the -lopengl32 import library provided by the MinGW/Zig toolchain.
if(NOT TARGET OpenGL::GL)
    add_library(OpenGL::GL INTERFACE IMPORTED)
    set_target_properties(OpenGL::GL PROPERTIES
        INTERFACE_LINK_LIBRARIES "-lopengl32"
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
# On the Linux host this finds /usr/include, injecting host system
# headers (-isystem /usr/include) that conflict with Zig's Windows
# libc (e.g. mbstate_t redefinition).  Pre-create the targets with an
# empty include path so CMake never searches the host filesystem.
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
