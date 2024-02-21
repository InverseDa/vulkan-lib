#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

layout(location = 0)out vec4 vColor;

void main() {
    gl_Position = vec4(position, 0, 1);
    vColor = color;
}
