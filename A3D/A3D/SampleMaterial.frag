#version 330 core

in vec3 processedVertex;
out vec4 fragColor;

void main() {
	fragColor = vec4(processedVertex, 1.f);
}
