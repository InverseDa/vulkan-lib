#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec4 inColor;
layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = inColor;
}
