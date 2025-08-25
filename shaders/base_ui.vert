#version 450 core

layout(location = 0) in vec2 in_vertex;

layout(location = 0) out vec2 out_color;

void main() {
    gl_Position = vec4(in_vertex, 0.0, 1.0);
    out_color = (gl_Position.xy + vec2(1)) / vec2(2);
}
