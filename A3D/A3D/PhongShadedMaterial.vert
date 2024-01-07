#version 330 core

layout (location = 0) in vec3 inVertex;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;

out vec2 texCoord;

void main() {
	texCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
	gl_Position = mvpMatrix * vec4(inVertex, 1.f);
}
