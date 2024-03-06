#version 460
#pragma shader_stage(vertex)

vec2 position[3] = {
{ 0, -.5f },
{ -.5f, .5f },
{ .5f, .5f },
};

void main() {
    gl_Position = vec4(position[gl_VertexIndex], 0, 1);
}
