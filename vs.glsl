#version 330 core

uniform vec2 screen_size;

layout(location = 0) in vec3 quad_vert_pos;
layout(location = 1) in vec2 sprite_pos;

void main() {
    const float scale = 0.25f;
    float aspect_ratio = screen_size.x / screen_size.y;

    gl_Position = vec4(quad_vert_pos.xy * vec2(1, aspect_ratio) * scale +
                       sprite_pos, quad_vert_pos.z, 1.0f);
}

