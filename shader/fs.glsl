#version 330 core

uniform sampler2D sprite_sheet;

in vec2 tex_coords;

out vec4 fragColor;

void main()
{
    fragColor = texture(sprite_sheet, tex_coords);
}
