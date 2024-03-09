#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

layout(set = 0, binding = 0) uniform UniformBuffer {
    mat4 project;
    mat4 view;
} ubo;

layout(push_constant) uniform PushConstant {
    mat4 model;
} pc;

void main() {
    gl_Position = ubo.project * ubo.view * pc.model * vec4(inPosition, 0.f, 1.f);
    outTexCoord = inTexCoord;
}