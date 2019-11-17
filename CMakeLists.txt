add_subdirectory(vecmat)

if(DEFINED glsprite_GLEXT_HEADER)
    target_compile_definitions(glsprite PRIVATE
        GLSPRITE_GLEXT_HEADER=${glsprite_GLEXT_HEADER})
    target_compile_definitions(glsprite PRIVATE
        GLSPRITE_GLEXT_HEADER=${glsprite_GLEXT_HEADER})
endif()

add_library(
        glsprite
    STATIC
        glsprite.c
)

target_include_directories(
        glsprite
    INTERFACE
        .
)

target_link_libraries(
        glsprite
    PUBLIC
        vecmat
)

add_library(
        glsprite-glutil
    STATIC
        sdl-main/glutil.c
)

target_include_directories(
        glsprite-glutil
    PUBLIC
        sdl-main/
)
