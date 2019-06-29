#version 330 core

uniform sampler2D sprite_sheet;

in vec2 tex_coords;

void main()
{
    gl_FragColor = texture(sprite_sheet, tex_coords);
}
