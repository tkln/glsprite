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
