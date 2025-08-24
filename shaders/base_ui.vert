#version 450 core

vec2 vertices[] = vec2[3](
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(0.5, 0.5)
);

layout(location = 0) out vec2 out_color;

void main() {
    gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
    out_color = (gl_Position.xy + vec2(1)) / vec2(2);
}
