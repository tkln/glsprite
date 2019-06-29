/*
 * Copyright (c) 2019 Aapo Vienamo
 * SPDX-License-Identifier: MIT
 */

#ifndef GLSPRITE_H
#define GLSPRITE_H

#include <GL/gl.h>

struct vec3 {
    float x, y, z;
};

struct vec2 {
    float x, y;
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

struct glsprite_sheet {
    unsigned width;
    unsigned height;
    GLuint texture_id;
};

struct glsprite_draw_buffer {
    const struct glsprite_sheet *sheet;
    size_t num_sprites;
    size_t num_allocd;
    struct vec2 *sheet_offsets;
    struct vec2 *sprite_positions;
    struct vec2 *sprite_dimensions;
    struct vec2 *sprite_origins;
    float *sprite_angles;
};

int glsprite_renderer_init(struct glsprite_renderer *r, GLuint prog_id,
                           unsigned screen_w, unsigned screen_h);

void glsprite_draw_buffer_init(struct glsprite_draw_buffer *buf,
                               const struct glsprite_sheet *sheet);

void glsprite_draw_buffer_grow(struct glsprite_draw_buffer *buf);

static inline void glsprite_draw_buffer_push(struct glsprite_draw_buffer *buf,
                                             struct vec2 sheet_pos,
                                             struct vec2 sprite_pos,
                                             struct vec2 sprite_dim,
                                             struct vec2 sprite_orig,
                                             float sprite_angle)
{
    size_t i = buf->num_sprites;

    if (buf->num_sprites >= buf->num_allocd || buf->num_allocd == 0)
        glsprite_draw_buffer_grow(buf);

    buf->sheet_offsets[i] = sheet_pos;
    buf->sprite_positions[i] = sprite_pos;
    buf->sprite_dimensions[i] = sprite_dim;
    buf->sprite_origins[i] = sprite_orig;
    buf->sprite_angles[i] = sprite_angle;

    buf->num_sprites = i + 1;
}

void glsprite_render_draw_buffer(const struct glsprite_renderer *rend,
                                 const struct glsprite_draw_buffer *buf);

static inline void glsprite_draw_buffer_clear(struct glsprite_draw_buffer *buf)
{
    buf->num_sprites = 0;
}

void glsprite_draw_buffer_destroy(struct glsprite_draw_buffer *buf);

#endif