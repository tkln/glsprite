#version 330 core

layout(location = 0) in vec3 quad_vert_pos;
layout(location = 1) in vec2 sprite_pos;

void main() {
    const float scale = 0.25f;
    gl_Position = vec4(quad_vert_pos.xy * scale + sprite_pos, quad_vert_pos.z,
                       1.0f);
}

