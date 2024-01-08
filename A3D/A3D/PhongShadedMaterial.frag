#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D TextureSlot0;

/*
layout (std140) uniform ShaderData {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float opacity;
	float specularExponent;
	float paddingA;
	float paddingB;
};*/

void main() {
	vec4 texDiffuseColor = texture(TextureSlot0, texCoord);
	fragColor = texDiffuseColor;
}
