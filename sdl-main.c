#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "glutil.h"
#include "stb/stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))
#define DEG2RAD(deg) (((deg) / 180.0f) * M_PI)

struct vec3 {
    float x, y, z;
};

static const struct vec3 quad_verts[] = {
    { -1.0f, -1.0f, 0.0f },
    {  1.0f, -1.0f, 0.0f },
    { -1.0f,  1.0f, 0.0f },
    {  1.0f,  1.0f, 0.0f },
};

struct vec2 {
    float x, y;
};

static const struct vec2 sprite_positions[] = {
    { 100, 100 },
    { 500, 300 },
    { 300, 400 },
    { 200, 200 },
};

static const struct vec2 sprite_sizes[] = {
    { 21, 21 },
    { 21, 21 },
    { 21, 21 },
    { 21, 21 },
};

static const float sprite_angles[] = {
    0.1f,
    0.0f,
    DEG2RAD(15.0f),
    DEG2RAD(45.0f),
};

static const struct vec2 sheet_offsets[] = {
    { 2, 2 },
    { 25, 48 },
    { 2, 2 },
    { 2, 2 },
};

static const struct vec2 sprite_origins[] = {
    { 10, 10 },
    { 10, 10 },
    { 10, 10 },
    { 10, 10 },
};

enum {
    VA_IDX_QUAD_VERT,
    VA_IDX_SPRITE_POS,
    VA_IDX_SPRITE_SIZE,
    VA_IDX_SPRITE_ROT,
    VA_IDX_SHEET_OFFSET,
    VA_IDX_SPRITE_ORIGIN,
};

int main(void)
{
    SDL_Window *win;
    SDL_GLContext gl_ctx;
    SDL_Event ev;
    bool running = true;

    GLuint fs_id;
    GLuint vs_id;
    GLuint prog_id;
    GLuint vao_id;
    GLuint quad_verts_vbo_id;
    GLuint sprite_pos_vbo_id;
    GLuint sprite_size_vbo_id;
    GLuint sprite_rot_vbo_id;
    GLuint sheet_offset_vbo_id;
    GLuint sprite_origin_vbo_id;
    GLint screen_size_uniform_loc;
    GLint sheet_size_uniform_loc;
    GLuint sprite_sheet_tex_id;

    int sprite_sheet_w, sprite_sheet_h, sprite_sheet_num_channels;
    unsigned char *sprite_sheet_data;

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

    stbi_set_flip_vertically_on_load(true);
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

    fs_id = glutil_compile_shader_file("fs.glsl", GL_FRAGMENT_SHADER);
    assert(fs_id);
    vs_id = glutil_compile_shader_file("vs.glsl", GL_VERTEX_SHADER);
    assert(vs_id);
    prog_id = glutil_link_shaders(glCreateProgram(), fs_id, vs_id);
    assert(prog_id);

    screen_size_uniform_loc = glGetUniformLocation(prog_id, "screen_size");
    assert(screen_size_uniform_loc != -1);

    sheet_size_uniform_loc = glGetUniformLocation(prog_id, "sheet_size");
    assert(sheet_size_uniform_loc != -1);

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &quad_verts_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, quad_verts_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(VA_IDX_QUAD_VERT, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &sprite_pos_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, sprite_pos_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_positions), sprite_positions,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(VA_IDX_SPRITE_POS, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &sprite_size_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, sprite_size_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_sizes), sprite_sizes,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(VA_IDX_SPRITE_SIZE, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &sprite_rot_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, sprite_rot_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_angles), sprite_angles,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(VA_IDX_SPRITE_ROT, 1, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &sheet_offset_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, sheet_offset_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sheet_offsets), sheet_offsets,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(VA_IDX_SHEET_OFFSET, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &sprite_origin_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, sprite_origin_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_origins), sprite_origins,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(VA_IDX_SPRITE_ORIGIN, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttribDivisor(VA_IDX_QUAD_VERT, 0);
    glVertexAttribDivisor(VA_IDX_SPRITE_POS, 1);
    glVertexAttribDivisor(VA_IDX_SPRITE_SIZE, 1);
    glVertexAttribDivisor(VA_IDX_SPRITE_ROT, 1);
    glVertexAttribDivisor(VA_IDX_SHEET_OFFSET, 1);
    glVertexAttribDivisor(VA_IDX_SPRITE_ORIGIN, 1);

    glEnableVertexAttribArray(VA_IDX_QUAD_VERT);
    glEnableVertexAttribArray(VA_IDX_SPRITE_POS);
    glEnableVertexAttribArray(VA_IDX_SPRITE_SIZE);
    glEnableVertexAttribArray(VA_IDX_SPRITE_ROT);
    glEnableVertexAttribArray(VA_IDX_SHEET_OFFSET);
    glEnableVertexAttribArray(VA_IDX_SPRITE_ORIGIN);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glUseProgram(prog_id);
    glUniform2f(screen_size_uniform_loc, 640, 480);
    glUniform2f(sheet_size_uniform_loc, sprite_sheet_w, sprite_sheet_h);

    while (running) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, ARRAY_LEN(quad_verts),
                              ARRAY_LEN(sprite_positions));

        SDL_GL_SwapWindow(win);
        SDL_Delay(16);

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)
                running = 0;
        }
    }

    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}
