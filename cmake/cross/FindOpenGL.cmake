# Custom FindOpenGL for macOS cross-compilation with Zig
# Zig handles framework linking internally, so we just need to tell CMake
# that OpenGL is available and should be linked as a framework.

if(NOT OpenGL_FOUND)
    set(OpenGL_FOUND TRUE)
    set(OPENGL_FOUND TRUE)
    set(OPENGL_gl_LIBRARY "-framework OpenGL" CACHE STRING "" FORCE)
    set(OPENGL_INCLUDE_DIR "" CACHE PATH "" FORCE)
    set(OPENGL_LIBRARIES "-framework OpenGL")

    if(NOT TARGET OpenGL::GL)
        add_library(OpenGL::GL INTERFACE IMPORTED)
        set_target_properties(OpenGL::GL PROPERTIES
            INTERFACE_LINK_LIBRARIES "-framework OpenGL"
        )
    endif()

    if(NOT TARGET OpenGL::OpenGL)
        add_library(OpenGL::OpenGL INTERFACE IMPORTED)
        set_target_properties(OpenGL::OpenGL PROPERTIES
            INTERFACE_LINK_LIBRARIES "-framework OpenGL"
        )
    endif()
endif()
