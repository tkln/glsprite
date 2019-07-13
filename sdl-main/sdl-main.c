/*
 * Copyright (c) 2019 Aapo Vienamo
 * SPDX-License-Identifier: MIT
 */

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "glutil.h"
#include "stb/stb_image.h"
#include "../glsprite.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define DEG2RAD(deg) (((deg) / 180.0f) * M_PI)

static struct vec2f sprite_positions[] = {
    VECMAT_INIT(100, 100),
    VECMAT_INIT(500, 300),
    VECMAT_INIT(300, 400),
    VECMAT_INIT(200, 200),
};

static struct vec2f sprite_sizes[] = {
    VECMAT_INIT(21, 21),
    VECMAT_INIT(21, 21),
    VECMAT_INIT(21, 21),
    VECMAT_INIT(21, 21),
};

static float sprite_angles[] = {
    0.1f,
    0.0f,
    DEG2RAD(15.0f),
    DEG2RAD(45.0f),
};

static struct vec2f sheet_offsets[] = {
    VECMAT_INIT(2, 2),
    VECMAT_INIT(25, 48),
    VECMAT_INIT(2, 2),
    VECMAT_INIT(2, 2),
};

static struct vec2f sprite_origins[] = {
    VECMAT_INIT(10, 10),
    VECMAT_INIT(10, 10),
    VECMAT_INIT(10, 10),
    VECMAT_INIT(10, 10),
};

int main(void)
{
    SDL_Window *win;
    SDL_GLContext gl_ctx;
    SDL_Event ev;
    bool running = true;

    struct glsprite_renderer renderer;
    struct glsprite_sheet sheet;
    struct glsprite_draw_buffer draw_buffer;

    int sprite_sheet_w, sprite_sheet_h, sprite_sheet_num_channels;
    unsigned char *sprite_sheet_data;
    int err;
    int i;

    GLuint fs_id;
    GLuint vs_id;
    GLuint prog_id;
    GLuint sprite_sheet_tex_id;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    win = SDL_CreateWindow(NULL, SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, 640, 480,
                           SDL_WINDOW_OPENGL);

    gl_ctx = SDL_GL_CreateContext(win);

    glewExperimental = GL_TRUE;
    glewInit();

    SDL_GL_SetSwapInterval(1);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    sprite_sheet_data = stbi_load("spritesheet.png", &sprite_sheet_w,
                                  &sprite_sheet_h, &sprite_sheet_num_channels,
                                  0);

    glGenTextures(1, &sprite_sheet_tex_id);
    glBindTexture(GL_TEXTURE_2D, sprite_sheet_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sprite_sheet_w, sprite_sheet_h, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, sprite_sheet_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    fs_id = glutil_compile_shader_file("../shader/fs.glsl", GL_FRAGMENT_SHADER);
    assert(fs_id);
    vs_id = glutil_compile_shader_file("../shader/vs.glsl", GL_VERTEX_SHADER);
    assert(vs_id);
    prog_id = glutil_link_shaders(glCreateProgram(), fs_id, vs_id);
    assert(prog_id);

    err = glsprite_renderer_init(&renderer, prog_id, 640, 480);
    assert(err == 0);

    glsprite_sheet_init(&sheet, sprite_sheet_tex_id, sprite_sheet_w,
                        sprite_sheet_h);

    glsprite_draw_buffer_init(&draw_buffer, &sheet);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    while (running) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (i = 0; i < 4; ++i)
            glsprite_draw_buffer_push(&draw_buffer, sheet_offsets[i],
                                      sprite_positions[i],
                                      sprite_sizes[i],
                                      sprite_origins[i],
                                      sprite_angles[i]);

        glsprite_render_draw_buffer(&renderer, &draw_buffer);

        glsprite_draw_buffer_clear(&draw_buffer);

        SDL_GL_SwapWindow(win);
        SDL_Delay(16);

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)
                running = 0;
        }
    }

    glsprite_draw_buffer_destroy(&draw_buffer);

    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}
