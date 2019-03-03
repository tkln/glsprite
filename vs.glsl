#version 330 core

uniform vec2 screen_size;

layout(location = 0) in vec3 quad_vert_pos;
layout(location = 1) in vec2 sprite_pos;

void main() {
    const float scale = 0.25f;
    float aspect_ratio = screen_size.x / screen_size.y;
    vec2 qvp = quad_vert_pos.xy * vec2(1, aspect_ratio) * scale;
    vec2 sp = sprite_pos / (screen_size * 0.5f) - 1.0f;

    gl_Position = vec4(qvp + sp, quad_vert_pos.z, 1.0f);
}

