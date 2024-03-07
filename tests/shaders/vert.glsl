#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;

layout(set = 0, binding = 0) uniform UniformBuffer {
    mat4 project;
    mat4 view;
    mat4 model;
} ubo;

void main() {
    gl_Position = ubo.project * ubo.view * ubo.model * vec4(inPosition, 0.f, 1.f);
}