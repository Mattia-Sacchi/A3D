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

out VS_OUT {
	vec3 WorldPos;
	vec2 TexCoord;
	vec3 Normal;
} vsOut;

void main() {
	vsOut.WorldPos = vec3(mMatrix * vec4(inVertex, 1.0));
	vsOut.TexCoord = inTexCoord;
	vsOut.Normal = mNormalMatrix * inNormal;
	
	gl_Position = projection * view * vec4(vsOut.WorldPos, 1.0);
}
