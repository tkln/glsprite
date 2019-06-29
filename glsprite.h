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

void glsprite_render_draw_buffer(const struct glsprite_renderer *rend,
                                 const struct glsprite_draw_buffer *buf);

#endif
