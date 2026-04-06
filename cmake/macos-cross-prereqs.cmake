# Pre-create imported targets and fix platform flags for macOS cross-compilation
# from Linux using Zig. Included via CMAKE_PROJECT_INCLUDE so it runs after
# CMake's platform detection but before the project's find_package calls.

# ── Fix @rpath support ───────────────────────────────────────────────
# CMake's Darwin platform modules don't fully load during zig cross-compilation,
# leaving these unset. Qt frameworks use @rpath install names and need them.
if(NOT CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG)
    set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath," CACHE INTERNAL "")
endif()
if(NOT CMAKE_SHARED_LIBRARY_RUNTIME_CXX_FLAG)
    set(CMAKE_SHARED_LIBRARY_RUNTIME_CXX_FLAG "-Wl,-rpath," CACHE INTERNAL "")
endif()
if(NOT CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG_SEP)
    set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG_SEP ":" CACHE INTERNAL "")
endif()
if(NOT CMAKE_SHARED_LIBRARY_RUNTIME_CXX_FLAG_SEP)
    set(CMAKE_SHARED_LIBRARY_RUNTIME_CXX_FLAG_SEP ":" CACHE INTERNAL "")
endif()
if(NOT CMAKE_SHARED_LIBRARY_SONAME_C_FLAG)
    set(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "-Wl,-install_name," CACHE INTERNAL "")
endif()
if(NOT CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG)
    set(CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG "-Wl,-install_name," CACHE INTERNAL "")
endif()

# ── OpenGL framework stub ────────────────────────────────────────────
# Qt6Gui depends on WrapOpenGL. On macOS it's just -framework OpenGL.
if(NOT TARGET OpenGL::GL)
    add_library(OpenGL::GL INTERFACE IMPORTED)
    set_target_properties(OpenGL::GL PROPERTIES
        INTERFACE_LINK_LIBRARIES "-framework OpenGL"
    )
    set(OPENGL_FOUND TRUE CACHE BOOL "" FORCE)
    set(OpenGL_FOUND TRUE CACHE BOOL "" FORCE)
    set(OPENGL_gl_LIBRARY "-framework OpenGL" CACHE STRING "" FORCE)
    set(OPENGL_INCLUDE_DIR "" CACHE PATH "" FORCE)
endif()

if(NOT TARGET WrapOpenGL::WrapOpenGL)
    add_library(WrapOpenGL::WrapOpenGL INTERFACE IMPORTED)
    target_link_libraries(WrapOpenGL::WrapOpenGL INTERFACE "-framework OpenGL")
    set(WrapOpenGL_FOUND TRUE CACHE BOOL "" FORCE)
endif()
