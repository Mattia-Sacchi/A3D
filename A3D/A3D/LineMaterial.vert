#version 330 core

layout (location = 0) in vec3 inVertex;
layout (location = 3) in vec4 inColor;

out vec4 vs_Color;

void main() {
	gl_Position = vec4(inVertex, 1.0);
	vs_Color = inColor;
}
