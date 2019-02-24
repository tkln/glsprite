#version 330 core

in vec4 in_position;

void main() {
   gl_Position = in_position;
   gl_Position.xy *= 0.5;
}

