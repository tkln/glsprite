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

struct glsprite_draw_buffer {
    size_t num_sprites;
    struct vec2 *sheet_offsets;
    struct vec2 *sprite_positions;
    struct vec2 *sprite_dimensions;
    struct vec2 *sprite_origins;
    float *sprite_angles;
};

static struct vec2 sprite_positions[] = {
    { 100, 100 },
    { 500, 300 },
    { 300, 400 },
    { 200, 200 },
};

static struct vec2 sprite_sizes[] = {
    { 21, 21 },
    { 21, 21 },
    { 21, 21 },
    { 21, 21 },
};

static float sprite_angles[] = {
    0.1f,
    0.0f,
    DEG2RAD(15.0f),
    DEG2RAD(45.0f),
};

static struct vec2 sheet_offsets[] = {
    { 2, 2 },
    { 25, 48 },
    { 2, 2 },
    { 2, 2 },
};

static struct vec2 sprite_origins[] = {
    { 10, 10 },
    { 10, 10 },
    { 10, 10 },
    { 10, 10 },
};

static struct glsprite_draw_buffer draw_buffer = {
    .num_sprites = 4,
    .sheet_offsets = sheet_offsets,
    .sprite_positions = sprite_positions,
    .sprite_dimensions = sprite_sizes,
    .sprite_origins = sprite_origins,
    .sprite_angles = sprite_angles,
};

enum {
    VA_IDX_QUAD_VERT,
    VA_IDX_SPRITE_POS,
    VA_IDX_SPRITE_SIZE,
    VA_IDX_SPRITE_ROT,
    VA_IDX_SHEET_OFFSET,
    VA_IDX_SPRITE_ORIGIN,
};

struct glsprite_renderer {
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
};

void glsprite_render_draw_buffer(const struct glsprite_renderer *rend,
                                 const struct glsprite_draw_buffer *buf)
{
    size_t n = buf->num_sprites;

    glBindVertexArray(rend->vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, rend->sprite_pos_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(buf->sprite_positions[0]),
                 sprite_positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, rend->sprite_size_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(buf->sprite_dimensions[0]),
                 buf->sprite_dimensions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, rend->sprite_rot_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(buf->sprite_angles[0]),
                 buf->sprite_angles, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, rend->sheet_offset_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(buf->sheet_offsets[0]),
                 buf->sheet_offsets, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, rend->sprite_origin_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(buf->sprite_origins[0]),
                 buf->sprite_origins, GL_DYNAMIC_DRAW);

    glUseProgram(rend->prog_id);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, ARRAY_LEN(quad_verts), n);
}

int glsprite_renderer_init(struct glsprite_renderer *r, GLuint prog_id,
                           unsigned screen_w, unsigned screen_h)
{
    r->prog_id = prog_id;
    glUseProgram(prog_id);

    r->screen_size_uniform_loc = glGetUniformLocation(prog_id, "screen_size");
    if (r->screen_size_uniform_loc < 0)
        return -1;

    r->sheet_size_uniform_loc = glGetUniformLocation(prog_id, "sheet_size");
    if (r->sheet_size_uniform_loc < 0)
        return -1;

    glUniform2f(r->screen_size_uniform_loc, screen_w, screen_h);

    glGenVertexArrays(1, &r->vao_id);
    glBindVertexArray(r->vao_id);

    glGenBuffers(1, &r->quad_verts_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, r->quad_verts_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(VA_IDX_QUAD_VERT, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &r->sprite_pos_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, r->sprite_pos_vbo_id);
    glVertexAttribPointer(VA_IDX_SPRITE_POS, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &r->sprite_size_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, r->sprite_size_vbo_id);
    glVertexAttribPointer(VA_IDX_SPRITE_SIZE, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &r->sprite_rot_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, r->sprite_rot_vbo_id);
    glVertexAttribPointer(VA_IDX_SPRITE_ROT, 1, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &r->sheet_offset_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, r->sheet_offset_vbo_id);
    glVertexAttribPointer(VA_IDX_SHEET_OFFSET, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &r->sprite_origin_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, r->sprite_origin_vbo_id);
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

    return 0;
}

int main(void)
{
    SDL_Window *win;
    SDL_GLContext gl_ctx;
    SDL_Event ev;
    bool running = true;

    struct glsprite_renderer renderer;

    int sprite_sheet_w, sprite_sheet_h, sprite_sheet_num_channels;
    unsigned char *sprite_sheet_data;
    int err;

    GLuint fs_id;
    GLuint vs_id;
    GLuint prog_id;

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

    sprite_sheet_data = stbi_load("spritesheet.png", &sprite_sheet_w,
                                  &sprite_sheet_h, &sprite_sheet_num_channels,
                                  0);

    GLuint sprite_sheet_tex_id;

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

    err = glsprite_renderer_init(&renderer, prog_id, 640, 480);
    assert(err == 0);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glUniform2f(renderer.sheet_size_uniform_loc, sprite_sheet_w,
                sprite_sheet_h);

    while (running) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glsprite_render_draw_buffer(&renderer, &draw_buffer);

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
