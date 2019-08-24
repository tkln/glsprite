/*
 * Copyright (c) 2019 Aapo Vienamo
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <vecmat/vec3f.h>

#include "glsprite.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

static const struct vec3f quad_verts[] = {
    VEC3F_INIT(-1.0f, -1.0f, 0.0f),
    VEC3F_INIT( 1.0f, -1.0f, 0.0f),
    VEC3F_INIT(-1.0f,  1.0f, 0.0f),
    VEC3F_INIT( 1.0f,  1.0f, 0.0f),
};

enum {
    VA_IDX_QUAD_VERT,
    VA_IDX_SPRITE_POS,
    VA_IDX_SPRITE_SIZE,
    VA_IDX_SPRITE_ROT,
    VA_IDX_SHEET_OFFSET,
    VA_IDX_SPRITE_ORIGIN,
};

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

void glsprite_sheet_init(struct glsprite_sheet *sheet, GLuint texture_id,
                         unsigned width, unsigned height)
{
    sheet->texture_id = texture_id;
    sheet->width = width;
    sheet->height = height;
}

void glsprite_grid_init(struct glsprite_grid *grid, unsigned sprite_width,
                        unsigned sprite_height, unsigned margin)
{
    grid->sprite_dims = vec2f_init(sprite_width, sprite_height);
    grid->grid_dims = vec2f_adds(grid->sprite_dims, margin);
    grid->margin = margin;
}

void glsprite_draw_buffer_init(struct glsprite_draw_buffer *buf,
                               const struct glsprite_sheet *sheet)
{
    buf->sheet = sheet;
    buf->num_sprites = 0;
    buf->num_allocd = 0;
    buf->sheet_offsets = NULL;
    buf->sprite_positions = NULL;
    buf->sprite_dimensions = NULL;
    buf->sprite_origins = NULL;
    buf->sprite_angles = NULL;
}

void glsprite_draw_buffer_grow(struct glsprite_draw_buffer *buf)
{
    size_t n = buf->num_allocd;

    if (n == 0)
        buf->num_allocd = 1;

    n = buf->num_allocd * 2;

    buf->sheet_offsets = realloc(buf->sheet_offsets,
                                 sizeof(buf->sheet_offsets[0]) * n);
    buf->sprite_positions = realloc(buf->sprite_positions,
                                    sizeof(buf->sprite_positions[0]) * n);
    buf->sprite_dimensions = realloc(buf->sprite_dimensions,
                                     sizeof(buf->sprite_dimensions[0]) * n);
    buf->sprite_origins = realloc(buf->sprite_origins,
                                  sizeof(buf->sprite_origins[0]) * n);
    buf->sprite_angles = realloc(buf->sprite_angles,
                                 sizeof(buf->sprite_angles[0]) * n);

    buf->num_allocd = n;
}

void glsprite_draw_buffer_push_grid(struct glsprite_draw_buffer *buf,
                                    const struct glsprite_grid *grid,
                                    struct vec2i sprite_idx,
                                    struct vec2f sprite_pos,
                                    struct vec2f sprite_orig,
                                    float sprite_angle)
{
    struct vec2f idx = vec2f_init(sprite_idx.x, sprite_idx.y);
    struct vec2f sheet_pos = vec2f_adds(vec2f_mul(idx, grid->grid_dims),
                                        grid->margin);
    glsprite_draw_buffer_push(buf, sheet_pos, sprite_pos, grid->sprite_dims,
                              sprite_orig, sprite_angle);
}

void glsprite_draw_buffer_destroy(struct glsprite_draw_buffer *buf)
{
    buf->num_sprites = 0;
    buf->num_allocd = 0;
    free(buf->sheet_offsets);
    free(buf->sprite_positions);
    free(buf->sprite_dimensions);
    free(buf->sprite_origins);
    free(buf->sprite_angles);
}

void glsprite_render_draw_buffer(const struct glsprite_renderer *rend,
                                 const struct glsprite_draw_buffer *buf)
{
    size_t n = buf->num_sprites;

    glBindTexture(GL_TEXTURE_2D, buf->sheet->texture_id);
    glUniform2f(rend->sheet_size_uniform_loc, buf->sheet->width,
                buf->sheet->height);

    glBindVertexArray(rend->vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, rend->sprite_pos_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(buf->sprite_positions[0]),
                 buf->sprite_positions, GL_DYNAMIC_DRAW);

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
