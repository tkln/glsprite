#version 330 core

uniform vec2 screen_size;
uniform vec2 sheet_size;

layout(location = 0) in vec3 quad_vert_pos;
layout(location = 1) in vec2 sprite_pos;
layout(location = 2) in vec2 sprite_size;
layout(location = 3) in float sprite_rot;

out vec2 tex_coords;

void main() {
    mat2 rot = mat2(cos(sprite_rot), sin(sprite_rot),
                    -sin(sprite_rot), cos(sprite_rot));
    vec2 qvp = quad_vert_pos.xy * sprite_size * rot / screen_size;
    vec2 sp = sprite_pos / (screen_size * 0.5f) - 1.0f;

    gl_Position = vec4(qvp + sp, quad_vert_pos.z, 1.0f);
    tex_coords = quad_vert_pos.xy * sprite_size / (sheet_size * 2);
}
