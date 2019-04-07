#version 330 core

uniform vec2 screen_size;
uniform vec2 sheet_size;
uniform sampler2D sprite_sheet;

flat in vec2 fs_sprite_pos;
flat in vec2 fs_sprite_size;
flat in float fs_sprite_rot;

void main()
{
    mat2 rot = mat2(cos(-fs_sprite_rot), sin(-fs_sprite_rot),
                    -sin(-fs_sprite_rot), cos(-fs_sprite_rot));
    vec2 sample_pos = (gl_FragCoord.xy - fs_sprite_pos) * rot / sheet_size;

    gl_FragColor = texture(sprite_sheet, sample_pos);
}
