#version 330 core

layout (location = 0) in vec3 inVertex;
uniform mat4 mvpMatrix;
out vec3 inVxPos;

void main() {
	inVxPos = inVertex;
	gl_Position = mvpMatrix * vec4(inVertex, 1.f);
}
