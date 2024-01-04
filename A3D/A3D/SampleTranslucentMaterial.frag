#version 330 core

in vec3 inVxPos;
out vec4 fragColor;

void main() {
	fragColor = vec4(inVxPos, 0.3f);
}
