#version 330 core

layout (location = 0) in vec3 inVertex;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;

layout (std140) uniform MeshUBO_Data {
	mat4 pMatrix;
	mat4 vMatrix;
	mat4 mMatrix;

	mat4 mvMatrix;
	mat4 mvpMatrix;

	mat4 mNormalMatrix;
	mat4 mvNormalMatrix;
	mat4 mvpNormalMatrix;
};

out vec3 WorldPos;

void main() {
	WorldPos = inVertex;

	mat4 rotView = mat4(mat3(vMatrix));
	vec4 clipPos = pMatrix * rotView * vec4(inVertex, 1.0);
	gl_Position = clipPos;
}
